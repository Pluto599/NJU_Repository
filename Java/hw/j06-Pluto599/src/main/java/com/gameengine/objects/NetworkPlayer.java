package com.gameengine.objects;

import java.util.List;

import com.gameengine.components.PhysicsComponent;
import com.gameengine.components.RenderComponent;
import com.gameengine.components.TransformComponent;
import com.gameengine.core.GameObject;
import com.gameengine.graphics.Renderer;
import com.gameengine.math.Vector2;
import com.gameengine.net.NetState;

/**
 * 由远程客户端控制的玩家镜像，由服务器在本地场景中驱动。
 */
public class NetworkPlayer extends GameObject {
    private final Renderer renderer;

    public NetworkPlayer(String name, Renderer renderer, Vector2 spawnPosition) {
        super(name);
        this.renderer = renderer;

        addComponent(new TransformComponent(new Vector2(spawnPosition)));
        RenderComponent render = addComponent(new RenderComponent(
                RenderComponent.RenderType.RECTANGLE,
                new Vector2(20, 20),
                new RenderComponent.Color(0.2f, 1.0f, 0.2f, 1.0f)));
        render.setRenderer(renderer);

        PhysicsComponent physics = addComponent(new PhysicsComponent(1.0f));
        physics.setFriction(500f);
    }

    @Override
    public void initialize() {
        tag = "Player";
    }

    @Override
    public void update(float deltaTime) {
        PhysicsComponent physics = getComponent(PhysicsComponent.class);
        if (physics != null) {
            physics.setVelocity(new Vector2(NetState.getP2Vx(), NetState.getP2Vy()));
        }
        super.update(deltaTime);
        clampInsideBounds();
    }

    @Override
    public void onCollisionEnter(List<GameObject> other) {
        for (GameObject obj : other) {
            if ("Enemy".equals(obj.tag)) {
                resetPosition();
                break;
            }
        }
    }

    private void clampInsideBounds() {
        TransformComponent transform = getComponent(TransformComponent.class);
        if (transform == null) {
            return;
        }
        Vector2 position = transform.getPosition();
        float maxX = renderer.getWidth() - 20;
        float maxY = renderer.getHeight() - 20;
        position.x = Math.max(0, Math.min(maxX, position.x));
        position.y = Math.max(0, Math.min(maxY, position.y));
        transform.setPosition(position);
    }

    private void resetPosition() {
        TransformComponent transform = getComponent(TransformComponent.class);
        if (transform != null) {
            transform.setPosition(new Vector2(renderer.getWidth() / 2.0f + 40, renderer.getHeight() / 2.0f));
        }
    }
}
