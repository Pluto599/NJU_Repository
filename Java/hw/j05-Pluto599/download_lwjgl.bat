@echo off
setlocal enabledelayedexpansion

set LWJGL_VERSION=3.3.6
set MAVEN_REPO=https://repo1.maven.org/maven2/org/lwjgl
set LIB_DIR=lib\lwjgl

rem --- 检测架构 ---
set "ARCH_RAW=%PROCESSOR_ARCHITECTURE%"
if /i "%ARCH_RAW%"=="AMD64" set ARCH_ID=x86_64
if /i "%ARCH_RAW%"=="ARM64" set ARCH_ID=arm64
if not defined ARCH_ID (
  echo Unsupported ARCH: %ARCH_RAW%
  exit /b 1
)

rem 固定 OS=windows
set OS_ID=windows
set CLASSIFIER=%OS_ID%-%ARCH_ID%

rem 创建目录
if not exist "%LIB_DIR%" mkdir "%LIB_DIR%"
if errorlevel 1 (
  echo 无法创建目录 %LIB_DIR%
  exit /b 1
)

rem ---- 下载函数 ----
rem 优先使用 curl，其次 powershell
set "CURL_AVAILABLE="
where curl >nul 2>nul && set CURL_AVAILABLE=1

:download_module
rem %1 = 模块后缀 (可能为空、-glfw、-opengl)
set "MOD=%~1"
set "BASE=%MAVEN_REPO%/lwjgl%MOD%/%LWJGL_VERSION%/lwjgl%MOD%-%LWJGL_VERSION%"
set "JAR_URL=%BASE%.jar"
set "NATIVES_URL=%BASE%-natives-%CLASSIFIER%.jar"
set "JAR_DEST=%LIB_DIR%\lwjgl%MOD%-%LWJGL_VERSION%.jar"
set "NAT_DEST=%LIB_DIR%\lwjgl%MOD%-%LWJGL_VERSION%-natives-%CLASSIFIER%.jar"

echo 获取 %JAR_URL%
if defined CURL_AVAILABLE (
  curl -Ls -o "%JAR_DEST%" "%JAR_URL%"
) else (
  powershell -Command "(New-Object Net.WebClient).DownloadFile('%JAR_URL%', '%JAR_DEST%')"
)
if not exist "%JAR_DEST%" (
  echo 下载失败: %JAR_URL%
  exit /b 1
)

echo 获取 %NATIVES_URL%
if defined CURL_AVAILABLE (
  curl -Ls -o "%NAT_DEST%" "%NATIVES_URL%"
) else (
  powershell -NoProfile -Command "try { (New-Object Net.WebClient).DownloadFile('%NATIVES_URL%','%NAT_DEST%') } catch { exit 1 }"
)

rem 如果文件存在但很小（常为 HTML 错误页），尝试回退到不含 arch 的 windows classifier
if exist "%NAT_DEST%" (
  for %%I in ("%NAT_DEST%") do set "NATSIZE=%%~zI"
  if defined NATSIZE (
    if %NATSIZE% LSS 1024 (
      echo 下载的文件体积异常 (%NATSIZE% bytes)，尝试回退到不含 arch 的 classifier
      del /f /q "%NAT_DEST%" >nul 2>nul
      set "ALT_NATIVES_URL=%BASE%-natives-%OS_ID%.jar"
      set "ALT_NAT_DEST=%LIB_DIR%\lwjgl%MOD%-%LWJGL_VERSION%-natives-%OS_ID%.jar"
      echo 尝试获取 %ALT_NATIVES_URL%
      if defined CURL_AVAILABLE (
        curl -Ls -o "%ALT_NAT_DEST%" "%ALT_NATIVES_URL%"
      ) else (
        powershell -NoProfile -Command "try { (New-Object Net.WebClient).DownloadFile('%ALT_NATIVES_URL%','%ALT_NAT_DEST%') } catch { exit 1 }"
      )
      if exist "%ALT_NAT_DEST%" (
        set "NAT_DEST=%ALT_NAT_DEST%"
      ) else (
        echo 回退下载失败: %ALT_NATIVES_URL%
        exit /b 1
      )
    )
  )
) else (
  echo 下载失败: %NATIVES_URL%
  exit /b 1
)
goto :eof

rem ---- 下载模块集合 ----
call :download_module ""
call :download_module "-glfw"
call :download_module "-opengl"

rem ---- 解压 natives ----
set "NATIVES_DIR=%LIB_DIR%\natives\%CLASSIFIER%"
if not exist "%NATIVES_DIR%" mkdir "%NATIVES_DIR%"
set "TMP_DIR=%LIB_DIR%\.tmp"
if not exist "%TMP_DIR%" mkdir "%TMP_DIR%"

for %%F in ("%LIB_DIR%\*natives-%CLASSIFIER%.jar") do (
  echo 解压 %%~nxF
  rmdir /s /q "%TMP_DIR%\extract" 2>nul
  mkdir "%TMP_DIR%\extract"
  powershell -Command "Expand-Archive -LiteralPath '%%~fF' -DestinationPath '%TMP_DIR%\extract' -Force" >nul 2>nul
  rem 两种可能的结构：windows/x86_64/org/lwjgl 或 org/lwjgl
  if exist "%TMP_DIR%\extract\%OS_ID%\%ARCH_ID%\org\lwjgl" (
    xcopy /e /y "%TMP_DIR%\extract\%OS_ID%\%ARCH_ID%\org\lwjgl" "%NATIVES_DIR%\org\lwjgl\" >nul
  ) else if exist "%TMP_DIR%\extract\org\lwjgl" (
    xcopy /e /y "%TMP_DIR%\extract\org\lwjgl" "%NATIVES_DIR%\org\lwjgl\" >nul
  )
)

echo LWJGL %LWJGL_VERSION% 下载完成 -> %LIB_DIR%
echo Natives: %NATIVES_DIR%
endlocal