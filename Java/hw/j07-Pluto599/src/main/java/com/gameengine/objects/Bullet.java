package com.gameengine.objects;

import java.util.List;

import com.gameengine.components.PhysicsComponent;
import com.gameengine.components.RenderComponent;
import com.gameengine.components.TransformComponent;
import com.gameengine.core.GameObject;
import com.gameengine.graphics.Renderer;
import com.gameengine.math.Vector2;

public class Bullet extends GameObject {
	private Renderer renderer;

	public Bullet(String name, Renderer renderer, Vector2 position, Vector2 direction) {
		super(name);
		this.renderer = renderer;

		// 添加变换组件
		TransformComponent transform = this.addComponent(new TransformComponent(position));

		// 添加渲染组件 - 改为矩形，使用蓝色
		RenderComponent render = this.addComponent(new RenderComponent(
				RenderComponent.RenderType.RECTANGLE,
				new Vector2(5, 5),
				new RenderComponent.Color(0.0f, 0.5f, 1.0f, 1.0f) // 蓝色
		));
		render.setRenderer(renderer);

		// 添加物理组件
		PhysicsComponent physics = this.addComponent(new PhysicsComponent(0.5f));
		physics.setFriction(0f);
		physics.setVelocity(direction.multiply(300));
	}

	@Override
	public void initialize() {
		tag = "Bullet";
	}

	@Override
	public void update(float deltaTime) {
		super.update(deltaTime);
		updateComponents(deltaTime);

		// // 超出屏幕边界时销毁
		// TransformComponent transform = getComponent(TransformComponent.class);
		// if (transform != null) {
		// Vector2 pos = transform.getPosition();
		// if (pos.x <= 0 || pos.x >= 800 || pos.y <= 0 || pos.y >= 600) {
		// this.destroy();
		// }
		// }
	}

	@Override
	public void onCollisionEnter(List<GameObject> other) {
		for (GameObject obj : other) {
			if (obj.tag.equals("Enemy")) {
				// 碰到敌人，子弹消失
				// System.out.println("Bullet hit Enemy!");
				this.destroy();
				break;
			}
		}
	}
}
