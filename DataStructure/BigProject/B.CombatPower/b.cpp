#include<iostream>
using namespace std;

const int DefaultSize = 128;

class MaxHeap
{
public:
	MaxHeap(int sz = DefaultSize);
	MaxHeap(int arr[], int n);
	~MaxHeap() { delete[]heap; }

	// MaxHeap& operator=(const MaxHeap& hp);

	int size() { return currentSize; }
	int top() { return heap[0]; }
	bool push(int& x);
	bool pop();
	bool pop(int& x);
	bool empty() const { return currentSize == 0; }
	bool full() const { return currentSize == maxHeapSize; }
	void clear() { currentSize = 0; }

private:
	int* heap;
	int currentSize;
	int maxHeapSize;
	void siftDown(int start, int m);
	void siftUp(int start);

};




int main()
{
	// n 代表战斗总次数
	// 已经过 N 次战斗时，最高的 (N / m)取上整 个战斗结果中最低的战斗结果为 x，
	//					  最近的 r 个战斗结果中最高的 k 个战斗结果为 y[1], ... , y[k] (不足 r 个用 0 补齐)
	// 当前战斗力为 x * k + ∑y
	// 输出 q 次即时查询的结果

	//初始化
	int n, m, r, k, q;
	cin >> n >> m >> r >> k >> q;

	int* combat_results = new int[n + 1];	//存储所有战斗的结果
	combat_results[0] = 0;
	for (int i = 1; i <= n; i++)
		cin >> combat_results[i];

	int* check_points = new int[q + 1];		//存储查询的时刻
	check_points[0] = 0;
	for (int i = 1; i <= q; i++)
		cin >> check_points[i];

	MaxHeap heap_1(n);	//用于计算位于查询点时最高的 (N / m)取上整 个战斗结果
	MaxHeap heap_2(r);	//用于计算最近的 r 个战斗结果中最高的 k 个战斗结果

	for (int i = 1; i <= q; i++)
	{
		heap_2.clear();

		int top_index = (check_points[i] + m - 1) / m;
		int x, y = 0;
		int* poped_x = new int[top_index];

		// 求 x
		for (int j = check_points[i - 1] + 1; j <= check_points[i]; j++)
			heap_1.push(combat_results[j]);
		// 保存被 pop 的战斗结果
		for (int j = 0; j < top_index; j++)
		{
			heap_1.pop(x);
			poped_x[j] = x;
		}

		// 求 y[1] ... y[k]
		for (int j = 0; j < r; j++)
		{
			int index = check_points[i] - j;
			if (index <= 0)
				break;

			heap_2.push(combat_results[index]);
		}

		for (int j = 0; j < min(k, check_points[i]); j++)
		{
			y += heap_2.top();
			heap_2.pop();
		}

		// 打印当前检查点的结果
		cout << x * k + y << ' ';

		// 恢复堆，将计算 x 时被 pop 的战斗结果 push 回 heap_1
		for (int j = 0; j < top_index; j++)
			heap_1.push(poped_x[j]);
	}

	return 0;
}



MaxHeap::MaxHeap(int sz)
{
	maxHeapSize = (DefaultSize < sz) ? sz : DefaultSize;
	heap = new int[maxHeapSize];

	if (heap == nullptr)
	{
		cerr << "Heap storage allocation failed!" << endl;
		return;
	}

	currentSize = 0;
}

MaxHeap::MaxHeap(int arr[], int n)
{
	maxHeapSize = (DefaultSize < n) ? n : DefaultSize;
	heap = new int[maxHeapSize];

	if (heap == nullptr)
	{
		cerr << "Heap storage allocation failed!" << endl;
		return;
	}

	for (int i = 0; i < n; i++)
		heap[i] = arr[i];
	currentSize = n;

	int currentPos = (currentSize - 2) / 2;
	while (currentPos >= 0)
	{
		siftDown(currentPos, currentSize - 1);
		currentPos--;
	}
}

bool MaxHeap::push(int& x)
{
	if (currentSize == maxHeapSize)
	{
		cerr << "Heap full!" << endl;
		return false;
	}

	heap[currentSize] = x;
	siftUp(currentSize);
	currentSize++;
	return true;
}

bool MaxHeap::pop()
{
	if (empty())
	{
		cout << "Heap empty!" << endl;
		return false;
	}

	heap[0] = heap[currentSize - 1];
	currentSize--;
	siftDown(0, currentSize - 1);
	return true;
}

bool MaxHeap::pop(int& x)
{
	if (empty())
	{
		cout << "Heap empty!" << endl;
		return false;
	}

	x = heap[0];
	heap[0] = heap[currentSize - 1];
	currentSize--;
	siftDown(0, currentSize - 1);
	return true;
}

void MaxHeap::siftDown(int start, int m)
{
	int i = start;
	int j = 2 * i + 1;
	int temp = heap[i];
	while (j <= m)
	{
		if (j < m && heap[j] < heap[j + 1])
			j++;
		if (temp >= heap[j])
			break;
		else
		{
			heap[i] = heap[j];
			i = j;
			j = 2 * j + 1;
		}
	}
	heap[i] = temp;
}

void MaxHeap::siftUp(int start)
{
	int j = start;
	int i = (j - 1) / 2;
	int temp = heap[j];
	while (j > 0)
	{
		if (heap[i] >= temp)
			break;
		else
		{
			heap[j] = heap[i];
			j = i;
			i = (i - 1) / 2;
		}
	}
	heap[j] = temp;
}