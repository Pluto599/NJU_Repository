package com.gameengine.example;

import com.gameengine.core.GameEngine;
import com.gameengine.graphics.Renderer;
import com.gameengine.input.InputManager;
import com.gameengine.save.GameSaveService;
import com.gameengine.scene.OfflineGameScene;
import com.gameengine.scene.Scene;

import java.awt.event.KeyEvent;
import java.nio.file.Path;
import java.util.Collections;
import java.util.List;

public class OfflineMenuScene extends Scene {
    private static final String SAVE_DIRECTORY = "recordings";

    private final GameEngine engine;
    private final Renderer renderer;
    private final int width;
    private final int height;
    private final InputManager input;

    private final String[] mainOptions = { "开始游戏", "继续游戏" };
    private int optionIndex = 0;

    private boolean selectingSave = false;
    private List<Path> saveFiles = Collections.emptyList();
    private int saveIndex = 0;
    private String message = "";

    public OfflineMenuScene(GameEngine engine, int width, int height) {
        super("OfflineMenu");
        this.engine = engine;
        this.renderer = engine.getRenderer();
        this.width = width;
        this.height = height;
        this.input = engine.getInputManager();
    }

    @Override
    public void update(float deltaTime) {
        super.update(deltaTime);

        // 在“选择存档”等菜单时按 ESC 返回开始界面（Launcher）
        if (input.isKeyJustPressed(KeyEvent.VK_ESCAPE)) {
            engine.setScene(new LauncherScene(engine, width, height));
            return;
        }

        if (selectingSave) {
            handleSaveSelection();
        } else {
            handleMainMenu();
        }
    }

    private void handleMainMenu() {
        if (input.isKeyJustPressed(KeyEvent.VK_UP)) {
            optionIndex = (optionIndex - 1 + mainOptions.length) % mainOptions.length;
        } else if (input.isKeyJustPressed(KeyEvent.VK_DOWN)) {
            optionIndex = (optionIndex + 1) % mainOptions.length;
        } else if (input.isKeyJustPressed(KeyEvent.VK_ENTER) || input.isKeyJustPressed(KeyEvent.VK_SPACE)) {
            if (optionIndex == 0) {
                engine.setScene(new OfflineGameScene(engine, width, height, false, null));
            } else {
                refreshSaveFiles();
                if (saveFiles.isEmpty()) {
                    message = "未找到存档文件";
                } else {
                    selectingSave = true;
                    message = "↑↓ 选择存档，Enter 加载；ESC 返回主页";
                }
            }
        }
    }

    private void handleSaveSelection() {
        if (saveFiles.isEmpty()) {
            selectingSave = false;
            return;
        }

        if (input.isKeyJustPressed(KeyEvent.VK_UP)) {
            saveIndex = (saveIndex - 1 + saveFiles.size()) % saveFiles.size();
        } else if (input.isKeyJustPressed(KeyEvent.VK_DOWN)) {
            saveIndex = (saveIndex + 1) % saveFiles.size();
        } else if (input.isKeyJustPressed(KeyEvent.VK_ENTER) || input.isKeyJustPressed(KeyEvent.VK_SPACE)) {
            Path selected = saveFiles.get(saveIndex);
            engine.setScene(new OfflineGameScene(engine, width, height, true, selected.toString()));
        }
    }

    private void refreshSaveFiles() {
        saveFiles = GameSaveService.listSaveFiles(SAVE_DIRECTORY);
        saveIndex = 0;
    }

    @Override
    public void render() {
        renderer.drawRect(0, 0, width, height, 0.08f, 0.08f, 0.1f, 1.0f);

        String title = "单机模式";
        renderer.drawText(title, width / 2f - title.length() * 14f, 120, 28f, 1f, 1f, 1f, 1f);

        if (selectingSave) {
            renderSaveList();
        } else {
            renderMainOptions();
        }

        if (message != null && !message.isEmpty()) {
            renderer.drawText(message, 40, height - 60, 18f, 0.9f, 0.8f, 0.4f, 1f);
        }

        super.render();
    }

    private void renderMainOptions() {
        float startY = 260f;
        float highlightWidth = 360f;
        float highlightHeight = 56f;
        for (int i = 0; i < mainOptions.length; i++) {
            String option = mainOptions[i];
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

        String hint = "↑↓ 选择，Enter 确认；ESC 返回主页";
        renderer.drawText(hint, 40, height - 90, 18f, 0.8f, 0.8f, 0.8f, 1f);
    }

    private void renderSaveList() {
        renderer.drawText("选择存档:", 60, 200, 20f, 0.9f, 0.9f, 0.9f, 1f);
        float startY = 240f;
        int maxToShow = 8;
        int startIndex = Math.max(0, Math.min(saveIndex - 3, Math.max(0, saveFiles.size() - maxToShow)));
        int endIndex = Math.min(saveFiles.size(), startIndex + maxToShow);
        for (int i = startIndex; i < endIndex; i++) {
            Path file = saveFiles.get(i);
            String name = file.getFileName().toString();
            float y = startY + (i - startIndex) * 36f;
            if (i == saveIndex) {
                renderer.drawRect(50, y - 24, width - 100, 32f, 0.2f, 0.4f, 0.6f, 0.6f);
            }
            renderer.drawText(name, 60, y, 18f, 0.95f, 0.95f, 0.95f, 1f);
        }
    }
}
