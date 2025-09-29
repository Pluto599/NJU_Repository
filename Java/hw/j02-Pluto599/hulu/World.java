package hulu;

import java.awt.Color;
import java.net.URL;

import classloader.SteganographyClassLoader;

@SuppressWarnings({ "deprecation", "rawtypes" })
public class World {

    public static final int WIDTH = 22;
    public static final int HEIGHT = 7;

    private Tile[][] tiles;

    private Hulu[] bros;

    private String[] sortSteps;

    public World() throws Exception {

        tiles = new Tile[WIDTH][HEIGHT];

        for (int i = 0; i < WIDTH; i++) {
            for (int j = 0; j < HEIGHT; j++) {
                tiles[i][j] = new Tile(i, j);
            }
        }

        for (int i = 0; i < WIDTH; i++) {
            for (int j = 0; j < HEIGHT; j++) {
                this.put(new Grass(this), i, j);
            }
        }

        bros = new Hulu[7];

        bros[3] = new Hulu(new Color(204, 0, 0), 1, this);
        bros[5] = new Hulu(new Color(255, 165, 0), 2, this);
        bros[1] = new Hulu(new Color(252, 233, 79), 3, this);
        bros[0] = new Hulu(new Color(78, 154, 6), 4, this);
        bros[4] = new Hulu(new Color(50, 175, 255), 5, this);
        bros[6] = new Hulu(new Color(114, 159, 207), 6, this);
        bros[2] = new Hulu(new Color(173, 127, 168), 7, this);

        this.put(bros[0], 4, 3);
        this.put(bros[1], 6, 3);
        this.put(bros[2], 8, 3);
        this.put(bros[3], 10, 3);
        this.put(bros[4], 12, 3);
        this.put(bros[5], 14, 3);
        this.put(bros[6], 16, 3);

        // SteganographyClassLoader loader = new SteganographyClassLoader(
        //         new URL("https://i.postimg.cc/50vcc4fk/hulu-Insert-Sorter.png"));
        URL imgUrl = World.class.getClassLoader().getResource("hulu.InsertSorter.png");
        SteganographyClassLoader loader = new SteganographyClassLoader(imgUrl);

        Class c = loader.loadClass("hulu.InsertSorter");

        Sorter sorter = (Sorter) c.newInstance();

        sorter.load(bros);
        sorter.sort();

        sortSteps = this.parsePlan(sorter.getPlan());
    }

    private String[] parsePlan(String plan) {
        return plan.split("\n");
    }

    public void next(int step) {
        String[] couple = this.sortSteps[step].split("<->");
        getBroByRank(bros, Integer.parseInt(couple[0])).swap(getBroByRank(bros, Integer.parseInt(couple[1])));
    }

    private Hulu getBroByRank(Hulu[] bros, int rank) {
        for (Hulu bro : bros) {
            if (bro.getRank() == rank) {
                return bro;
            }
        }
        return null;
    }

    public String[] steps() {
        return this.sortSteps;
    }

    public Hulu[] getBros() {
        return this.bros;
    }

    public Thing get(int x, int y) {
        return this.tiles[x][y].getThing();
    }

    public void put(Thing t, int x, int y) {
        this.tiles[x][y].setThing(t);
    }

}
