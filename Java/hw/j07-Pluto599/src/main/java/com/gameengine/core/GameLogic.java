package com.gameengine.core;

import com.gameengine.components.TransformComponent;
import com.gameengine.components.PhysicsComponent;
import com.gameengine.input.InputManager;
import com.gameengine.math.Vector2;
import com.gameengine.objects.Player;
import com.gameengine.scene.Scene;

import java.util.List;
import java.util.Map;
import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.ConcurrentHashMap;

/**
 * 游戏逻辑类，处理具体的游戏规则
 */
public class GameLogic {
    private Scene scene;
    private InputManager inputManager;
    // ========== 并行处理优化 ==========
    // 使用线程池来并行处理物理计算和碰撞检测
    private ExecutorService physicsExecutor;

    // 游戏屏幕尺寸
    private int gameWidth;
    private int gameHeight;

    public GameLogic(Scene scene, int gameWidth, int gameHeight) {
        this.scene = scene;
        this.inputManager = InputManager.getInstance();
        this.gameWidth = gameWidth;
        this.gameHeight = gameHeight;

        // ========== 并行处理优化 ==========
        // 创建线程池,线程数为CPU核心数-1
        int threadCount = Math.max(2, Runtime.getRuntime().availableProcessors() - 1);
        this.physicsExecutor = Executors.newFixedThreadPool(threadCount);
        // System.out.println("GameLogic: 创建线程池,线程数=" + threadCount);
        // System.out.println("GameLogic: 游戏尺寸=" + gameWidth + "x" + gameHeight);
    }

    /**
     * 清理资源
     * ========== 并行处理优化 ==========
     * 关闭线程池,释放资源
     */
    public void cleanup() {
        if (physicsExecutor != null && !physicsExecutor.isShutdown()) {
            physicsExecutor.shutdown();
            try {
                if (!physicsExecutor.awaitTermination(1, TimeUnit.SECONDS)) {
                    physicsExecutor.shutdownNow();
                }
            } catch (InterruptedException e) {
                physicsExecutor.shutdownNow();
                Thread.currentThread().interrupt();
            }
        }
    }

    /**
     * 处理玩家输入
     */
    public void handlePlayerInput() {
        Player player = Player.getInstance();
        TransformComponent transform = player.getComponent(TransformComponent.class);
        PhysicsComponent physics = player.getComponent(PhysicsComponent.class);

        if (transform == null || physics == null)
            return;

        Vector2 movement = new Vector2();

        if (inputManager.isKeyPressed(87) || inputManager.isKeyPressed(38)) { // W或上箭头
            movement.y -= 1;
        }
        if (inputManager.isKeyPressed(83) || inputManager.isKeyPressed(40)) { // S或下箭头
            movement.y += 1;
        }
        if (inputManager.isKeyPressed(65) || inputManager.isKeyPressed(37)) { // A或左箭头
            movement.x -= 1;
        }
        if (inputManager.isKeyPressed(68) || inputManager.isKeyPressed(39)) { // D或右箭头
            movement.x += 1;
        }

        if (movement.magnitude() > 0) {
            movement = movement.normalize().multiply(200);
            physics.setVelocity(movement);
        }

        // 边界检查（动态适配屏幕尺寸）
        Vector2 pos = transform.getPosition();
        if (pos.x < 0)
            pos.x = 0;
        if (pos.y < 0)
            pos.y = 0;
        if (pos.x > gameWidth - 20)
            pos.x = gameWidth - 20;
        if (pos.y > gameHeight - 20)
            pos.y = gameHeight - 20;
        transform.setPosition(pos);
    }

