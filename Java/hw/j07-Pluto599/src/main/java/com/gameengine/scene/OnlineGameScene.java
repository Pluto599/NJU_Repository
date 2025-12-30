package com.gameengine.scene;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Random;
import java.util.Set;

import com.gameengine.components.RenderComponent;
import com.gameengine.components.TransformComponent;
import com.gameengine.core.GameEngine;
import com.gameengine.core.GameLogic;
import com.gameengine.core.GameObject;
import com.gameengine.graphics.Renderer;
import com.gameengine.input.InputManager;
import com.gameengine.math.Vector2;
import com.gameengine.net.NetState;
import com.gameengine.net.NetworkBuffer;
import com.gameengine.objects.Bullet;
import com.gameengine.objects.Enemy;
import com.gameengine.objects.NetworkPlayer;
import com.gameengine.objects.Player;

/**
 * 联机场景：Server 负责权威模拟，Client 负责插值渲染。
 */
public class OnlineGameScene extends Scene {
    public enum Mode { SERVER, CLIENT }

    private final GameEngine engine;
    private final Mode mode;
    private final Renderer renderer;
    private final InputManager input;
    private final Random random = new Random();
    private final Map<GameObject, String> entityIds = new HashMap<>();
    private final Map<String, GameObject> mirrorObjects = new HashMap<>();

    private GameLogic gameLogic;
    private float enemySpawnTimer = 0f;
    private NetworkPlayer networkPlayer;
    private int nextEntityId = 0;
    private float networkShootTimer = 0f;
    private static final float NETWORK_SHOOT_COOLDOWN = 0.25f;

    public OnlineGameScene(GameEngine engine, Mode mode) {
        super("OnlineGameScene");
        this.engine = engine;
        this.mode = mode;
        this.renderer = engine.getRenderer();
        this.input = engine.getInputManager();
    }

    @Override
    public void initialize() {
        super.initialize();
        entityIds.clear();
        mirrorObjects.clear();
        nextEntityId = 0;
        enemySpawnTimer = 0f;
        networkPlayer = null;
        networkShootTimer = 0f;
        Player.resetInstance();

        if (mode == Mode.SERVER) {
            int width = renderer.getWidth();
            int height = renderer.getHeight();
            gameLogic = new GameLogic(this, width, height);
            Enemy.setGameDimensions(width, height);
            createPlayer(width, height);
            createInitialEnemies();
            createDecorations();
        } else {
            gameLogic = null;
        }
    }

    @Override
    public void update(float deltaTime) {
        if (input.isKeyJustPressed(java.awt.event.KeyEvent.VK_BACK_SPACE)) {
            engine.setScene(new com.gameengine.example.LauncherScene(engine, renderer.getWidth(), renderer.getHeight()));
            return;
        }

        if (mode == Mode.CLIENT) {
            applyNetworkSnapshot();
        }

        super.update(deltaTime);

        if (mode == Mode.SERVER) {
            updateServerLogic(deltaTime);
            cleanupEntityIds();
            syncNetworkState();
        } else {
            removeDestroyedMirrors();
        }
    }

    @Override
    public void render() {
        renderer.drawRect(0, 0, renderer.getWidth(), renderer.getHeight(), 0.1f, 0.1f, 0.2f, 1.0f);

        renderer.drawText("WASD/上下左右 移动", 10, 25, 15f, 1f, 1f, 1f, 1f);
        renderer.drawText("IJKL 射击", 10, 50, 15f, 1f, 1f, 1f, 1f);
        renderer.drawText("Backspace 返回主页", 10, 75, 15f, 1f, 1f, 1f, 1f);

        if (mode == Mode.SERVER) {
            renderer.drawText("SERVER MODE · 端口 7777", 10, 100, 15f, 0.6f, 1.0f, 0.6f, 1f);
            renderer.drawText("已连接客户端: " + NetState.getClientCount(), 10, 125, 15f, 0.6f, 1.0f, 0.6f, 1f);
        } else {
            renderer.drawText("CLIENT MODE", 10, 100, 15f, 0.6f, 0.8f, 1.0f, 1f);
            renderer.drawText("等待并插值远端实体", 10, 125, 15f, 0.6f, 0.8f, 1.0f, 1f);
        }

        Player player = Player.getInstance();
        if (player != null) {
            renderer.drawText("分数: " + player.getScore(), renderer.getWidth() - 180, 30, 20f, 1f, 1f, 1f, 1f);
        }

        super.render();
    }

