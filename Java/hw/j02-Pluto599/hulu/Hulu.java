package hulu;

import java.awt.Color;

public class Hulu extends Creature {

    private int rank;

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


    public void swap(Hulu another) {
        int x = this.getX();
        int y = this.getY();
        this.moveTo(another.getX(), another.getY());
        another.moveTo(x, y);
    }

}
