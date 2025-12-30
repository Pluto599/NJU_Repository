package com.gameengine.save;

import com.gameengine.math.Vector2;
import com.gameengine.testsupport.TestOutputExtension;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;
import org.junit.jupiter.api.extension.ExtendWith;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.attribute.FileTime;
import java.util.List;
import java.util.Optional;

import static org.junit.jupiter.api.Assertions.*;

@ExtendWith(TestOutputExtension.class)
class GameSaveServiceTest {

	@Test
	void saveAndLoadRoundTrip(@TempDir Path tempDir) throws IOException {
		Path saveFile = tempDir.resolve("save.json");
		GameSaveService svc = new GameSaveService(saveFile.toString());

		GameSaveService.GameSaveData data = new GameSaveService.GameSaveData();
		data.width = 1024;
		data.height = 768;
		data.playerScore = 42;
		data.playerPosition = new Vector2(1.5f, -2.25f);

		GameSaveService.EnemySnapshot e = new GameSaveService.EnemySnapshot();
		e.state = "active";
		e.active = true;
		e.position = new Vector2(10, 20);
		e.velocity = new Vector2(-1, 2);
		data.enemies.add(e);

		svc.save(data);
		assertTrue(Files.exists(saveFile));

		Optional<GameSaveService.GameSaveData> loadedOpt = svc.load();
		assertTrue(loadedOpt.isPresent());
		GameSaveService.GameSaveData loaded = loadedOpt.get();

		assertEquals(1024, loaded.width);
		assertEquals(768, loaded.height);
		assertEquals(42, loaded.playerScore);
		assertEquals(new Vector2(1.5f, -2.25f), loaded.playerPosition);
		assertEquals(1, loaded.enemies.size());
		assertEquals(new Vector2(10, 20), loaded.enemies.get(0).position);
		assertEquals(new Vector2(-1, 2), loaded.enemies.get(0).velocity);
		assertTrue(loaded.enemies.get(0).active);
		assertEquals("active", loaded.enemies.get(0).state);
	}

	@Test
	void listSaveFilesSortsByLastModifiedDesc(@TempDir Path tempDir) throws IOException {
		Path a = tempDir.resolve("a.json");
		Path b = tempDir.resolve("b.json");
		Files.writeString(a, "{}\n");
		Files.writeString(b, "{}\n");

		Files.setLastModifiedTime(a, FileTime.fromMillis(1000));
		Files.setLastModifiedTime(b, FileTime.fromMillis(2000));

		List<Path> files = GameSaveService.listSaveFiles(tempDir.toString());
		assertEquals(2, files.size());
		assertEquals(b.getFileName().toString(), files.get(0).getFileName().toString());
		assertEquals(a.getFileName().toString(), files.get(1).getFileName().toString());
	}
}
