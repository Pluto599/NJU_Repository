package com.gameengine.core;

import com.gameengine.components.PhysicsComponent;
import com.gameengine.components.TransformComponent;
import com.gameengine.math.Vector2;
import com.gameengine.testsupport.TestOutputExtension;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;

import static org.junit.jupiter.api.Assertions.*;

@ExtendWith(TestOutputExtension.class)
class GameObjectAndComponentsTest {

	static class CountingComponent extends Component<CountingComponent> {
		int initCalls;
		int updateCalls;
		int renderCalls;

		@Override
		public void initialize() {
			initCalls++;
		}

		@Override
		public void update(float deltaTime) {
			updateCalls++;
		}

		@Override
		public void render() {
			renderCalls++;
		}
	}

	@Test
	void addGetHasAndDestroyWork() {
		GameObject obj = new GameObject("t");
		CountingComponent comp = obj.addComponent(new CountingComponent());

		assertEquals(1, comp.initCalls);
		assertSame(comp, obj.getComponent(CountingComponent.class));
		assertTrue(obj.hasComponent(CountingComponent.class));

		obj.update(0.016f);
		obj.render();
		assertEquals(1, comp.updateCalls);
		assertEquals(1, comp.renderCalls);

		comp.setEnabled(false);
		obj.update(0.016f);
		obj.render();
		assertEquals(1, comp.updateCalls);
		assertEquals(1, comp.renderCalls);

		obj.destroy();
		assertFalse(obj.isActive());
		assertNull(obj.getComponent(CountingComponent.class));
	}

	@Test
	void physicsComponentMovesTransform() {
		GameObject obj = new GameObject("p");
		TransformComponent transform = obj.addComponent(new TransformComponent(new Vector2(0, 0)));
		PhysicsComponent physics = obj.addComponent(new PhysicsComponent());
		physics.setFriction(0);

		physics.applyImpulse(new Vector2(10, 0));
		obj.update(1.0f);

		Vector2 pos = transform.getPosition();
		assertTrue(pos.x > 0.0f);
	}
}
