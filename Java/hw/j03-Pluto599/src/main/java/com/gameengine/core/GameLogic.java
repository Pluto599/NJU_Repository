package com.gameengine.core;

import com.gameengine.components.TransformComponent;
import com.gameengine.components.PhysicsComponent;
import com.gameengine.core.GameObject;
import com.gameengine.input.InputManager;
import com.gameengine.math.Vector2;
import com.gameengine.objects.Player;
import com.gameengine.scene.Scene;

import java.util.List;
import java.util.Map;
import java.util.ArrayList;
import java.util.Dictionary;
import java.util.Enumeration;
import java.util.HashMap;

/**
 * 游戏逻辑类，处理具体的游戏规则
 */
public class GameLogic {
    private Scene scene;
    private InputManager inputManager;

    // ========== 性能统计 ==========
    // 用于统计性能数据
    private long totalPhysicsTime = 0;
    private long totalCollisionTime = 0;
    private int frameCount = 0;
    private long lastPrintTime = System.currentTimeMillis();

    public GameLogic(Scene scene) {
        this.scene = scene;
        this.inputManager = InputManager.getInstance();
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

        // 边界检查
        Vector2 pos = transform.getPosition();
        if (pos.x < 0)
            pos.x = 0;
        if (pos.y < 0)
            pos.y = 0;
        if (pos.x > 800 - 20)
            pos.x = 800 - 20;
        if (pos.y > 600 - 20)
            pos.y = 600 - 20;
        transform.setPosition(pos);
    }

    /**
     * 更新物理系统
     */
    public void updatePhysics() {
        // ========== 性能统计 ==========
        long startTime = System.nanoTime();

        List<PhysicsComponent> physicsComponents = scene.getComponents(PhysicsComponent.class);
        for (PhysicsComponent physics : physicsComponents) {
            // 边界销毁
            if (!physics.getOwner().tag.equals("Player")) {
                TransformComponent transform = physics.getOwner().getComponent(TransformComponent.class);
                if (transform != null) {
                    Vector2 pos = transform.getPosition();
                    if (pos.x <= 0 || pos.x >= 800 || pos.y <= 0 || pos.y >= 600) {
                        // System.out.println(physics.getOwner().tag + " is destroyed");
                        physics.getOwner().destroy();
                    }
                }
            }
            // TransformComponent transform =
            // physics.getOwner().getComponent(TransformComponent.class);
            // if (transform != null) {
            // Vector2 pos = transform.getPosition();
            // Vector2 velocity = physics.getVelocity();
            //
            // if (pos.x <= 0 || pos.x >= 800 - 15) {
            // velocity.x = -velocity.x;
            // physics.setVelocity(velocity);
            // }
            // if (pos.y <= 0 || pos.y >= 600 - 15) {
            // velocity.y = -velocity.y;
            // physics.setVelocity(velocity);
            // }
            //
            // // 确保在边界内
            // if (pos.x < 0) pos.x = 0;
            // if (pos.y < 0) pos.y = 0;
            // if (pos.x > 800 - 15) pos.x = 800 - 15;
            // if (pos.y > 600 - 15) pos.y = 600 - 15;
            // transform.setPosition(pos);
            // }
        }

        // ========== 性能统计 ==========
        long endTime = System.nanoTime();
        totalPhysicsTime += (endTime - startTime);
    }

    /**
     * 检查碰撞
     */
    public void checkCollisions() {
        // ========== 性能统计 ==========
        long startTime = System.nanoTime();
        // // 直接查找玩家对象
        // List<GameObject> players =
        // scene.findGameObjectsByComponent(TransformComponent.class);
        // if (players.isEmpty()) return;

        // GameObject player = players.get(0);
        // TransformComponent playerTransform =
        // player.getComponent(TransformComponent.class);
        // if (playerTransform == null) return;

        // // 直接查找所有游戏对象，然后过滤出敌人
        // for (GameObject obj : scene.getGameObjects()) {
        // if (obj.getName().equals("Enemy")) {
        // TransformComponent enemyTransform =
        // obj.getComponent(TransformComponent.class);
        // if (enemyTransform != null) {
        // float distance =
        // playerTransform.getPosition().distance(enemyTransform.getPosition());
        // if (distance < 25) {
        // // 碰撞！重置玩家位置
        // playerTransform.setPosition(new Vector2(400, 300));
        // break;
        // }
        // }
        // }
        // }
        // for (GameObject obj : scene.getGameObjects()) {
        // if (obj.getName().equals("Enemy")) {
        // TransformComponent enemyTransform =
        // obj.getComponent(TransformComponent.class);
        // if (enemyTransform != null) {
        // float distance =
        // playerTransform.getPosition().distance(enemyTransform.getPosition());
        // if (distance < 25) {
        // // 碰撞！重置玩家位置
        // playerTransform.setPosition(new Vector2(400, 300));
        // break;
        // }
        // }
        // }
        // }
        List<GameObject> objs = scene.getGameObjects();
        Dictionary<GameObject, List<GameObject>> sendList = new java.util.Hashtable<>();

        for (GameObject a : objs) {
            if (!a.hasComponent(PhysicsComponent.class))
                continue;

            List<GameObject> colliders = new ArrayList<>();

            for (GameObject b : objs) {
                if (!b.hasComponent(PhysicsComponent.class))
                    continue;

                if (objs.indexOf(a) != objs.indexOf(b)) {
                    float distance = a.getComponent(TransformComponent.class).getPosition()
                            .distance(b.getComponent(TransformComponent.class).getPosition());
                    if (distance < 25) {
                        // 碰撞
                        // System.out.println("Collision detected between " + a.getName() + " and " +
                        // b.getName());
                        colliders.add(b);
                    }
                }
            }

            // if (a.tag.equals("Enemy") && colliders.size() > 0) {
            // System.out.println("a collides with " + colliders.size() + " objects.");
            // }
            if (!colliders.isEmpty()) {
                sendList.put(a, colliders);
            }
        }
        for (Enumeration<GameObject> e = sendList.keys(); e.hasMoreElements();) {
            GameObject obj = e.nextElement();
            obj.onCollisionEnter(sendList.get(obj));
        }

        // 性能统计
        long endTime = System.nanoTime();
        totalCollisionTime += (endTime - startTime);
        frameCount++;

        // 每3秒输出一次统计信息
        long currentTime = System.currentTimeMillis();
        if (currentTime - lastPrintTime >= 3000) {
            double avgPhysicsMs = (totalPhysicsTime / (double) frameCount) / 1_000_000.0;
            double avgCollisionMs = (totalCollisionTime / (double) frameCount) / 1_000_000.0;
            double totalMs = avgPhysicsMs + avgCollisionMs;
            System.out.println(String.format(
                    "========== 性能统计 ========== | 物理更新: %.2fms | 碰撞检测: %.2fms | 总计: %.2fms | 帧数: %d",
                    avgPhysicsMs, avgCollisionMs, totalMs, frameCount));
            // 重置统计
            totalPhysicsTime = 0;
            totalCollisionTime = 0;
            frameCount = 0;
            lastPrintTime = currentTime;
        }
    }
}