    @Override
    public void clear() {
        mirrorObjects.clear();
        entityIds.clear();
        if (gameLogic != null) {
            gameLogic.cleanup();
            gameLogic = null;
        }
        super.clear();
    }

    @Override
    public void onExit() {
        if (mode == Mode.CLIENT) {
            NetState.closeActiveClient();
        }
        if (gameLogic != null) {
            gameLogic.cleanup();
            gameLogic = null;
        }
        Player.resetInstance();
        super.onExit();
    }

    private void updateServerLogic(float deltaTime) {
        enemySpawnTimer += deltaTime;
        if (enemySpawnTimer >= 2.0f) {
            spawnEnemy();
            enemySpawnTimer = 0f;
        }

        if (gameLogic != null) {
            gameLogic.handlePlayerInput();
            gameLogic.updatePhysics();
            gameLogic.checkCollisions();
        }

        if (NetState.hasClient() && networkPlayer == null) {
            createNetworkPlayer();
        }

        handleNetworkShooting(deltaTime);
    }

    private void createPlayer(int width, int height) {
        float startX = width / 2.0f;
        float startY = height / 2.0f;
        Player player = Player.getInstance("Player", this, renderer, startX, startY);
        addGameObject(player);
    }

    private void createInitialEnemies() {
        for (int i = 0; i < 3; i++) {
            spawnEnemy();
        }
    }

    private void spawnEnemy() {
        Enemy enemy = new Enemy("Enemy", renderer);
        addGameObject(enemy);
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
                    random.nextFloat() * renderer.getWidth(),
                    random.nextFloat() * renderer.getHeight());

