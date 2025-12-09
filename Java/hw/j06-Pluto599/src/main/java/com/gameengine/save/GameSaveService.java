package com.gameengine.save;

import com.gameengine.math.Vector2;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
import java.util.Optional;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.Collectors;
import java.util.stream.Stream;

public class GameSaveService {
	private final Path savePath;

	public GameSaveService(String savePath) {
		this.savePath = Paths.get(savePath);
	}

	public void save(GameSaveData data) throws IOException {
		if (data == null) {
			return;
		}
		Path parent = savePath.getParent();
		if (parent != null) {
			Files.createDirectories(parent);
		}
		String json = buildJson(data);
		Files.write(savePath, json.getBytes(StandardCharsets.UTF_8));
	}

	public Optional<GameSaveData> load() throws IOException {
		if (!Files.exists(savePath) || !Files.isRegularFile(savePath)) {
			return Optional.empty();
		}
		String content = Files.readString(savePath, StandardCharsets.UTF_8);
		if (content == null || content.trim().isEmpty()) {
			return Optional.empty();
		}

		GameSaveData data = new GameSaveData();
		data.width = (int) Math.round(extractNumber(content, "width", 0));
		data.height = (int) Math.round(extractNumber(content, "height", 0));

		String playerSection = extractObject(content, "player");
		if (!playerSection.isEmpty()) {
			data.playerScore = (int) Math.round(extractNumber(playerSection, "score", 0));
			String positionSection = extractObject(playerSection, "position");
			float px = (float) extractNumber(positionSection, "x", 0f);
			float py = (float) extractNumber(positionSection, "y", 0f);
			data.playerPosition = new Vector2(px, py);
		}

		String enemiesSection = extractArray(content, "enemies");
		List<String> enemyObjects = splitObjects(enemiesSection);
		for (String enemyObject : enemyObjects) {
			EnemySnapshot snapshot = new EnemySnapshot();
			snapshot.state = extractString(enemyObject, "state", "active");
			snapshot.active = extractBoolean(enemyObject, "active", true);
			String enemyPosition = extractObject(enemyObject, "position");
			float ex = (float) extractNumber(enemyPosition, "x", 0f);
			float ey = (float) extractNumber(enemyPosition, "y", 0f);
			snapshot.position = new Vector2(ex, ey);
			String enemyVelocity = extractObject(enemyObject, "velocity");
			float vx = (float) extractNumber(enemyVelocity, "x", 0f);
			float vy = (float) extractNumber(enemyVelocity, "y", 0f);
			snapshot.velocity = new Vector2(vx, vy);
			data.enemies.add(snapshot);
		}

		return Optional.of(data);
	}

	public static List<Path> listSaveFiles(String directory) {
		Path dir = Paths.get(directory);
		if (!Files.exists(dir) || !Files.isDirectory(dir)) {
			return new ArrayList<>();
		}
		try (Stream<Path> stream = Files.list(dir)) {
			return stream
					.filter(Files::isRegularFile)
					.filter(p -> p.getFileName().toString().toLowerCase().endsWith(".json"))
					.sorted(Comparator.comparingLong((Path p) -> {
						try {
							return Files.getLastModifiedTime(p).toMillis();
						} catch (IOException e) {
							return 0L;
						}
					}).reversed())
					.collect(Collectors.toList());
		} catch (IOException e) {
			return new ArrayList<>();
		}
	}

	private String buildJson(GameSaveData data) {
		StringBuilder sb = new StringBuilder();
		sb.append("{\n");
		sb.append("  \"width\": ").append(data.width).append(",\n");
		sb.append("  \"height\": ").append(data.height).append(",\n");
		sb.append("  \"player\": {\n");
		sb.append("    \"score\": ").append(data.playerScore).append(",\n");
		if (data.playerPosition != null) {
			sb.append("    \"position\": {\"x\": ")
					.append(data.playerPosition.x).append(", \"y\": ")
					.append(data.playerPosition.y).append("}\n");
		} else {
			sb.append("    \"position\": {\"x\": 0.0, \"y\": 0.0}\n");
		}
		sb.append("  },\n");
		sb.append("  \"enemies\": [\n");
		for (int i = 0; i < data.enemies.size(); i++) {
			EnemySnapshot enemy = data.enemies.get(i);
			sb.append("    {\n");
			sb.append("      \"state\": \"").append(escapeJson(enemy.state)).append("\",\n");
			sb.append("      \"active\": ").append(enemy.active).append(",\n");
			Vector2 pos = enemy.position != null ? enemy.position : new Vector2();
			sb.append("      \"position\": {\"x\": ").append(pos.x).append(", \"y\": ")
					.append(pos.y).append("},\n");
			Vector2 vel = enemy.velocity != null ? enemy.velocity : new Vector2();
			sb.append("      \"velocity\": {\"x\": ").append(vel.x).append(", \"y\": ")
					.append(vel.y).append("}\n");
			sb.append("    }");
			if (i < data.enemies.size() - 1) {
				sb.append(",");
			}
			sb.append("\n");
		}
		sb.append("  ]\n");
		sb.append("}\n");
		return sb.toString();
	}

