package com.gameengine.example;

import com.gameengine.components.*;
import com.gameengine.core.GameObject;
import com.gameengine.core.GameEngine;
import com.gameengine.core.GameLogic;
import com.gameengine.graphics.Renderer;
import com.gameengine.math.Vector2;
import com.gameengine.objects.*;
import com.gameengine.scene.Scene;

import java.util.List;
import java.util.Random;

/**
 * 游戏示例
 */
public class GameExample {
    public static void main(String[] args) {
        System.out.println("启动游戏引擎...");
        
        try {
            // 创建游戏引擎
            GameEngine engine = new GameEngine(800, 600, "游戏引擎");
            
            // 创建游戏场景
            Scene gameScene = new Scene("GameScene") {
                public Renderer renderer;
                private Random random;
                private float time;
                private GameLogic gameLogic;
                
                @Override
                public void initialize() {
                    super.initialize();
                    this.renderer = engine.getRenderer();
                    this.random = new Random();
                    this.time = 0;
                    this.gameLogic = new GameLogic(this);
                    
                    // 创建游戏对象
                    createPlayer();
                    createEnemies();
                    createDecorations();
                }
                
                @Override
                public void update(float deltaTime) {
                    super.update(deltaTime);
                    time += deltaTime;
                    
                    // 使用游戏逻辑类处理游戏规则
                    gameLogic.handlePlayerInput();
                    gameLogic.updatePhysics();
                    gameLogic.checkCollisions();
                    
                    // 生成新敌人
                    if (time > 2.0f) {
                        createEnemy();
                        time = 0;
                    }
                }
                
                @Override
                public void render() {
                    // 绘制背景
                    renderer.drawRect(0, 0, 800, 600, 0.1f, 0.1f, 0.2f, 1.0f);

                    // 操作提示
                    renderer.drawText("WASD/上下左右 移动", 10, 25, 15f, 1f, 1f, 1f, 1f);
                    renderer.drawText("IJKL 射击",  10, 50, 15f, 1f, 1f, 1f, 1f);

                    // 显示分数
                    Player player = Player.getInstance();
                    if (player != null) {
                        renderer.drawText("分数：" + player.getScore(), 680, 30, 20, 1.0f, 1.0f, 1.0f, 1.0f);
                    }
                    
                    // 渲染所有对象
                    super.render();
                }
                
                private void createPlayer() {
                    // 创建葫芦娃 - 所有部位都在一个GameObject中
                    Player player = Player.getInstance("Player", this, renderer);
                    addGameObject(player);
                }
                
                private void createEnemies() {
                    for (int i = 0; i < 3; i++) {
                        createEnemy();
                    }
                }
                
                private void createEnemy() {
                    Enemy enemy = new Enemy("Enemy", renderer);
                    addGameObject(enemy);
                }

                private void createDecorations() {
                    for (int i = 0; i < 5; i++) {
                        createDecoration();
                    }
                }
                
                private void createDecoration() {
                    GameObject decoration = new GameObject("Decoration") {
                        @Override
                        public void update(float deltaTime) {
                            super.update(deltaTime);
                            updateComponents(deltaTime);
                        }

                        @Override
                        public void render() {
                            renderComponents();
                        }
                    };

                    // 随机位置
                    Vector2 position = new Vector2(
                            random.nextFloat() * 800,
                            random.nextFloat() * 600);

                    // 添加变换组件
                    TransformComponent transform = decoration.addComponent(new TransformComponent(position));

                    // 添加渲染组件
                    RenderComponent render = decoration.addComponent(new RenderComponent(
                            RenderComponent.RenderType.CIRCLE,
                            new Vector2(5, 5),
                            new RenderComponent.Color(0.5f, 0.5f, 1.0f, 0.8f)));
                    render.setRenderer(renderer);

                    addGameObject(decoration);
                }
            };
            
            // 设置场景
            engine.setScene(gameScene);
            
            // 运行游戏
            engine.run();
            
        } catch (Exception e) {
            System.err.println("游戏运行出错: " + e.getMessage());
            e.printStackTrace();
        }
        
        System.out.println("游戏结束");
    }
}
