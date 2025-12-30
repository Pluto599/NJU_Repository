package com.gameengine.example;

import com.gameengine.core.GameEngine;
import com.gameengine.scene.OfflineGameScene;
import com.gameengine.scene.OnlineServerHostScene;

/**
 * J07 启动入口。
 *
 * - 无参数：进入开始界面（可选单机/联机服务器/退出）
 * - server：直接启动联机服务器并进入联机场景
 * - offline：直接进入单机菜单
 */
public class GameExample {
    public static void main(String[] args) {
        final int width = 1024;
        final int height = 768;

        GameEngine engine = new GameEngine(width, height, "J07");

        String mode = (args != null && args.length > 0) ? args[0] : "";
        if ("server".equalsIgnoreCase(mode)) {
            engine.setScene(new OnlineServerHostScene(engine, 7777));
        } else if ("offline".equalsIgnoreCase(mode)) {
            engine.setScene(new OfflineMenuScene(engine, width, height));
        } else if ("new".equalsIgnoreCase(mode)) {
            engine.setScene(new OfflineGameScene(engine, width, height, false, null));
        } else {
            engine.setScene(new LauncherScene(engine, width, height));
        }

        engine.run();
    }
}
