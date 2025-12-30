@echo off
chcp 65001 > nul
setlocal enabledelayedexpansion

rem --- 编译 ---
if not exist build\classes mkdir build\classes

set "LWJGL_CP=."
if exist "lib\lwjgl" (
  set "LWJGL_CP=.;lib\lwjgl\*"
)

echo 收集源码文件...
(for /R src\main\java %%f in (*.java) do @echo %%f) > build\sources.txt

echo 编译中...
javac -d build\classes -cp "%LWJGL_CP%" @build\sources.txt
if errorlevel 1 (
  echo 编译失败
  exit /b 1
)

set "CLASSPATH=build\classes"
if exist "lib\lwjgl" (
  set "CLASSPATH=%CLASSPATH%;lib\lwjgl\*"
)

rem --- natives ---
set "ARCH_RAW=%PROCESSOR_ARCHITECTURE%"
if /i "%ARCH_RAW%"=="AMD64" set ARCH_ID=x86_64
if /i "%ARCH_RAW%"=="ARM64" set ARCH_ID=arm64
if not defined ARCH_ID set ARCH_ID=x86_64

set OS_ID=windows
set "NATIVE_DIR=lib\lwjgl\natives\%OS_ID%-%ARCH_ID%"
set "JAVA_FLAGS="

if exist "%NATIVE_DIR%" (
  set "JAVA_FLAGS=-Dorg.lwjgl.librarypath=%NATIVE_DIR%"
)

set "HOST=%~1"
if "%HOST%"=="" set "HOST=127.0.0.1"

echo 运行客户端，连接到 %HOST% ...
java %JAVA_FLAGS% -cp "%CLASSPATH%" com.gameengine.example.ClientLauncher "%HOST%"

endlocal
