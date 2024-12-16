#include<iostream>
using namespace std;
#include<cassert>

const int DefaultSize = 128;

template<class T>
class MinHeap
{
public:
	MinHeap(int sz = DefaultSize);
	MinHeap(T arr[], int n);
	~MinHeap() { delete[]heap; }
	bool push(T& x);
	bool pop();
	bool pop(T& x);
	bool empty() const { return currentSize == 0; }
	bool full() const { return currentSize == maxHeapSize; }
	void clear() { currentSize = 0; }

private:
	T* heap;
	int currentSize;
	int maxHeapSize;
	void siftDown(int start, int m);
	void siftUp(int start);

};

template<class T>
MinHeap<T>::MinHeap(int sz)
{
	maxHeapSize = (DefaultSize < sz) ? sz : DefaultSize;
	heap = new T[maxHeapSize];

	if (heap == nullptr)
	{
		cerr << "Heap storage allocation failed!" << endl;
		return;
	}

	currentSize = 0;
}

template<class T>
MinHeap<T>::MinHeap(T arr[], int n)
{
	maxHeapSize = (DefaultSize < n) ? n : DefaultSize;
	heap = new T[maxHeapSize];

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

template<class T>
bool MinHeap<T>::push(T& x)
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

template<class T>
bool MinHeap<T>::pop()
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

template<class T>
bool MinHeap<T>::pop(T& x)
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

template<class T>
void MinHeap<T>::siftDown(int start, int m)
{
	int i = start;
	int j = 2 * i + 1;
	T temp = heap[i];
	while (j <= m)
	{
		if (j < m && heap[j] > heap[j + 1])
			j++;
		if (temp <= heap[j])
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

template<class T>
void MinHeap<T>::siftUp(int start)
{
	int j = start;
	int i = (j - 1) / 2;
	T temp = heap[j];
	while (j > 0)
	{
		if (heap[i] <= temp)
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




int main()
{
	int n, hp = 1, cnt = 0;
	cin >> n;
	int* nums = new int[n * 2];
	int amt = 0;
	for (int i = 0; i < n; i++)
	{
		cin >> nums[i];
		amt += nums[i];
	}
	if (amt < 0)
	{
		cout << -1;
		return 0;
	}

	MinHeap<int> enemyRooms(n);

	int tail = n;
	for (int i = 0; i < tail; i++)
	{
		if (nums[i] < 0)
			enemyRooms.push(nums[i]);

		hp += nums[i];
		while(hp <= 0)
		{
			int min;
			enemyRooms.pop(min);
			hp -= min;
			nums[tail] = min;
			tail++;
			cnt++;
		}
	}
	cout << cnt;

	return 0;
}