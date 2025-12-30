@echo off
chcp 65001 > nul
setlocal enabledelayedexpansion

rem --- 编译一次，然后启动 server + client ---
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

echo 启动 server（新窗口）...
start "J07 Server" cmd /c java %JAVA_FLAGS% -cp "%CLASSPATH%" com.gameengine.example.GameExample server

echo 等待 server 启动...
timeout /t 1 /nobreak > nul

echo 启动 client（本窗口）...
java %JAVA_FLAGS% -cp "%CLASSPATH%" com.gameengine.example.ClientLauncher 127.0.0.1

endlocal
