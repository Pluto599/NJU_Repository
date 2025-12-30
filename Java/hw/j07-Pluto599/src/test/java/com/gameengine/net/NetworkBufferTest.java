package com.gameengine.net;

import com.gameengine.testsupport.TestOutputExtension;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;

import java.util.Map;

import static org.junit.jupiter.api.Assertions.*;

@ExtendWith(TestOutputExtension.class)
class NetworkBufferTest {

	@Test
	void parseJsonLineParsesKeyframe() {
		String line = "{\"type\":\"kf\",\"t\":123.0,\"entities\":[{\"id\":\"p1\",\"x\":1,\"y\":2},{\"id\":\"p2\",\"x\":3.5,\"y\":-4}]}";
		NetworkBuffer.Keyframe kf = NetworkBuffer.parseJsonLine(line);
		assertNotNull(kf);
		assertEquals(123.0, kf.t, 1e-6);
		assertEquals(2, kf.entities.size());
		assertEquals("p1", kf.entities.get(0).id);
		assertEquals(1.0f, kf.entities.get(0).x, 1e-6);
		assertEquals(2.0f, kf.entities.get(0).y, 1e-6);
	}

	@Test
	void sampleInterpolatesAroundTargetTime() {
		String id = "unit-test-entity";
		double now = System.currentTimeMillis() / 1000.0;
		double t1 = now - 0.12 - 0.05;
		double t2 = now - 0.12 + 0.05;

		NetworkBuffer.Keyframe a = new NetworkBuffer.Keyframe();
		a.t = t1;
		NetworkBuffer.Entity ea = new NetworkBuffer.Entity();
		ea.id = id;
		ea.x = 0;
		ea.y = 0;
		a.entities.add(ea);

		NetworkBuffer.Keyframe b = new NetworkBuffer.Keyframe();
		b.t = t2;
		NetworkBuffer.Entity eb = new NetworkBuffer.Entity();
		eb.id = id;
		eb.x = 10;
		eb.y = 20;
		b.entities.add(eb);

		NetworkBuffer.push(a);
		NetworkBuffer.push(b);

		Map<String, float[]> out = NetworkBuffer.sample();
		assertTrue(out.containsKey(id));
		float[] xy = out.get(id);
		assertNotNull(xy);
		assertEquals(2, xy.length);
		assertTrue(xy[0] >= 0.0f && xy[0] <= 10.0f);
		assertTrue(xy[1] >= 0.0f && xy[1] <= 20.0f);
	}
}
