package com.gameengine.example;

import com.gameengine.components.RenderComponent;
import com.gameengine.components.TransformComponent;
import com.gameengine.components.PhysicsComponent;
import com.gameengine.core.GameLogic;
import com.gameengine.core.GameObject;
import com.gameengine.core.GameEngine;
import com.gameengine.graphics.Renderer;
import com.gameengine.input.InputManager;
import com.gameengine.math.Vector2;
import com.gameengine.objects.Enemy;
import com.gameengine.objects.Player;
import com.gameengine.save.GameSaveService;
import com.gameengine.scene.Scene;

import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.KeyEvent;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.Collections;
import java.util.List;
import java.util.Optional;
import java.util.Random;

/**
 * 游戏示例
 */
public class GameExample {
    private static final String SAVE_DIRECTORY = "recordings";
    private static final String DEFAULT_SAVE_FILE = "autosave.json";
    private static final int MAX_SAVE_FILES = 10;

    public static void main(String[] args) {
        System.out.println("启动游戏引擎...");

        try {
            // 创建全屏游戏引擎
            Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
            int screenWidth = (int) screenSize.getWidth();
            int screenHeight = (int) screenSize.getHeight();
            System.out.println("屏幕尺寸: " + screenWidth + "x" + screenHeight);

            // 创建游戏引擎（全屏）
            GameEngine engine = new GameEngine(screenWidth, screenHeight, "游戏引擎");

            // 设置开始菜单场景
            Scene startMenu = new StartMenuScene(engine, screenWidth, screenHeight);
            engine.setScene(startMenu);

            // 运行游戏
            engine.run();

        } catch (Exception e) {
            System.err.println("游戏运行出错: " + e.getMessage());
            e.printStackTrace();
        }

        System.out.println("游戏结束");
    }

    private static Scene createMainScene(GameEngine engine, int screenWidth, int screenHeight,
            boolean loadFromSave, String savePath) {
        return new MainGameScene(engine, screenWidth, screenHeight, loadFromSave, savePath);
    }

    private static class MainGameScene extends Scene {
        private final Renderer renderer;
        private final Random random;
        private final int gameWidth;
        private final int gameHeight;
        private GameLogic gameLogic;
        private float enemySpawnTimer;
        private String notification = "";
        private float notificationTimer;
        private float autoSaveTimer;
        private final boolean loadFromSave;
        private final String loadPath;
        private static final float AUTO_SAVE_INTERVAL = 10f;

        MainGameScene(GameEngine engine, int width, int height, boolean loadFromSave, String savePath) {
            super("GameScene");
            this.renderer = engine.getRenderer();
            this.random = new Random();
            this.gameWidth = width;
            this.gameHeight = height;
            this.loadFromSave = loadFromSave;
            String defaultPath = Paths.get(SAVE_DIRECTORY, DEFAULT_SAVE_FILE).toString();
            this.loadPath = loadFromSave ? determineLoadPath(savePath, defaultPath) : null;
        }

        @Override
        public void initialize() {
            super.initialize();
            Player.resetInstance();
            this.enemySpawnTimer = 0f;
            this.notificationTimer = 0f;
            this.notification = "";
            this.autoSaveTimer = 0f;
            this.gameLogic = new GameLogic(this, gameWidth, gameHeight);

            Enemy.setGameDimensions(gameWidth, gameHeight);

            boolean loaded = loadFromSave && loadGame();
            if (!loaded) {
                createPlayer();
                createInitialEnemies();
                pushNotification("新游戏已开始");
            }

            createDecorations();
        }

        @Override
        public void update(float deltaTime) {
            super.update(deltaTime);
            enemySpawnTimer += deltaTime;
            if (notificationTimer > 0f) {
                notificationTimer = Math.max(0f, notificationTimer - deltaTime);
            }

            handleAutoSave(deltaTime);

            gameLogic.handlePlayerInput();
            gameLogic.updatePhysics();
            gameLogic.checkCollisions();

            if (enemySpawnTimer > 2.0f) {
                spawnEnemy();
                enemySpawnTimer = 0f;
            }
        }

