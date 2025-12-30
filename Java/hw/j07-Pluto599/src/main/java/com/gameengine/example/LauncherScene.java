package com.gameengine.example;

import com.gameengine.core.GameEngine;
import com.gameengine.input.InputManager;
import com.gameengine.graphics.Renderer;
import com.gameengine.scene.OnlineServerHostScene;
import com.gameengine.scene.Scene;

import java.awt.event.KeyEvent;

public class LauncherScene extends Scene {
    private final GameEngine engine;
    private final Renderer renderer;
    private final int width;
    private final int height;
    private final InputManager input;

    private final String[] options = { "单机模式", "联机服务器", "退出" };
    private int optionIndex = 0;

    public LauncherScene(GameEngine engine, int width, int height) {
        super("Launcher");
        this.engine = engine;
        this.renderer = engine.getRenderer();
        this.width = width;
        this.height = height;
        this.input = engine.getInputManager();
    }

    @Override
    public void update(float deltaTime) {
        super.update(deltaTime);

        if (input.isKeyJustPressed(KeyEvent.VK_ESCAPE)) {
            engine.stop();
            return;
        }

        if (input.isKeyJustPressed(KeyEvent.VK_UP)) {
            optionIndex = (optionIndex - 1 + options.length) % options.length;
        } else if (input.isKeyJustPressed(KeyEvent.VK_DOWN)) {
            optionIndex = (optionIndex + 1) % options.length;
        } else if (input.isKeyJustPressed(KeyEvent.VK_ENTER) || input.isKeyJustPressed(KeyEvent.VK_SPACE)) {
            if (optionIndex == 0) {
                engine.setScene(new OfflineMenuScene(engine, width, height));
                return;
            }
            if (optionIndex == 1) {
                engine.setScene(new OnlineServerHostScene(engine, 7777));
                return;
            }
            engine.stop();
        }
    }

    @Override
    public void render() {
        renderer.drawRect(0, 0, width, height, 0.08f, 0.08f, 0.1f, 1.0f);

        String title = "J07";
        renderer.drawText(title, width / 2f - title.length() * 14f, 120, 32f, 1f, 1f, 1f, 1f);

        float startY = 260f;
        float highlightWidth = 360f;
        float highlightHeight = 56f;
        for (int i = 0; i < options.length; i++) {
            String option = options[i];
            float textWidth = option.length() * 24f;
            float x = width / 2f - textWidth / 2f;
            float y = startY + i * 64f;
            if (i == optionIndex) {
                float rectX = width / 2f - highlightWidth / 2f;
                renderer.drawRect(rectX, y - highlightHeight / 2f, highlightWidth, highlightHeight, 0.2f, 0.3f, 0.6f,
                        0.6f);
            }
            renderer.drawText(option, x, y, 24f, 1f, 1f, 1f, 1f);
        }

        String hint = "↑↓ 选择，Enter 确认，ESC 退出";
        renderer.drawText(hint, 40, height - 60, 18f, 0.8f, 0.8f, 0.8f, 1f);

        super.render();
    }
}
