package com.gameengine.recording;

import com.gameengine.testsupport.TestOutputExtension;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;

import static org.junit.jupiter.api.Assertions.*;

@ExtendWith(TestOutputExtension.class)
class RecordingJsonTest {

	@Test
	void fieldExtractsSimpleValues() {
		String json = "{\"type\":\"kf\",\"t\":123.5,\"id\":\"abc\"}";
		assertEquals("\"kf\"", RecordingJson.field(json, "type"));
		assertEquals("123.5", RecordingJson.field(json, "t"));
		assertEquals("\"abc\"", RecordingJson.field(json, "id"));
	}

	@Test
	void stripQuotesAndParseDouble() {
		assertEquals("abc", RecordingJson.stripQuotes("\"abc\""));
		assertEquals("123", RecordingJson.stripQuotes("123"));
		assertEquals(12.34, RecordingJson.parseDouble("12.34"), 1e-9);
		assertEquals(12.34, RecordingJson.parseDouble("\"12.34\""), 1e-9);
		assertEquals(0.0, RecordingJson.parseDouble("not-a-number"), 1e-9);
	}

	@Test
	void splitTopLevelRespectsBraces() {
		String content = "{\"a\":1},{\"b\":{\"c\":2}},{\"d\":3}";
		String[] parts = RecordingJson.splitTopLevel(content);
		assertEquals(3, parts.length);
		assertTrue(parts[0].contains("\"a\""));
		assertTrue(parts[1].contains("\"b\""));
		assertTrue(parts[2].contains("\"d\""));
	}

	@Test
	void extractArrayReturnsInnerContent() {
		String json = "{\"entities\":[{\"id\":\"a\"},{\"id\":\"b\"}]}";
		int idx = json.indexOf('[');
		String inner = RecordingJson.extractArray(json, idx);
		assertTrue(inner.contains("\"a\""));
		assertTrue(inner.contains("\"b\""));
		assertFalse(inner.contains("["));
		assertFalse(inner.contains("]"));
	}
}
