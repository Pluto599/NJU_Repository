package com.gameengine.recording;

import java.util.ArrayList;
import java.util.List;

/**
 * 轻量 JSON 工具，供网络与录制模块复用，避免依赖外部库。
 */
public final class RecordingJson {
	private RecordingJson() {
	}

	public static String field(String json, String key) {
		if (json == null || key == null) {
			return null;
		}
		int start = json.indexOf("\"" + key + "\"");
		if (start < 0) {
			return null;
		}
		int colon = json.indexOf(':', start);
		if (colon < 0) {
			return null;
		}
		int cursor = colon + 1;
		int comma = json.indexOf(',', cursor);
		int brace = json.indexOf('}', cursor);
		int end = comma < 0 ? brace : (brace < 0 ? comma : Math.min(comma, brace));
		if (end < 0) {
			end = json.length();
		}
		return json.substring(cursor, end).trim();
	}

	public static String stripQuotes(String text) {
		if (text == null) {
			return null;
		}
		String trimmed = text.trim();
		if (trimmed.length() >= 2 && trimmed.startsWith("\"") && trimmed.endsWith("\"")) {
			return trimmed.substring(1, trimmed.length() - 1);
		}
		return trimmed;
	}

	public static double parseDouble(String value) {
		if (value == null) {
			return 0.0;
		}
		try {
			return Double.parseDouble(stripQuotes(value));
		} catch (NumberFormatException ex) {
			return 0.0;
		}
	}

	public static String[] splitTopLevel(String content) {
		List<String> segments = new ArrayList<>();
		if (content == null || content.isEmpty()) {
			return new String[0];
		}
		int depth = 0;
		int start = 0;
		for (int i = 0; i < content.length(); i++) {
			char ch = content.charAt(i);
			if (ch == '{') {
				depth++;
			} else if (ch == '}') {
				depth--;
			} else if (ch == ',' && depth == 0) {
				String segment = content.substring(start, i).trim();
				if (!segment.isEmpty()) {
					segments.add(segment);
				}
				start = i + 1;
			}
		}
		if (start < content.length()) {
			String tail = content.substring(start).trim();
			if (!tail.isEmpty()) {
				segments.add(tail);
			}
		}
		return segments.toArray(new String[0]);
	}

	public static String extractArray(String json, int startIdx) {
		if (json == null || startIdx < 0 || startIdx >= json.length() || json.charAt(startIdx) != '[') {
			return "";
		}
		int depth = 1;
		int begin = startIdx + 1;
		for (int i = startIdx + 1; i < json.length(); i++) {
			char ch = json.charAt(i);
			if (ch == '[') {
				depth++;
			} else if (ch == ']') {
				depth--;
				if (depth == 0) {
					return json.substring(begin, i);
				}
			}
		}
		return "";
	}
}