        @Override
        public void render() {
            renderer.drawRect(0, 0, gameWidth, gameHeight, 0.1f, 0.1f, 0.2f, 1.0f);

            renderer.drawText("WASD/上下左右 移动", 10, 25, 15f, 1f, 1f, 1f, 1f);
            renderer.drawText("IJKL 射击", 10, 50, 15f, 1f, 1f, 1f, 1f);
            renderer.drawText("每10秒自动存档，退出时也会保存", 10, 80, 15f, 0.9f, 0.9f, 0.4f, 1f);

            Player player = Player.getInstance();
            if (player != null) {
                renderer.drawText("分数：" + player.getScore(), gameWidth - 160, 30, 20, 1.0f, 1.0f, 1.0f, 1.0f);
            }

            super.render();

            if (notificationTimer > 0f && !notification.isEmpty()) {
                renderer.drawText(notification, 10, gameHeight - 30, 18f, 0.9f, 0.7f, 0.2f, 1f);
            }
        }

        @Override
        public void clear() {
            if (gameLogic != null) {
                gameLogic.cleanup();
            }
            super.clear();
        }

        private void handleAutoSave(float deltaTime) {
            autoSaveTimer += deltaTime;
            if (autoSaveTimer >= AUTO_SAVE_INTERVAL) {
                if (saveGame()) {
                    pushNotification("自动存档完成");
                }
                autoSaveTimer = 0f;
            }
        }

        private void pushNotification(String text) {
            notification = text;
            notificationTimer = 3.0f;
        }

        private void createPlayer() {
            float centerX = gameWidth / 2.0f;
            float centerY = gameHeight / 2.0f;
            createPlayer(new Vector2(centerX, centerY));
        }

        private void createPlayer(Vector2 position) {
            Player player = Player.getInstance("Player", this, renderer, position.x, position.y);
            addGameObject(player);
        }

        private void createInitialEnemies() {
            Enemy.setGameDimensions(gameWidth, gameHeight);
            for (int i = 0; i < 3; i++) {
                spawnEnemy();
            }
        }

        private void spawnEnemy() {
            Enemy enemy = new Enemy("Enemy", renderer);
            addGameObject(enemy);
        }

        private void spawnEnemyFromSnapshot(GameSaveService.EnemySnapshot snapshot) {
            Enemy enemy = new Enemy("Enemy", renderer);
            addGameObject(enemy);
            TransformComponent transform = enemy.getComponent(TransformComponent.class);
            if (transform != null && snapshot.position != null) {
                transform.setPosition(new Vector2(snapshot.position));
            }
            PhysicsComponent physics = enemy.getComponent(PhysicsComponent.class);
            if (physics != null && snapshot.velocity != null) {
                physics.setVelocity(snapshot.velocity);
            }
        }

        private void createDecorations() {
            for (int i = 0; i < 5; i++) {
                GameObject decoration = new GameObject("Decoration") {
                    @Override
                    public void update(float deltaTime) {
                        super.update(deltaTime);
                        updateComponents(deltaTime);
                    }

                    @Override
                    public void render() {
                        renderComponents();
                    }
                };

                Vector2 position = new Vector2(
                        random.nextFloat() * gameWidth,
                        random.nextFloat() * gameHeight);

                decoration.addComponent(new TransformComponent(position));
                RenderComponent render = decoration.addComponent(new RenderComponent(
                        RenderComponent.RenderType.CIRCLE,
                        new Vector2(5, 5),
                        new RenderComponent.Color(0.5f, 0.5f, 1.0f, 0.8f)));
                render.setRenderer(renderer);

                addGameObject(decoration);
            }
        }

        private boolean saveGame() {
            try {
                String savePath = generateSavePath();
                GameSaveService saveService = new GameSaveService(savePath);
                GameSaveService.GameSaveData data = new GameSaveService.GameSaveData();
                data.width = gameWidth;
                data.height = gameHeight;
                data.playerScore = 0;
                Player player = Player.getInstance();
                if (player != null) {
                    data.playerScore = player.getScore();
                    TransformComponent transform = player.getComponent(TransformComponent.class);
                    if (transform != null) {
                        data.playerPosition = new Vector2(transform.getPosition());
                    }
                }
                for (GameObject obj : getGameObjects()) {
                    if (obj instanceof Enemy) {
                        GameSaveService.EnemySnapshot snapshot = new GameSaveService.EnemySnapshot();
                        snapshot.state = obj.tag != null ? obj.tag : "Enemy";
                        snapshot.active = obj.isActive();
                        TransformComponent transform = obj.getComponent(TransformComponent.class);
                        if (transform != null) {
                            snapshot.position = new Vector2(transform.getPosition());
                        }
                        PhysicsComponent physics = obj.getComponent(PhysicsComponent.class);
                        if (physics != null) {
                            snapshot.velocity = physics.getVelocity();
                        }
                        data.enemies.add(snapshot);
                    }
                }
                saveService.save(data);
                pruneOldSaves();
                return true;
            } catch (IOException e) {
                pushNotification("存档失败: " + e.getMessage());
                return false;
            }
        }

