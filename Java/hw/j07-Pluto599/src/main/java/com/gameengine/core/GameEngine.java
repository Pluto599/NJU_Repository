package com.gameengine.core;

import com.gameengine.graphics.Renderer;
import com.gameengine.input.InputManager;
import com.gameengine.scene.Scene;
import javax.swing.Timer;

/**
 * 游戏引擎
 */
public class GameEngine {
    private Renderer renderer;
    private InputManager inputManager;
    private Scene currentScene;
    private boolean running;
    private float targetFPS;
    private float deltaTime;
    private long lastTime;
    private String title;
    private Timer gameTimer;
    private final Thread shutdownHook;
    private volatile boolean shutdownHandled;
    private boolean shutdownHookRegistered;

    public GameEngine(int width, int height, String title) {
        this.title = title;
        this.renderer = new Renderer(width, height, title);
        this.inputManager = InputManager.getInstance();
        this.running = false;
        this.targetFPS = 60.0f;
        this.deltaTime = 0.0f;
        this.lastTime = System.nanoTime();
        this.shutdownHandled = false;
        this.shutdownHook = new Thread(() -> {
            // JVM 关闭时（例如 Ctrl+C），先停掉游戏循环，避免 Timer 线程继续调用已清理的资源。
            running = false;
            if (gameTimer != null) {
                try {
                    gameTimer.stop();
                } catch (Exception ignored) {
                }
            }
            notifySceneExit();
        });
        this.shutdownHook.setName("GameEngine-ShutdownHook");
        Runtime.getRuntime().addShutdownHook(this.shutdownHook);
        this.shutdownHookRegistered = true;
    }

    /**
     * 初始化游戏引擎
     */
    public boolean initialize() {
        return true; // Swing渲染器不需要特殊初始化
    }

    /**
     * 运行游戏引擎
     */
    public void run() {
        if (!initialize()) {
            System.err.println("游戏引擎初始化失败");
            return;
        }

        if (!shutdownHookRegistered) {
            try {
                Runtime.getRuntime().addShutdownHook(shutdownHook);
                shutdownHookRegistered = true;
            } catch (IllegalArgumentException ignored) {
                // 钩子已注册
            }
        }
        shutdownHandled = false;
        running = true;

        // 初始化当前场景
        if (currentScene != null) {
            currentScene.initialize();
        }

        // 创建游戏循环定时器
        gameTimer = new Timer((int) (1000 / targetFPS), e -> {
            if (running) {
                update();
                render();
            }
        });

        gameTimer.start();
    }

    /**
     * 更新游戏逻辑
     */
    private void update() {
        // 计算时间间隔
        long currentTime = System.nanoTime();
        deltaTime = (currentTime - lastTime) / 1_000_000_000.0f; // 转换为秒
        lastTime = currentTime;

        // 更新场景
        if (currentScene != null) {
            currentScene.update(deltaTime);
        }

        // 处理事件
        renderer.pollEvents();

        // 检查窗口是否关闭
        if (renderer.shouldClose()) {
            notifySceneExit();
            running = false;
            gameTimer.stop();
            cleanup();
        }

        // 更新输入快照，供下一帧使用
        inputManager.update();
    }

    /**
     * 渲染游戏
     */
    private void render() {
        renderer.beginFrame();

        // 渲染场景
        if (currentScene != null) {
            currentScene.render();
        }

        renderer.endFrame();
    }

    /**
     * 设置当前场景
     */
    public void setScene(Scene scene) {
        Scene previous = this.currentScene;
        if (previous != null && previous != scene) {
            try {
                previous.onExit();
            } catch (Exception ignored) {
            }
            try {
                previous.clear();
            } catch (Exception ignored) {
            }
        }

        this.currentScene = scene;
        if (scene != null && running) {
            scene.initialize();
        }
    }

    /**
     * 获取当前场景
     */
    public Scene getCurrentScene() {
        return currentScene;
    }

    /**
     * 停止游戏引擎
     */
    public void stop() {
        notifySceneExit();
        running = false;
        if (gameTimer != null) {
            gameTimer.stop();
        }
        cleanup();
    }

    /**
     * 清理资源
     */
    private void cleanup() {
        if (currentScene != null) {
            currentScene.clear();
        }
        renderer.cleanup();
        removeShutdownHook();
    }

    private void notifySceneExit() {
        Scene sceneToNotify;
        synchronized (this) {
            if (shutdownHandled) {
                return;
            }
            shutdownHandled = true;
            sceneToNotify = currentScene;
        }
        if (sceneToNotify != null) {
            sceneToNotify.onExit();
        }
    }

    private void removeShutdownHook() {
        if (!shutdownHookRegistered) {
            return;
        }
        try {
            boolean removed = Runtime.getRuntime().removeShutdownHook(shutdownHook);
            if (removed) {
                shutdownHookRegistered = false;
            }
        } catch (IllegalStateException ignored) {
            // JVM 正在关闭，无法移除钩子
        }
    }

    /**
     * 获取渲染器
     */
    public Renderer getRenderer() {
        return renderer;
    }

    /**
     * 获取输入管理器
     */
    public InputManager getInputManager() {
        return inputManager;
    }

    /**
     * 获取时间间隔
     */
    public float getDeltaTime() {
        return deltaTime;
    }

    /**
     * 设置目标帧率
     */
    public void setTargetFPS(float fps) {
        this.targetFPS = fps;
        if (gameTimer != null) {
            gameTimer.setDelay((int) (1000 / fps));
        }
    }

    /**
     * 获取目标帧率
     */
    public float getTargetFPS() {
        return targetFPS;
    }

    /**
     * 检查引擎是否正在运行
     */
    public boolean isRunning() {
        return running;
    }
}
