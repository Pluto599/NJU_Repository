@echo off
chcp 65001 > nul

REM Simple compilation script
echo Compiling game engine...

REM Create output directory
if not exist build\classes mkdir build\classes

REM Compile all Java files
javac -encoding UTF-8 -d build/classes ^
    -cp . ^
    src/main/java/com/gameengine/math/Vector2.java ^
    src/main/java/com/gameengine/input/InputManager.java ^
    src/main/java/com/gameengine/core/Component.java ^
    src/main/java/com/gameengine/core/GameObject.java ^
    src/main/java/com/gameengine/components/TransformComponent.java ^
    src/main/java/com/gameengine/components/PhysicsComponent.java ^
    src/main/java/com/gameengine/components/RenderComponent.java ^
    src/main/java/com/gameengine/graphics/Renderer.java ^
    src/main/java/com/gameengine/core/GameEngine.java ^
    src/main/java/com/gameengine/objects/Bullet.java ^
    src/main/java/com/gameengine/objects/Player.java ^
    src/main/java/com/gameengine/objects/Enemy.java ^
    src/main/java/com/gameengine/core/GameLogic.java ^
    src/main/java/com/gameengine/scene/Scene.java ^
    src/main/java/com/gameengine/example/GameExample.java


if %errorlevel% equ 0 (
    echo Compilation successful!
) else (
    echo Compilation failed!
    exit /b 1
)