        private boolean loadGame() {
            try {
                if (loadPath == null || loadPath.isEmpty()) {
                    return false;
                }
                Optional<GameSaveService.GameSaveData> optional = new GameSaveService(loadPath).load();
                if (!optional.isPresent()) {
                    return false;
                }
                GameSaveService.GameSaveData data = optional.get();
                Vector2 playerPos = data.playerPosition != null
                        ? data.playerPosition
                        : new Vector2(gameWidth / 2.0f, gameHeight / 2.0f);
                createPlayer(playerPos);
                Player player = Player.getInstance();
                if (player != null) {
                    player.setScore(data.playerScore);
                }
                for (GameSaveService.EnemySnapshot snapshot : data.enemies) {
                    if (snapshot != null && snapshot.active) {
                        spawnEnemyFromSnapshot(snapshot);
                    }
                }
                enemySpawnTimer = 0f;
                autoSaveTimer = 0f;
                String fileName = Paths.get(loadPath).getFileName().toString();
                pushNotification("已加载存档: " + fileName);
                return true;
            } catch (IOException e) {
                pushNotification("加载存档失败: " + e.getMessage());
                return false;
            }
        }

        @Override
        public void onExit() {
            saveGame();
        }

        private String determineLoadPath(String desiredPath, String fallback) {
            if (desiredPath == null || desiredPath.isEmpty()) {
                return fallback;
            }
            String lower = desiredPath.toLowerCase();
            if (!lower.endsWith(".json")) {
                return fallback;
            }
            return desiredPath;
        }

        private String generateSavePath() throws IOException {
            DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyyMMdd_HHmmss_SSS");
            String timestamp = LocalDateTime.now().format(formatter);
            String baseName = "save_" + timestamp;
            Path directory = Paths.get(SAVE_DIRECTORY);
            Files.createDirectories(directory);
            Path candidate = directory.resolve(baseName + ".json");
            int counter = 1;
            while (Files.exists(candidate)) {
                candidate = directory.resolve(baseName + "_" + counter + ".json");
                counter++;
            }
            return candidate.toString();
        }

        private void pruneOldSaves() {
            List<Path> files = GameSaveService.listSaveFiles(SAVE_DIRECTORY);
            if (files.size() <= MAX_SAVE_FILES) {
                return;
            }
            for (int i = MAX_SAVE_FILES; i < files.size(); i++) {
                Path path = files.get(i);
                try {
                    Files.deleteIfExists(path);
                } catch (IOException e) {
                    System.err.println("删除旧存档失败: " + path + " - " + e.getMessage());
                }
            }
        }
    }

    private static class StartMenuScene extends Scene {
        private final GameEngine engine;
        private final Renderer renderer;
        private final int screenWidth;
        private final int screenHeight;
        private final String[] mainOptions = { "开始游戏", "继续游戏" };
        private int optionIndex = 0;
        private boolean selectingSave = false;
        private List<Path> saveFiles = Collections.emptyList();
        private int saveIndex = 0;
        private String message = "";
        private float messageTimer = 0f;
        private InputManager input;

        StartMenuScene(GameEngine engine, int width, int height) {
            super("StartMenu");
            this.engine = engine;
            this.renderer = engine.getRenderer();
            this.screenWidth = width;
            this.screenHeight = height;
        }

        @Override
        public void initialize() {
            super.initialize();
            this.input = engine.getInputManager();
            setMessage("按上下选择，回车确认", Float.POSITIVE_INFINITY);
        }