	private String escapeJson(String input) {
		if (input == null || input.isEmpty()) {
			return "";
		}
		return input.replace("\\", "\\\\").replace("\"", "\\\"");
	}

	private static double extractNumber(String json, String key, double defaultValue) {
		if (json == null || json.isEmpty()) {
			return defaultValue;
		}
		Pattern pattern = Pattern.compile("\\\"" + Pattern.quote(key) + "\\\"\\s*:\\s*(-?\\d+(?:\\.\\d+)?)");
		Matcher matcher = pattern.matcher(json);
		if (matcher.find()) {
			try {
				return Double.parseDouble(matcher.group(1));
			} catch (NumberFormatException ignored) {
				return defaultValue;
			}
		}
		return defaultValue;
	}

	private static boolean extractBoolean(String json, String key, boolean defaultValue) {
		if (json == null || json.isEmpty()) {
			return defaultValue;
		}
		Pattern pattern = Pattern.compile("\\\"" + Pattern.quote(key) + "\\\"\\s*:\\s*(true|false)");
		Matcher matcher = pattern.matcher(json);
		if (matcher.find()) {
			return Boolean.parseBoolean(matcher.group(1));
		}
		return defaultValue;
	}

	private static String extractString(String json, String key, String defaultValue) {
		if (json == null || json.isEmpty()) {
			return defaultValue;
		}
		Pattern pattern = Pattern.compile("\\\"" + Pattern.quote(key) + "\\\"\\s*:\\s*\\\"(.*?)\\\"");
		Matcher matcher = pattern.matcher(json);
		if (matcher.find()) {
			return matcher.group(1);
		}
		return defaultValue;
	}

	private static String extractObject(String json, String key) {
		return extractSection(json, key, '{', '}');
	}

	private static String extractArray(String json, String key) {
		return extractSection(json, key, '[', ']');
	}

	private static String extractSection(String json, String key, char open, char close) {
		if (json == null || json.isEmpty()) {
			return "";
		}
		String token = "\"" + key + "\"";
		int keyIndex = json.indexOf(token);
		if (keyIndex < 0) {
			return "";
		}
		int start = json.indexOf(open, keyIndex);
		if (start < 0) {
			return "";
		}
		boolean inString = false;
		boolean escape = false;
		int depth = 0;
		int sectionStart = start + 1;
		for (int i = start + 1; i < json.length(); i++) {
			char c = json.charAt(i);
			if (escape) {
				escape = false;
				continue;
			}
			if (c == '\\') {
				escape = true;
				continue;
			}
			if (c == '"') {
				inString = !inString;
				continue;
			}
			if (inString) {
				continue;
			}
			if (c == open) {
				depth++;
			} else if (c == close) {
				if (depth == 0) {
					return json.substring(sectionStart, i);
				}
				depth--;
			}
		}
		return "";
	}

	private static List<String> splitObjects(String content) {
		List<String> result = new ArrayList<>();
		if (content == null || content.isEmpty()) {
			return result;
		}
		boolean inString = false;
		boolean escape = false;
		int depth = 0;
		int start = -1;
		for (int i = 0; i < content.length(); i++) {
			char c = content.charAt(i);
			if (escape) {
				escape = false;
				continue;
			}
			if (c == '\\') {
				escape = true;
				continue;
			}
			if (c == '"') {
				inString = !inString;
				continue;
			}
			if (inString) {
				continue;
			}
			if (c == '{') {
				if (depth == 0) {
					start = i + 1;
				}
				depth++;
			} else if (c == '}') {
				depth--;
				if (depth == 0 && start >= 0) {
					result.add(content.substring(start, i));
					start = -1;
				}
			}
		}
		return result;
	}

	public static class GameSaveData {
		public int width;
		public int height;
		public int playerScore;
		public Vector2 playerPosition;
		public List<EnemySnapshot> enemies = new ArrayList<>();
	}

	public static class EnemySnapshot {
		public Vector2 position = new Vector2();
		public Vector2 velocity = new Vector2();
		public boolean active = true;
		public String state = "active";
	}
}
