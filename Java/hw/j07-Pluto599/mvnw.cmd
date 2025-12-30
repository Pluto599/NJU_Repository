@echo off
setlocal

set WRAPPER_JAR=%~dp0.mvn\wrapper\maven-wrapper.jar
if not exist "%WRAPPER_JAR%" (
  echo [mvnw] Missing %WRAPPER_JAR%
  echo [mvnw] Please run: powershell -ExecutionPolicy Bypass -File .\scripts\download-maven-wrapper.ps1
  exit /b 1
)

if "%JAVA_HOME%"=="" (
  set JAVA_EXE=java
) else (
  set JAVA_EXE=%JAVA_HOME%\bin\java.exe
)

"%JAVA_EXE%" -classpath "%WRAPPER_JAR%" -Dmaven.multiModuleProjectDirectory=%~dp0 org.apache.maven.wrapper.MavenWrapperMain %*