            decoration.addComponent(new TransformComponent(position));
            RenderComponent render = decoration.addComponent(new RenderComponent(
                    RenderComponent.RenderType.CIRCLE,
                    new Vector2(5, 5),
                    new RenderComponent.Color(0.5f, 0.5f, 1.0f, 0.8f)));
            render.setRenderer(renderer);
            addGameObject(decoration);
        }
    }

    private void createNetworkPlayer() {
        networkPlayer = new NetworkPlayer("Player2", renderer,
                new Vector2(renderer.getWidth() / 2.0f + 40, renderer.getHeight() / 2.0f));
        addGameObject(networkPlayer);
    }

    private void handleNetworkShooting(float deltaTime) {
        if (networkPlayer == null) {
            return;
        }
        networkShootTimer = Math.max(0f, networkShootTimer - deltaTime);
        if (networkShootTimer > 0f) {
            return;
        }
        float[] dir = new float[2];
        if (NetState.consumeP2Shoot(dir)) {
            Vector2 direction = new Vector2(dir[0], dir[1]);
            if (direction.magnitude() > 0f) {
                spawnNetworkBullet(direction.normalize());
                networkShootTimer = NETWORK_SHOOT_COOLDOWN;
            }
        }
    }

    private void spawnNetworkBullet(Vector2 direction) {
        TransformComponent transform = networkPlayer.getComponent(TransformComponent.class);
        if (transform == null) {
            return;
        }
        Vector2 origin = transform.getPosition();
        Bullet bullet = new Bullet("Player2Bullet-" + System.nanoTime(), renderer, origin, direction);
        addGameObject(bullet);
    }

    private void syncNetworkState() {
        long now = System.currentTimeMillis();
        StringBuilder builder = new StringBuilder();
        builder.append('{')
                .append("\"type\":\"kf\",")
                .append("\"t\":").append(now / 1000.0).append(',')
                .append("\"entities\":[");

        boolean first = true;
        for (GameObject obj : getGameObjects()) {
            if (obj == null || !obj.isActive()) {
                continue;
            }
            TransformComponent transform = obj.getComponent(TransformComponent.class);
            if (transform == null) {
                continue;
            }
            String kind = resolveKind(obj);
            if (kind == null) {
                continue;
            }
            String id = entityIds.computeIfAbsent(obj, ignored -> kind + ":" + nextEntityId++);
            Vector2 pos = transform.getPosition();
            if (!first) {
                builder.append(',');
            }
            builder.append('{')
                    .append("\"id\":\"").append(id).append("\",")
                    .append("\"x\":").append(Math.round(pos.x)).append(',')
                    .append("\"y\":").append(Math.round(pos.y))
                    .append('}');
            first = false;
        }
        builder.append(']')
                .append('}');
        NetState.setLastKeyframeJson(builder.toString());
    }

    private void cleanupEntityIds() {
        entityIds.entrySet().removeIf(entry -> entry.getKey() == null || !entry.getKey().isActive());
    }

    private String resolveKind(GameObject obj) {
        if (obj instanceof Player) {
            return "player";
        }
        if (obj instanceof NetworkPlayer) {
            return "player2";
        }
        if (obj instanceof Enemy) {
            return "enemy";
        }
        if (obj instanceof Bullet) {
            return "bullet";
        }
        return null;
    }

    private void applyNetworkSnapshot() {
        Map<String, float[]> snapshot = NetworkBuffer.sample();
        Set<String> seen = new HashSet<>(snapshot.keySet());

        for (Map.Entry<String, float[]> entry : snapshot.entrySet()) {
            String id = entry.getKey();
            float[] coords = entry.getValue();
            GameObject proxy = mirrorObjects.get(id);
            if (proxy == null) {
                proxy = createMirrorObject(id);
                mirrorObjects.put(id, proxy);
                addGameObject(proxy);
            }
            TransformComponent transform = proxy.getComponent(TransformComponent.class);
            if (transform == null) {
                transform = proxy.addComponent(new TransformComponent(new Vector2()));
            }
            transform.setPosition(new Vector2(coords[0], coords[1]));
        }

        for (Map.Entry<String, GameObject> entry : new HashSet<>(mirrorObjects.entrySet())) {
            if (!seen.contains(entry.getKey())) {
                entry.getValue().destroy();
                mirrorObjects.remove(entry.getKey());
            }
        }
    }

    private void removeDestroyedMirrors() {
        mirrorObjects.entrySet().removeIf(entry -> entry.getValue() == null || !entry.getValue().isActive());
    }

    private GameObject createMirrorObject(String id) {
        final String type = parseTypePrefix(id);
        GameObject proxy;
        if ("player".equals(type) || "player2".equals(type)) {
            proxy = createPlayerVisual(id, "player2".equals(type));
        } else {
            proxy = new GameObject(id) {
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
            proxy.tag = type;
            proxy.addComponent(new TransformComponent(new Vector2()));

            RenderComponent.Color color;
            Vector2 size;
            switch (type) {
                case "enemy":
                    color = new RenderComponent.Color(1.0f, 0.5f, 0.0f, 1.0f);
                    size = new Vector2(20, 20);
                    break;
                case "bullet":
                    color = new RenderComponent.Color(0.0f, 0.5f, 1.0f, 1.0f);
                    size = new Vector2(5, 5);
                    break;
                default:
                    color = new RenderComponent.Color(0.9f, 0.9f, 0.2f, 1.0f);
                    size = new Vector2(10, 10);
                    break;
            }

            RenderComponent render = proxy.addComponent(new RenderComponent(
                    RenderComponent.RenderType.RECTANGLE,
                    size,
                    color));
            render.setRenderer(renderer);
        }
        return proxy;
    }

    private GameObject createPlayerVisual(String id, boolean isClientLocalPlayer) {
        GameObject proxy = new GameObject(id) {
            @Override
            public void update(float deltaTime) {
                super.update(deltaTime);
                updateComponents(deltaTime);
            }

            @Override
            public void render() {
                TransformComponent transform = getComponent(TransformComponent.class);
                if (transform == null) {
                    return;
                }
                Vector2 pos = transform.getPosition();
                float bodyR = isClientLocalPlayer ? 0.95f : 1.0f;
                float bodyG = isClientLocalPlayer ? 0.35f : 0.4f;
                float bodyB = isClientLocalPlayer ? 0.35f : 0.2f;
                renderer.drawRect(pos.x - 8, pos.y - 10, 16, 20, bodyR, bodyG, bodyB, 1.0f);
                renderer.drawRect(pos.x - 6, pos.y - 22, 12, 12, 1.0f, 0.8f, 0.2f, 1.0f);
                renderer.drawRect(pos.x - 13, pos.y - 5, 6, 12, 0.95f, 0.8f, 0.1f, 1.0f);
                renderer.drawRect(pos.x + 7, pos.y - 5, 6, 12, 0.2f, 0.9f, 0.4f, 1.0f);
            }
        };
        proxy.tag = isClientLocalPlayer ? "player2" : "player";
        proxy.addComponent(new TransformComponent(new Vector2()));
        return proxy;
    }

    private String parseTypePrefix(String id) {
        if (id == null) {
            return "unknown";
        }
        int idx = id.indexOf(':');
        if (idx < 0) {
            return id;
        }
        return id.substring(0, idx);
    }
}
