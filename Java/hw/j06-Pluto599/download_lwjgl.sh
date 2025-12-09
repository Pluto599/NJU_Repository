#!/bin/bash
set -e

LWJGL_VERSION="3.3.6"
# 这里的路径要指到 artifactId 对应的子目录，例如 org/lwjgl/lwjgl
MAVEN_REPO="https://repo1.maven.org/maven2/org/lwjgl"
LIB_DIR="lib/lwjgl"

detect_platform() {
  case "$(uname -s)" in
    Darwin*) OS="macos";;
    Linux*)  OS="linux";;
    MINGW*|MSYS*|CYGWIN*|Windows*) OS="windows";;
    *) echo "Unsupported OS: $(uname -s)"; exit 1;;
  esac
  case "$(uname -m)" in
    arm64|aarch64) ARCH="arm64";;
    x86_64) ARCH="x86_64";;
    *) echo "Unsupported ARCH: $(uname -m)"; exit 1;;
  esac
}

fetch() {
  local url="$1" dest="$2"
  mkdir -p "$(dirname "$dest")"
  if command -v curl >/dev/null 2>&1; then
    # -f 如果 404 会直接失败，避免把 HTML 存成 jar
    curl -fLs -o "$dest" "$url"
  elif command -v wget >/dev/null 2>&1; then
    wget -q -O "$dest" "$url"
  else
    echo "Need curl or wget"; exit 1
  fi
}

download_module() {
  local module="$1" classifier="$2"
  # module 为空时 artifactId 是 lwjgl，否则是 lwjgl-glfw 等
  local artifact="lwjgl${module}"
  local base="${MAVEN_REPO}/${artifact}/${LWJGL_VERSION}/${artifact}-${LWJGL_VERSION}"

  echo "Downloading ${artifact} ..."
  fetch "${base}.jar" "${LIB_DIR}/${artifact}-${LWJGL_VERSION}.jar"

  # Maven 中 natives classifier 是 natives-<os>，不含 arch
  local natives_classifier="natives-${OS}"
  echo "Downloading ${artifact} (${natives_classifier}) ..."
  fetch "${base}-${natives_classifier}.jar" \
        "${LIB_DIR}/${artifact}-${LWJGL_VERSION}-${natives_classifier}.jar"
}

extract_natives() {
  local classifier="$1"
  # 这里 classifier 沿用原逻辑，用于目录名，不影响下载
  local natives_dir="${LIB_DIR}/natives/${classifier}"
  mkdir -p "$natives_dir"
  tmp="${LIB_DIR}/.tmp"
  mkdir -p "$tmp"

  # 匹配 natives-<os>.jar
  for jar in "${LIB_DIR}"/*-natives-"${OS}".jar; do
    [ -f "$jar" ] || continue
    rm -rf "$tmp"/*
    if command -v unzip >/dev/null 2>&1; then
      unzip -q -o "$jar" -d "$tmp" || true
    elif command -v jar >/dev/null 2>&1; then
      (cd "$tmp" && jar xf "$jar") || true
    fi
    if [ -d "$tmp/$OS/$ARCH/org/lwjgl" ]; then
      mkdir -p "$natives_dir/org"
      cp -r "$tmp/$OS/$ARCH/org/lwjgl" "$natives_dir/org/" 2>/dev/null || true
    elif [ -d "$tmp/org/lwjgl" ]; then
      mkdir -p "$natives_dir/org"
      cp -r "$tmp/org/lwjgl" "$natives_dir/org/" 2>/dev/null || true
    fi
  done
  rm -rf "$tmp"
}

main() {
  detect_platform
  classifier="${OS}-${ARCH}"
  mkdir -p "$LIB_DIR"
  for m in "" "-glfw" "-opengl"; do
    download_module "$m" "$classifier"
  done
  extract_natives "$classifier"
  echo "LWJGL ${LWJGL_VERSION} downloaded to ${LIB_DIR}"
}

main "$@"