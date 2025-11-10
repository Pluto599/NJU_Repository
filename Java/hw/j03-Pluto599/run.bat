@echo off
chcp 65001 > nul

echo Starting game engine...

REM Compile
call compile.bat

if %errorlevel% equ 0 (
    echo Running game...
    java -cp build/classes com.gameengine.example.GameExample
) else (
    echo Compilation failed, cannot run game
    exit /b 1
)
