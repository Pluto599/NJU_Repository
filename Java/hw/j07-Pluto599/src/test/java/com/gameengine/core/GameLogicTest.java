package com.gameengine.core;

import com.gameengine.components.PhysicsComponent;
import com.gameengine.components.TransformComponent;
import com.gameengine.input.InputManager;
import com.gameengine.math.Vector2;
import com.gameengine.objects.Player;
import com.gameengine.scene.Scene;
import com.gameengine.testsupport.TestOutputExtension;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;

import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

@ExtendWith(TestOutputExtension.class)
class GameLogicTest {

	static class CollideObject extends GameObject {
		int collisionCalls;
		int lastOtherSize;

		CollideObject(String name) {
			super(name);
			tag = name;
			addComponent(new TransformComponent(new Vector2(0, 0)));
			addComponent(new PhysicsComponent());
		}

		@Override
		public void onCollisionEnter(List<GameObject> other) {
			collisionCalls++;
			lastOtherSize = other == null ? -1 : other.size();
		}
	}

	@Test
	void handlePlayerInputSetsVelocityAndClampsPosition() {
		Scene scene = new Scene("test");
		Player.resetInstance();
		Player player = Player.getInstance("p", scene, null, 200, 200);
		player.initialize();
		scene.addGameObject(player);
		scene.update(0);

		GameLogic logic = new GameLogic(scene, 100, 80);
		InputManager input = InputManager.getInstance();

		try {
			input.onKeyPressed(87); // W
			logic.handlePlayerInput();
			PhysicsComponent physics = player.getComponent(PhysicsComponent.class);
			assertNotNull(physics);
			assertTrue(physics.getVelocity().y < 0);

			TransformComponent transform = player.getComponent(TransformComponent.class);
			assertNotNull(transform);
			Vector2 pos = transform.getPosition();
			assertEquals(80.0f, pos.x, 1e-6); // gameWidth - 20
			assertEquals(60.0f, pos.y, 1e-6); // gameHeight - 20
		} finally {
			input.onKeyReleased(87);
			input.update();
			logic.cleanup();
			Player.resetInstance();
		}
	}

	@Test
	void checkCollisionsInvokesCollisionCallback() {
		Scene scene = new Scene("test");
		CollideObject a = new CollideObject("A");
		CollideObject b = new CollideObject("B");

		// Make them overlap (distance 0)
		a.getComponent(TransformComponent.class).setPosition(new Vector2(10, 10));
		b.getComponent(TransformComponent.class).setPosition(new Vector2(10, 10));

		scene.addGameObject(a);
		scene.addGameObject(b);
		scene.update(0);

		GameLogic logic = new GameLogic(scene, 100, 80);
		try {
			logic.checkCollisions();
			assertEquals(1, a.collisionCalls);
			assertEquals(1, b.collisionCalls);
			assertEquals(1, a.lastOtherSize);
			assertEquals(1, b.lastOtherSize);
		} finally {
			logic.cleanup();
		}
	}

	@Test
	void updatePhysicsDestroysOutOfBoundsNonPlayer() {
		Scene scene = new Scene("test");
		CollideObject obj = new CollideObject("Bullet");
		obj.tag = "Bullet";
		obj.getComponent(TransformComponent.class).setPosition(new Vector2(-1, -1));
		scene.addGameObject(obj);
		scene.update(0);

		GameLogic logic = new GameLogic(scene, 100, 80);
		try {
			logic.updatePhysics();
			assertFalse(obj.isActive());
		} finally {
			logic.cleanup();
		}
	}
}