    /**
     * 更新物理系统
     * ========== 并行处理优化 ==========
     * 使用线程池并行更新所有物理组件,提高性能
     */
    public void updatePhysics() {
        List<PhysicsComponent> physicsComponents = scene.getComponents(PhysicsComponent.class);
        if (physicsComponents.isEmpty())
            return;

        // ========== 并行处理优化 ==========
        // 计算批次大小:将物理组件分成多个批次,每个线程处理一个批次
        int threadCount = Runtime.getRuntime().availableProcessors() - 1;
        threadCount = Math.max(2, threadCount);
        int batchSize = Math.max(1, physicsComponents.size() / threadCount + 1);

        List<Future<?>> futures = new ArrayList<>();

        // 将物理组件分批,提交给线程池并行处理
        for (int i = 0; i < physicsComponents.size(); i += batchSize) {
            final int start = i;
            final int end = Math.min(i + batchSize, physicsComponents.size());

            // 提交任务到线程池
            Future<?> future = physicsExecutor.submit(() -> {
                // 每个线程处理自己批次内的物理组件
                for (int j = start; j < end; j++) {
                    PhysicsComponent physics = physicsComponents.get(j);
                    updateSinglePhysics(physics);
                }
            });

            futures.add(future);
        }

        // 等待所有线程完成
        for (Future<?> future : futures) {
            try {
                future.get();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

    }

    /**
     * 更新单个物理组件
     * ========== 并行处理优化 ==========
     * 这个方法会被多个线程调用,处理各自的物理组件
     */
    private void updateSinglePhysics(PhysicsComponent physics) {
        // 边界销毁（动态适配屏幕尺寸）
        if (!physics.getOwner().tag.equals("Player")) {
            TransformComponent transform = physics.getOwner().getComponent(TransformComponent.class);
            if (transform != null) {
                Vector2 pos = transform.getPosition();
                if (pos.x <= 0 || pos.x >= gameWidth || pos.y <= 0 || pos.y >= gameHeight) {
                    physics.getOwner().destroy();
                }
            }
        }
    }

    /**
     * 检查碰撞
     * ========== 并行处理优化 ==========
     * 使用线程池并行检测碰撞,提高性能
     */
    public void checkCollisions() {
        List<GameObject> objs = scene.getGameObjects();
        if (objs.isEmpty())
            return;

        // 使用ConcurrentHashMap保证线程安全
        Map<GameObject, List<GameObject>> sendList = new ConcurrentHashMap<>();

        // 只处理有物理组件的对象
        List<GameObject> physicsObjs = new ArrayList<>();
        for (GameObject obj : objs) {
            if (obj.hasComponent(PhysicsComponent.class)) {
                physicsObjs.add(obj);
            }
        }

        if (physicsObjs.isEmpty())
            return;

        // ========== 并行处理优化 ==========
        // 计算批次大小
        int threadCount = Runtime.getRuntime().availableProcessors() - 1;
        threadCount = Math.max(2, threadCount);
        int batchSize = Math.max(1, physicsObjs.size() / threadCount + 1);

        List<Future<?>> futures = new ArrayList<>();

        // 将碰撞检测分批,提交给线程池并行处理
        for (int i = 0; i < physicsObjs.size(); i += batchSize) {
            final int start = i;
            final int end = Math.min(i + batchSize, physicsObjs.size());

            // 提交任务到线程池
            Future<?> future = physicsExecutor.submit(() -> {
                // 每个线程处理自己批次内的对象
                for (int j = start; j < end; j++) {
                    GameObject a = physicsObjs.get(j);
                    List<GameObject> colliders = new ArrayList<>();

                    // 检测与其他所有对象的碰撞
                    for (GameObject b : physicsObjs) {
                        if (a != b) {
                            float distance = a.getComponent(TransformComponent.class).getPosition()
                                    .distance(b.getComponent(TransformComponent.class).getPosition());
                            if (distance < 25) {
                                colliders.add(b);
                            }
                        }
                    }

                    if (!colliders.isEmpty()) {
                        sendList.put(a, colliders);
                    }
                }
            });

            futures.add(future);
        }

        // 等待所有线程完成
        for (Future<?> future : futures) {
            try {
                future.get();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        // 触发碰撞事件
        for (Map.Entry<GameObject, List<GameObject>> entry : sendList.entrySet()) {
            entry.getKey().onCollisionEnter(entry.getValue());
        }

    }
}
