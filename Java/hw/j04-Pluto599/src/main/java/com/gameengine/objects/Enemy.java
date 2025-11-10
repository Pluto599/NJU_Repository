package com.gameengine.objects;

import java.util.List;
import java.util.Random;

import com.gameengine.components.PhysicsComponent;
import com.gameengine.components.RenderComponent;
import com.gameengine.components.TransformComponent;
import com.gameengine.core.GameObject;
import com.gameengine.math.Vector2;
import com.gameengine.graphics.Renderer;

public class Enemy extends GameObject {
	private static final Random random = new Random();
	private static int gameWidth = 800; // 默认值
	private static int gameHeight = 600; // 默认值

	/**
	 * 设置游戏屏幕尺寸（用于敌人生成位置）
	 */
	public static void setGameDimensions(int width, int height) {
		gameWidth = width;
		gameHeight = height;
	}

	public Enemy(String name, Renderer renderer) {
		super(name);

		// 随机位置（动态适配屏幕尺寸）
		Vector2 position = new Vector2(
				random.nextFloat() * gameWidth,
				random.nextFloat() * gameHeight);

		// 添加变换组件
		this.addComponent(new TransformComponent(position));

		// 添加渲染组件 - 改为矩形，使用橙色
		RenderComponent render = this.addComponent(new RenderComponent(
				RenderComponent.RenderType.RECTANGLE,
				new Vector2(20, 20),
				new RenderComponent.Color(1.0f, 0.5f, 0.0f, 1.0f) // 橙色
		));
		render.setRenderer(renderer);

		// 添加物理组件
		PhysicsComponent physics = this.addComponent(new PhysicsComponent(0.5f));
		// physics.setVelocity(new Vector2(
		// (random.nextFloat() - 0.5f) * 100,
		// (random.nextFloat() - 0.5f) * 100));
		physics.setFriction(0f);
	}

	@Override
	public void initialize() {
		tag = "Enemy";
	}

	@Override
	public void update(float deltaTime) {
		super.update(deltaTime);
		updateComponents(deltaTime);

		// 敌人寻路
		TransformComponent transform = this.getComponent(TransformComponent.class);
		PhysicsComponent physics = this.getComponent(PhysicsComponent.class);

		Player player = Player.getInstance();
		if (player != null) {
			TransformComponent playerTransform = player.getComponent(TransformComponent.class);

			// 计算方向向量 (从敌人到玩家)
			Vector2 direction = new Vector2(
					playerTransform.getPosition().x - transform.getPosition().x,
					playerTransform.getPosition().y - transform.getPosition().y).normalize();

			// 设置敌人移动速度
			physics.setVelocity(direction.multiply(50));
		}
	}

	@Override
	public void onCollisionEnter(List<GameObject> other) {
		for (GameObject obj : other) {
			if (obj.tag.equals("Player")) {
				// 碰到玩家，敌人消失
				// System.out.println("Enemy hit Player!");
				this.destroy();
				break;
			} else if (obj.tag.equals("Bullet")) {
				// 碰到子弹，敌人消失
				// System.out.println("Enemy hit by Bullet!");

				// 增加玩家分数
				Player player = Player.getInstance();
				if (player != null) {
					player.addScore(10);
				}

				this.destroy();
				break;
			}
		}
	}

}
