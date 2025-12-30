package com.gameengine.scene;

import com.gameengine.core.GameEngine;
import com.gameengine.net.NioServer;

/**
 * 在线 Server 场景包装：负责启动/停止 NioServer。
 */
public class OnlineServerHostScene extends OnlineGameScene {
    private final NioServer server;

    public OnlineServerHostScene(GameEngine engine, int port) {
        super(engine, Mode.SERVER);
        this.server = new NioServer(port);
        this.server.start();
        Runtime.getRuntime().addShutdownHook(new Thread(server::stop, "nio-server-shutdown"));
    }

    @Override
    public void onExit() {
        try {
            server.stop();
        } catch (Exception ignored) {
        }
        super.onExit();
    }
}
