package hulu;

public class InsertSorter implements Sorter{
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
		int min = -1;
		for (int i = 0; i < a.length - 1; i++)
		{
			min = i;
			for (int j = i + 1; j < a.length; j++) {
				if (a[j].compareTo(a[min]) < 0) {
					min = j;
				}
			}
			swap(i, min);
		}
	}

	@Override
	public String getPlan() {
		return this.plan;
	}
}
