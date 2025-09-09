package hulu;

import java.awt.Color;
import java.util.HashMap;


public class Hulu extends Creature {

    private int rank;

    private class Tuple2<A, B> {
        public A first;
        public B second;

        public Tuple2(A first, B second) {
            this.first = first;
            this.second = second;
        }
    }
    public enum Direction {
        UP, DOWN, LEFT, RIGHT,
        UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT
    }
    public HashMap <Direction, Tuple2<Integer, Integer>> directionMap = new HashMap<>() {{
        put(Direction.UP, new Tuple2<>(0, -1));
        put(Direction.DOWN, new Tuple2<>(0, 1));
        put(Direction.LEFT, new Tuple2<>(-1, 0));
        put(Direction.RIGHT, new Tuple2<>(1, 0));
        put(Direction.UP_LEFT, new Tuple2<>(-1, -1));
        put(Direction.UP_RIGHT, new Tuple2<>(1, -1));
        put(Direction.DOWN_LEFT, new Tuple2<>(-1, 1));
        put(Direction.DOWN_RIGHT, new Tuple2<>(1, 1));
        }
    };

    public Hulu(Color color, int rank, World world) {
        super(color, (char) 2, world);
        this.rank = rank;
    }

    public int getRank() {
        return this.rank;
    }

    @Override
    public String toString() {
        return String.valueOf(this.rank);
    }

    public int compareTo(Hulu o) {
        return this.getRank() - o.getRank();
    }

    // public void swap(Hulu another) {
    //     int x = this.getX();
    //     int y = this.getY();
    //     this.moveTo(another.getX(), another.getY());
    //     another.moveTo(x, y);
    // }
    public void swap(Hulu another, Runnable repaintCallback) {
        int stepLength = this.getX() - another.getX() + 2;
        
        final int DELAY = 200; // 每步延迟(毫秒)
        Thread animationThread = new Thread(() -> {
            try {
                for (int i = 0; i < stepLength; i++)
                {
                    Direction selfDir, anotherDir;
                    if (i == 0){
                        selfDir = Direction.DOWN;
                        anotherDir = Direction.UP;
                    }
                    else if (i == stepLength - 1) {
                        selfDir = Direction.UP;
                        anotherDir = Direction.DOWN;
                    }
                    else {
                        selfDir = Direction.LEFT;
                        anotherDir = Direction.RIGHT;
                    }

                    int selfPosX = tile.getxPos(), selfPosY = tile.getyPos();
                    int selfDirPosX = directionMap.get(selfDir).first;
                    int selfDirPosY = directionMap.get(selfDir).second;
                    // System.err.println(this.getColor() + " move from " + selfPosX + "," + selfPosY + " to " + (selfDirPosX + selfPosX) + "," + (selfDirPosY + selfPosY));
                    this.moveTo(selfDirPosX + selfPosX, selfDirPosY + selfPosY);

                    int anotherPosX = another.tile.getxPos(), anotherPosY = another.tile.getyPos();
                    int anotherDirPosX = directionMap.get(anotherDir).first;
                    int anotherDirPosY = directionMap.get(anotherDir).second;
                    // System.err.println(another.getColor() + " move from " + anotherPosX + "," + anotherPosY + " to " + (anotherDirPosX + anotherPosX) + "," + (anotherDirPosY + anotherPosY));
                    another.moveTo(anotherDirPosX + anotherPosX, anotherDirPosY + anotherPosY);

                    javax.swing.SwingUtilities.invokeLater(repaintCallback);

                    Thread.sleep(DELAY);
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        });

        animationThread.start();
    }

    @Override
    public void moveTo(int xPos, int yPos) {
        // 先从原位置移除
        world.put(new Grass(world), tile.getxPos(), tile.getyPos());
        super.moveTo(xPos, yPos);
        // 调用原有的放置方法
    }
}
