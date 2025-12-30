package com.gameengine.objects;

import com.gameengine.components.PhysicsComponent;
import com.gameengine.components.TransformComponent;
import com.gameengine.math.Vector2;
import com.gameengine.scene.Scene;
import com.gameengine.testsupport.TestOutputExtension;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;

import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

@ExtendWith(TestOutputExtension.class)
class ObjectsTest {

	@Test
	void bulletDestroyedWhenHitsEnemy() {
		Bullet bullet = new Bullet("b", null, new Vector2(0, 0), new Vector2(1, 0));
		bullet.initialize();

		Enemy enemy = new Enemy("e", null);
		enemy.initialize();

		bullet.onCollisionEnter(List.of(enemy));
		assertFalse(bullet.isActive());
	}

	@Test
	void enemyGivesScoreAndDiesWhenHitByBullet() {
		Scene scene = new Scene("test");
		Player.resetInstance();
		Player player = Player.getInstance("p", scene, null, 0, 0);
		player.initialize();

		Enemy enemy = new Enemy("e", null);
		enemy.initialize();

		Bullet bullet = new Bullet("b", null, new Vector2(0, 0), new Vector2(1, 0));
		bullet.initialize();

		int before = player.getScore();
		enemy.onCollisionEnter(List.of(bullet));
		assertFalse(enemy.isActive());
		assertEquals(before + 10, player.getScore());

		Player.resetInstance();
	}

	@Test
	void enemyChasesPlayerBySettingVelocity() {
		Scene scene = new Scene("test");
		Player.resetInstance();
		Player player = Player.getInstance("p", scene, null, 50, 0);
		player.initialize();

		Enemy enemy = new Enemy("e", null);
		enemy.initialize();

		enemy.getComponent(TransformComponent.class).setPosition(new Vector2(0, 0));
		enemy.update(0.016f);

		PhysicsComponent physics = enemy.getComponent(PhysicsComponent.class);
		assertNotNull(physics);
		assertTrue(physics.getVelocity().x > 0.0f);

		Player.resetInstance();
	}

	@Test
	void playerCollisionResetsPositionAndScoreWhenNotHandledByScene() {
		Scene scene = new Scene("test");
		Player.resetInstance();
		Player player = Player.getInstance("p", scene, null, 10, 10);
		player.initialize();
		player.setScore(123);

		Enemy enemy = new Enemy("e", null);
		enemy.initialize();

		player.onCollisionEnter(List.of(enemy));

		Vector2 pos = player.getComponent(TransformComponent.class).getPosition();
		assertEquals(new Vector2(400, 300), pos);
		assertEquals(0, player.getScore());

		Player.resetInstance();
	}
}
