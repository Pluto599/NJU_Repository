package com.gameengine.objects;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.gameengine.components.PhysicsComponent;
import com.gameengine.components.TransformComponent;
import com.gameengine.core.GameObject;
import com.gameengine.graphics.Renderer;
import com.gameengine.input.InputManager;
import com.gameengine.math.Vector2;
import com.gameengine.scene.Scene;

public class Player extends GameObject {
	// 单例实例
	private static Player instance;

	private static final int KEY_I = 73; // I - 上
	private static final int KEY_J = 74; // J - 左
	private static final int KEY_K = 75; // K - 下
	private static final int KEY_L = 76; // L - 右

	private final Map<Integer, Vector2> shootDirections = new HashMap<>();

	private Vector2 basePosition;

	private Scene scene;
	private Renderer renderer;

	private boolean shoot = true;
	private float shootCooldown = 0.2f; // 射击冷却时间（秒）
	private float shootTimer = 0.0f;

	private int score = 0;

	/**
	 * 获取Player实例
	 * 
	 * @param name     玩家名称
	 * @param scene    游戏场景
	 * @param renderer 渲染器
	 * @return Player实例
	 */
	public static Player getInstance(String name, Scene scene, Renderer renderer) {
		return getInstance(name, scene, renderer, 400, 300);
	}

	/**
	 * 获取Player实例（指定初始位置）
	 * 
	 * @param name     玩家名称
	 * @param scene    游戏场景
	 * @param renderer 渲染器
	 * @param startX   初始X坐标
	 * @param startY   初始Y坐标
	 * @return Player实例
	 */
	public static Player getInstance(String name, Scene scene, Renderer renderer, float startX, float startY) {
		if (instance == null) {
			instance = new Player(name, scene, renderer, startX, startY);
		}
		return instance;
	}

	/**
	 * 获取已创建的Player实例
	 * 
	 * @return Player实例，如果未创建则返回null
	 */
	public static Player getInstance() {
		return instance;
	}

	/**
	 * 重置Player实例
	 */
	public static void resetInstance() {
		instance = null;
	}

	private Player(String name, Scene scene, Renderer renderer, float startX, float startY) {
		super(name);
		this.scene = scene;
		this.renderer = renderer;

		// 初始化射击方向映射
		shootDirections.put(KEY_I, new Vector2(0, -1)); // I - 上
		shootDirections.put(KEY_J, new Vector2(-1, 0)); // J - 左
		shootDirections.put(KEY_K, new Vector2(0, 1)); // K - 下
		shootDirections.put(KEY_L, new Vector2(1, 0)); // L - 右

		// 添加变换组件（使用传入的初始位置）
		this.addComponent(new TransformComponent(new Vector2(startX, startY)));

		// 添加物理组件
		PhysicsComponent physics = this.addComponent(new PhysicsComponent(1f));
		physics.setFriction(500f);
	}

	@Override
	public void initialize() {
		tag = "Player";
	}

	@Override
	public void update(float deltaTime) {
		super.update(deltaTime);
		updateComponents(deltaTime);

		// 更新所有部位的位置
		updateBodyParts();

		// 处理玩家射击
		handlePlayerShooting();

		if (shoot == false) {
			shootTimer += deltaTime;
			if (shootTimer >= shootCooldown) {
				shoot = true;
				shootTimer = 0.0f;
			}
		}
	}

	@Override
	public void render() {
		// 渲染所有部位
		renderBodyParts();
	}

	@Override
	public void onCollisionEnter(List<GameObject> other) {
		for (GameObject obj : other) {
			if (obj.tag.equals("Enemy")) {
				boolean handled = false;
				if (scene != null) {
					handled = scene.onPlayerDeath();
				}
				if (!handled) {
					// 默认行为：重置位置与分数（兼容联机等不处理死亡的场景）
					TransformComponent transform = getComponent(TransformComponent.class);
					if (transform != null) {
						transform.setPosition(new Vector2(400, 300));
					}
					resetScore();
				}
				break;
			}
		}
	}

	private void updateBodyParts() {
		TransformComponent transform = getComponent(TransformComponent.class);
		if (transform != null) {
			basePosition = transform.getPosition();
		}
	}

	private void renderBodyParts() {
		if (basePosition == null)
			return;

		// 渲染身体
		renderer.drawRect(
				basePosition.x - 8, basePosition.y - 10, 16, 20,
				1.0f, 0.0f, 0.0f, 1.0f // 红色
		);

		// 渲染头部
		renderer.drawRect(
				basePosition.x - 6, basePosition.y - 22, 12, 12,
				1.0f, 0.5f, 0.0f, 1.0f // 橙色
		);

		// 渲染左臂
		renderer.drawRect(
				basePosition.x - 13, basePosition.y - 5, 6, 12,
				1.0f, 0.8f, 0.0f, 1.0f // 黄色
		);

		// 渲染右臂
		renderer.drawRect(
				basePosition.x + 7, basePosition.y - 5, 6, 12,
				0.0f, 1.0f, 0.0f, 1.0f // 绿色
		);
	}

	/**
	 * 处理玩家射击
	 */
	public void handlePlayerShooting() {
		Vector2 position = getComponent(TransformComponent.class).getPosition();
		Vector2 direction = new Vector2();

		// 检测IJKL键
		InputManager inputManager = InputManager.getInstance();
		for (Map.Entry<Integer, Vector2> entry : shootDirections.entrySet()) {
			if (inputManager.isKeyPressed(entry.getKey()) && shoot) {
				// System.out.println("Key " + entry.getKey() + " pressed");
				direction = entry.getValue();
				shoot = false;
				shootBullet(position, direction);

				break;
			}
		}
	}

	private void shootBullet(Vector2 position, Vector2 direction) {
		// System.out.println("Bullet shoot!" );
		// 创建子弹对象
		Bullet bullet = new Bullet("PlayerBullet", renderer, position, direction.normalize());
		bullet.initialize();

		if (scene != null) {
			scene.addGameObject(bullet);
		} else {
			System.err.println("Warning: Cannot add bullet, scene reference is null");
		}
	}

	/**
	 * 增加分数
	 */
	public void addScore(int points) {
		this.score += points;
		// System.out.println("得分：" + score);
	}

	/**
	 * 重置分数
	 */
	public void resetScore() {
		this.score = 0;
		// System.out.println("分数已重置");
	}

	/**
	 * 获取当前分数
	 */
	public int getScore() {
		return score;
	}

	public void setScore(int score) {
		this.score = Math.max(0, score);
	}
}
