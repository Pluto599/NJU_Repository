package com.gameengine.scene;

import com.gameengine.components.PhysicsComponent;
import com.gameengine.components.RenderComponent;
import com.gameengine.components.TransformComponent;
import com.gameengine.core.GameEngine;
import com.gameengine.core.GameLogic;
import com.gameengine.core.GameObject;
import com.gameengine.graphics.Renderer;
import com.gameengine.input.InputManager;
import com.gameengine.math.Vector2;
import com.gameengine.objects.Enemy;
import com.gameengine.objects.Player;
import com.gameengine.save.GameSaveService;

import java.awt.event.KeyEvent;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.List;
import java.util.Optional;
import java.util.Random;

/**
 * 单机游戏场景：复用 j05 的存档/读档逻辑。
 */
public class OfflineGameScene extends Scene {
    private static final String SAVE_DIRECTORY = "recordings";
    private static final String DEFAULT_SAVE_FILE = "autosave.json";
    private static final int MAX_SAVE_FILES = 10;

    private final GameEngine engine;
    private final Renderer renderer;
    private final InputManager input;
    private final Random random = new Random();

    private final int gameWidth;
    private final int gameHeight;
    private final boolean loadFromSave;
    private final String loadPath;

    private GameLogic gameLogic;
    private float enemySpawnTimer;

    private String notification = "";
    private float notificationTimer;

    private float autoSaveTimer;
    private static final float AUTO_SAVE_INTERVAL = 10f;

    private boolean dead;
    private int deadOptionIndex;
    private final String[] deadOptions = { "重新开始", "回到主页" };

    public OfflineGameScene(GameEngine engine, int width, int height, boolean loadFromSave, String savePath) {
        super("OfflineGameScene");
        this.engine = engine;
        this.renderer = engine.getRenderer();
        this.input = engine.getInputManager();
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
        this.dead = false;
        this.deadOptionIndex = 0;

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
        if (input.isKeyJustPressed(KeyEvent.VK_BACK_SPACE)) {
            engine.setScene(new com.gameengine.example.LauncherScene(engine, gameWidth, gameHeight));
            return;
        }

        if (dead) {
            handleDeathMenu();
            return;
        }

        super.update(deltaTime);

        enemySpawnTimer += deltaTime;
        if (notificationTimer > 0f) {
            notificationTimer = Math.max(0f, notificationTimer - deltaTime);
        }

        handleAutoSave(deltaTime);

        if (gameLogic != null) {
            gameLogic.handlePlayerInput();
            gameLogic.updatePhysics();
            gameLogic.checkCollisions();
        }

        if (enemySpawnTimer > 2.0f) {
            spawnEnemy();
            enemySpawnTimer = 0f;
        }

        // 注意：这里不处理 ESC（游戏进行中按 ESC 不应该有反应）
    }

    @Override
    public void render() {
        renderer.drawRect(0, 0, gameWidth, gameHeight, 0.1f, 0.1f, 0.2f, 1.0f);

        renderer.drawText("WASD/上下左右 移动", 10, 25, 15f, 1f, 1f, 1f, 1f);
        renderer.drawText("IJKL 射击", 10, 50, 15f, 1f, 1f, 1f, 1f);
        renderer.drawText("Backspace 返回主页", 10, 75, 15f, 1f, 1f, 1f, 1f);
        renderer.drawText("每10秒自动存档，退出时也会保存", 10, 100, 15f, 0.9f, 0.9f, 0.4f, 1f);

        Player player = Player.getInstance();
        if (player != null) {
            renderer.drawText("分数：" + player.getScore(), gameWidth - 180, 30, 20, 1f, 1f, 1f, 1f);
        }

        super.render();

        if (notificationTimer > 0f && notification != null && !notification.isEmpty()) {
            renderer.drawText(notification, 10, gameHeight - 30, 18f, 0.9f, 0.7f, 0.2f, 1f);
        }

        if (dead) {
            renderDeathOverlay();
        }
    }

    @Override
    public void clear() {
        if (gameLogic != null) {
            gameLogic.cleanup();
            gameLogic = null;
        }
        super.clear();
    }

    @Override
    public void onExit() {
        saveGame();
    }

    @Override
    public boolean onPlayerDeath() {
        if (dead) {
            return true;
        }
        dead = true;
        deadOptionIndex = 0;
        return true;
    }

    private void handleDeathMenu() {
        if (input.isKeyJustPressed(KeyEvent.VK_UP)) {
            deadOptionIndex = (deadOptionIndex - 1 + deadOptions.length) % deadOptions.length;
        } else if (input.isKeyJustPressed(KeyEvent.VK_DOWN)) {
            deadOptionIndex = (deadOptionIndex + 1) % deadOptions.length;
        }

        if (input.isKeyJustPressed(KeyEvent.VK_ENTER) || input.isKeyJustPressed(KeyEvent.VK_SPACE)) {
            if (deadOptionIndex == 0) {
                // 重新开始
                engine.setScene(new OfflineGameScene(engine, gameWidth, gameHeight, false, null));
            } else {
                // 回到主页
                engine.setScene(new com.gameengine.example.LauncherScene(engine, gameWidth, gameHeight));
            }
        }
    }

    private void renderDeathOverlay() {
        renderer.drawRect(0, 0, gameWidth, gameHeight, 0f, 0f, 0f, 0.65f);

        String text = "你已死亡";
        float y = gameHeight / 2f - 60;
        renderer.drawTextCentered(text, gameWidth / 2f, y, 32f, 1f, 0.9f, 0.9f, 1f);

        float optionY = y + 60;
        for (int i = 0; i < deadOptions.length; i++) {
            String opt = deadOptions[i];
            float w = opt.length() * 22f;
            float ox = gameWidth / 2f - w / 2f;
            float oy = optionY + i * 50f;
            if (i == deadOptionIndex) {
                renderer.drawRect(gameWidth / 2f - 220, oy - 24, 440, 40, 0.2f, 0.4f, 0.6f, 0.75f);
            }
            renderer.drawText(opt, ox, oy, 24f, 1f, 1f, 1f, 1f);
        }
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

            Vector2 position = new Vector2(random.nextFloat() * gameWidth, random.nextFloat() * gameHeight);
            decoration.addComponent(new TransformComponent(position));
            RenderComponent render = decoration.addComponent(new RenderComponent(RenderComponent.RenderType.CIRCLE,
                    new Vector2(5, 5), new RenderComponent.Color(0.5f, 0.5f, 1.0f, 0.8f)));
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
            Vector2 playerPos = data.playerPosition != null ? data.playerPosition
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
