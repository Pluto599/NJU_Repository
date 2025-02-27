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
	// n ����ս���ܴ���
	// �Ѿ��� N ��ս��ʱ����ߵ� (N / m)ȡ���� ��ս���������͵�ս�����Ϊ x��
	//					  ����� r ��ս���������ߵ� k ��ս�����Ϊ y[1], ... , y[k] (���� r ���� 0 ����)
	// ��ǰս����Ϊ x * k + ��y
	// ��� q �μ�ʱ��ѯ�Ľ��

	//��ʼ��
	int n, m, r, k, q;
	cin >> n >> m >> r >> k >> q;

	int* combat_results = new int[n + 1];	//�洢����ս���Ľ��
	combat_results[0] = 0;
	for (int i = 1; i <= n; i++)
		cin >> combat_results[i];

	int* check_points = new int[q + 1];		//�洢��ѯ��ʱ��
	check_points[0] = 0;
	for (int i = 1; i <= q; i++)
		cin >> check_points[i];

	MaxHeap heap_1(n);	//���ڼ���λ�ڲ�ѯ��ʱ��ߵ� (N / m)ȡ���� ��ս�����
	MaxHeap heap_2(r);	//���ڼ�������� r ��ս���������ߵ� k ��ս�����

	for (int i = 1; i <= q; i++)
	{
		heap_2.clear();

		int top_index = (check_points[i] + m - 1) / m;
		int x, y = 0;
		int* poped_x = new int[top_index];

		// �� x
		for (int j = check_points[i - 1] + 1; j <= check_points[i]; j++)
			heap_1.push(combat_results[j]);
		// ���汻 pop ��ս�����
		for (int j = 0; j < top_index; j++)
		{
			heap_1.pop(x);
			poped_x[j] = x;
		}

		// �� y[1] ... y[k]
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

		// ��ӡ��ǰ����Ľ��
		cout << x * k + y << ' ';

		// �ָ��ѣ������� x ʱ�� pop ��ս����� push �� heap_1
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