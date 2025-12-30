package com.gameengine.math;

import com.gameengine.testsupport.TestOutputExtension;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;

import static org.junit.jupiter.api.Assertions.*;

@ExtendWith(TestOutputExtension.class)
class Vector2Test {

	@Test
	void addSubtractMultiplyWork() {
		Vector2 a = new Vector2(1, 2);
		Vector2 b = new Vector2(3, 4);

		assertEquals(new Vector2(4, 6), a.add(b));
		assertEquals(new Vector2(-2, -2), a.subtract(b));
		assertEquals(new Vector2(2, 4), a.multiply(2));
	}

	@Test
	void magnitudeNormalizeDotDistanceWork() {
		Vector2 v = new Vector2(3, 4);
		assertEquals(5.0f, v.magnitude(), 1e-6);

		Vector2 n = v.normalize();
		assertEquals(1.0f, n.magnitude(), 1e-6);

		assertEquals(25.0f, v.dot(v), 1e-6);
		assertEquals(5.0f, new Vector2(0, 0).distance(v), 1e-6);
	}

	@Test
	void normalizeZeroGivesZero() {
		assertEquals(new Vector2(0, 0), new Vector2(0, 0).normalize());
	}

	@Test
	void equalsUsesExactComponents() {
		assertEquals(new Vector2(1, 1), new Vector2(1, 1));
		assertNotEquals(new Vector2(1, 1), new Vector2(1, 1.0001f));
	}
}
