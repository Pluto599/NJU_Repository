package com.gameengine.example;

import com.gameengine.core.GameEngine;
import com.gameengine.net.NioServer;
import com.gameengine.scene.OnlineGameScene;

/**
 * 服务器入口：启动 NIO server 并直接进入联机场景。
 */
public class GameExample {
    private static final int DEFAULT_PORT = 7777;

    public static void main(String[] args) {
        final int width = 1024;
        final int height = 768;

        System.out.println("[Server] Starting on port " + DEFAULT_PORT + "...");
        NioServer server = new NioServer(DEFAULT_PORT);
        server.start();
        Runtime.getRuntime().addShutdownHook(new Thread(server::stop, "nio-server-shutdown"));

        GameEngine engine = new GameEngine(width, height, "J06 Online Server");
        engine.setScene(new OnlineGameScene(engine, OnlineGameScene.Mode.SERVER));
        engine.run();
    }
}
