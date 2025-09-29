package backup;

import hulu.Hulu;
import hulu.Sorter;

public class BubbleSorter implements Sorter {

    private Hulu[] a;

    @Override
    public void load(Hulu[] a) {
        this.a = a;
    }

    private void swap(int i, int j) {
        Hulu temp;
        temp = a[i];
        a[i] = a[j];
        a[j] = temp;
        plan += "" + a[i] + "<->" + a[j] + "\n";
    }

    private String plan = "";

    @Override
    public void sort() {
        boolean sorted = false;
        while (!sorted) {
            sorted = true;
            for (int i = 0; i < a.length - 1; i++) {
                if (a[i].compareTo(a[i + 1]) > 0) {
                    swap(i, i + 1);
                    sorted = false;
                }
            }
        }
    }

    @Override
    public String getPlan() {
        return this.plan;
    }

}