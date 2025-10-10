@echo off
chcp 65001 > nul

echo 启动游戏引擎...

REM 编译
call compile.bat

if %errorlevel% equ 0 (
    echo 运行游戏...
    java -cp build\classes com.gameengine.example.GameExample
) else (
    echo 编译失败，无法运行游戏
    exit /b 1
)