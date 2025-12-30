package com.gameengine.example;

import com.gameengine.core.GameEngine;
import com.gameengine.net.NioClient;
import com.gameengine.scene.OnlineGameScene;

import java.util.concurrent.TimeUnit;

/**
 * 客户端入口：连接远端 Server 并渲染插值后的状态。
 */
public class ClientLauncher {
    private static final int DEFAULT_PORT = 7777;

    public static void main(String[] args) {
        String host = (args != null && args.length > 0) ? args[0] : "127.0.0.1";
        final int width = 1024;
        final int height = 768;

        GameEngine engine = new GameEngine(width, height, "J06 Online Client");
        final int maxAttempts = 10;
        boolean connected = false;
        NioClient client = null;

        for (int attempt = 1; attempt <= maxAttempts; attempt++) {
            client = new NioClient();
            if (client.connect(host, DEFAULT_PORT) && client.join("Player2")) {
                connected = true;
                break;
            }
            client.close();
            System.err.println("[Client] Connect attempt " + attempt + " failed, retrying...");
            try {
                TimeUnit.MILLISECONDS.sleep(600);
            } catch (InterruptedException ignored) {
            }
        }

        if (connected && client != null) {
            client.startInputLoop(engine.getInputManager());
            client.startReceiveLoop();
            System.out.println("[Client] Connected to " + host + ":" + DEFAULT_PORT);
        } else {
            System.err.println("[Client] Failed to connect to " + host + ":" + DEFAULT_PORT);
            if (client != null) {
                client.close();
            }
        }

        engine.setScene(new OnlineGameScene(engine, OnlineGameScene.Mode.CLIENT));
        engine.run();
    }
}
