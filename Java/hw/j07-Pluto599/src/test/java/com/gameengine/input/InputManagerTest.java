package com.gameengine.input;

import com.gameengine.math.Vector2;
import com.gameengine.testsupport.TestOutputExtension;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;

import static org.junit.jupiter.api.Assertions.*;

@ExtendWith(TestOutputExtension.class)
class InputManagerTest {

	@Test
	void keyPressedJustPressedAndUpdateBehavior() {
		InputManager input = InputManager.getInstance();
		int key = 65; // 'A'

		input.onKeyPressed(key);
		assertTrue(input.isKeyPressed(key));
		assertTrue(input.isKeyJustPressed(key));
		assertTrue(input.isAnyKeyJustPressed());

		input.update();
		assertTrue(input.isKeyPressed(key));
		assertFalse(input.isKeyJustPressed(key));
		assertFalse(input.isAnyKeyJustPressed());

		input.onKeyReleased(key);
		assertFalse(input.isKeyPressed(key));
		input.update();
	}

	@Test
	void mousePositionAndButtonsWork() {
		InputManager input = InputManager.getInstance();

		input.onMouseMoved(12.5f, 9.25f);
		Vector2 pos = input.getMousePosition();
		assertEquals(new Vector2(12.5f, 9.25f), pos);
		assertEquals(12.5f, input.getMouseX(), 1e-6);
		assertEquals(9.25f, input.getMouseY(), 1e-6);

		int button = 1;
		input.onMousePressed(button);
		assertTrue(input.isMouseButtonPressed(button));
		assertTrue(input.isMouseButtonJustPressed(button));

		input.update();
		assertTrue(input.isMouseButtonPressed(button));
		assertFalse(input.isMouseButtonJustPressed(button));

		input.onMouseReleased(button);
		assertFalse(input.isMouseButtonPressed(button));
	}
}