        @Override
        public void update(float deltaTime) {
            super.update(deltaTime);
            if (messageTimer != Float.POSITIVE_INFINITY && messageTimer > 0f) {
                messageTimer = Math.max(0f, messageTimer - deltaTime);
            }
            if (messageTimer != Float.POSITIVE_INFINITY && messageTimer == 0f) {
                message = "";
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
                    engine.setScene(createMainScene(engine, screenWidth, screenHeight, false, null));
                    return;
                } else {
                    refreshSaveFiles();
                    if (saveFiles.isEmpty()) {
                        setMessage("未找到存档文件", 2.5f);
                    } else {
                        selectingSave = true;
                        setMessage("选择存档并按回车加载，ESC 返回", Float.POSITIVE_INFINITY);
                    }
                }
            }
        }

        private void handleSaveSelection() {
            if (input.isKeyJustPressed(KeyEvent.VK_ESCAPE) || input.isKeyJustPressed(KeyEvent.VK_BACK_SPACE)) {
                selectingSave = false;
                setMessage("按上下选择，回车确认", Float.POSITIVE_INFINITY);
                return;
            }
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
                engine.setScene(createMainScene(engine, screenWidth, screenHeight, true, selected.toString()));
                return;
            }
        }

        private void refreshSaveFiles() {
            saveFiles = GameSaveService.listSaveFiles(SAVE_DIRECTORY);
            saveIndex = 0;
        }

        private void setMessage(String text, float duration) {
            this.message = text;
            this.messageTimer = Math.max(0f, duration);
        }

        @Override
        public void render() {
            renderer.drawRect(0, 0, screenWidth, screenHeight, 0.08f, 0.08f, 0.1f, 1.0f);
            String title = "简易射击游戏";
            renderer.drawText(title, screenWidth / 2f - title.length() * 12f, 120, 28f, 1f, 1f, 1f, 1f);

            if (selectingSave) {
                renderSaveList();
            } else {
                renderMainOptions();
            }

            if (!message.isEmpty()) {
                renderer.drawText(message, 40, screenHeight - 60, 18f, 0.9f, 0.8f, 0.4f, 1f);
            }
        }

        private void renderMainOptions() {
            float startY = 220f;
            float highlightWidth = 320f;
            float highlightHeight = 56f;
            for (int i = 0; i < mainOptions.length; i++) {
                String option = mainOptions[i];
                float textWidth = option.length() * 24f;
                float x = screenWidth / 2f - textWidth / 2f;
                float y = startY + i * 50f;
                if (i == optionIndex) {
                    float rectX = screenWidth / 2f - highlightWidth / 2f;
                    renderer.drawRect(rectX, y - highlightHeight / 2f, highlightWidth, highlightHeight, 0.2f, 0.3f,
                            0.6f, 0.6f);
                }
                renderer.drawText(option, x, y, 24f, 1f, 1f, 1f, 1f);
            }
        }

        private void renderSaveList() {
            renderer.drawText("选择存档:", 60, 200, 20f, 0.9f, 0.9f, 0.9f, 1f);
            float startY = 240f;
            int maxToShow = 8;
            if (saveFiles.isEmpty()) {
                renderer.drawText("此目录下没有可用存档", 60, startY, 18f, 0.8f, 0.8f, 0.8f, 1f);
                return;
            }
            int startIndex = Math.max(0, Math.min(saveIndex - 3, Math.max(0, saveFiles.size() - maxToShow)));
            int endIndex = Math.min(saveFiles.size(), startIndex + maxToShow);
            for (int i = startIndex; i < endIndex; i++) {
                Path file = saveFiles.get(i);
                String name = file.getFileName().toString();
                float y = startY + (i - startIndex) * 36f;
                if (i == saveIndex) {
                    renderer.drawRect(50, y - 24, screenWidth - 100, 32f, 0.2f, 0.4f, 0.6f, 0.6f);
                }
                renderer.drawText(name, 60, y, 18f, 0.95f, 0.95f, 0.95f, 1f);
            }
            if (startIndex > 0) {
                renderer.drawText("↑", screenWidth - 80, startY - 24, 18f, 0.8f, 0.8f, 0.8f, 1f);
            }
            if (endIndex < saveFiles.size()) {
                renderer.drawText("↓", screenWidth - 80, startY + (endIndex - startIndex) * 36f, 18f, 0.8f, 0.8f, 0.8f,
                        1f);
            }
        }
    }
}
