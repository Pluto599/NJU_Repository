//#include<iostream>
//using namespace std;
//#include<cassert>
//
//const int DefaultSize = 128;
//
//template<class T>
//class MaxHeap
//{
//public:
//	MaxHeap(int sz = DefaultSize);
//	MaxHeap(T arr[], int n);
//	MaxHeap(const MaxHeap<T>& h);
//	~MaxHeap() { delete[]heap; }
//	T top() { return heap[0]; }
//	bool push(T& x);
//	bool pop();
//	bool pop(T& x);
//	bool empty() const { return currentSize == 0; }
//	bool full() const { return currentSize == maxHeapSize; }
//	void clear() { currentSize = 0; }
//
//private:
//	T* heap;
//	int currentSize;
//	int maxHeapSize;
//	void siftDown(int start, int m);
//	void siftUp(int start);
//
//};
//
//template<class T>
//MaxHeap<T>::MaxHeap(int sz)
//{
//	maxHeapSize = (DefaultSize < sz) ? sz : DefaultSize;
//	heap = new T[maxHeapSize];
//
//	if (heap == nullptr)
//	{
//		cerr << "Heap storage allocation failed!" << endl;
//		return;
//	}
//
//	currentSize = 0;
//}
//
//template<class T>
//MaxHeap<T>::MaxHeap(T arr[], int n)
//{
//	maxHeapSize = (DefaultSize < n) ? n : DefaultSize;
//	heap = new T[maxHeapSize];
//
//	if (heap == nullptr)
//	{
//		cerr << "Heap storage allocation failed!" << endl;
//		return;
//	}
//
//	for (int i = 0; i < n; i++)
//		heap[i] = arr[i];
//	currentSize = n;
//
//	int currentPos = (currentSize - 2) / 2;
//	while (currentPos >= 0)
//	{
//		siftDown(currentPos, currentSize - 1);
//		currentPos--;
//	}
//}
//
//template<class T>
//MaxHeap<T>::MaxHeap(const MaxHeap<T>& h)
//{
//	maxHeapSize = h.maxHeapSize;
//	currentSize = h.currentSize;
//	heap = new T[maxHeapSize];
//	for (int i = 0; i < currentSize; ++i)
//	{
//		heap[i] = h.heap[i];
//	}
//}
//
//template<class T>
//bool MaxHeap<T>::push(T& x)
//{
//	if (currentSize == maxHeapSize)
//	{
//		cerr << "Heap full!" << endl;
//		return false;
//	}
//
//	heap[currentSize] = x;
//	siftUp(currentSize);
//	currentSize++;
//	return true;
//}
//
//template<class T>
//bool MaxHeap<T>::pop()
//{
//	if (empty())
//	{
//		cout << "Heap empty!" << endl;
//		return false;
//	}
//
//	heap[0] = heap[currentSize - 1];
//	currentSize--;
//	siftDown(0, currentSize - 1);
//	return true;
//}
//
//template<class T>
//bool MaxHeap<T>::pop(T& x)
//{
//	if (empty())
//	{
//		cout << "Heap empty!" << endl;
//		return false;
//	}
//
//	x = heap[0];
//	heap[0] = heap[currentSize - 1];
//	currentSize--;
//	siftDown(0, currentSize - 1);
//	return true;
//}
//
//template<class T>
//void MaxHeap<T>::siftDown(int start, int m)
//{
//	int i = start;
//	int j = 2 * i + 1;
//	T temp = heap[i];
//	while (j <= m)
//	{
//		if (j < m && heap[j] < heap[j + 1])
//			j++;
//		if (temp >= heap[j])
//			break;
//		else
//		{
//			heap[i] = heap[j];
//			i = j;
//			j = 2 * j + 1;
//		}
//	}
//	heap[i] = temp;
//}
//
//template<class T>
//void MaxHeap<T>::siftUp(int start)
//{
//	int j = start;
//	int i = (j - 1) / 2;
//	T temp = heap[j];
//	while (j > 0)
//	{
//		if (heap[i] >= temp)
//			break;
//		else
//		{
//			heap[j] = heap[i];
//			j = i;
//			i = (i - 1) / 2;
//		}
//	}
//	heap[j] = temp;
//}
//
//int main()
//{
//	long long m, n, k;
//	cin >> m >> n >> k;
//	long long* initialReelImpacts = new long long[m];
//	for (long long i = 0; i < m; i++)
//		cin >> initialReelImpacts[i];
//	long long* newReelImpacts = new long long[n];
//	for (long long i = 0; i < n; i++)
//		cin >> newReelImpacts[i];
//
//	MaxHeap<long long> heap;
//	for (long long i = 0; i < m; i++)
//		heap.push(initialReelImpacts[i]);
//
//	long long ans = 0;
//	MaxHeap<long long> temp(heap);
//	for (long long i = 1; i < k; i++)
//		temp.pop();
//	ans += temp.top();
//	for (long long i = 0; i < n; i++)
//	{
//		heap.push(newReelImpacts[i]);
//		MaxHeap<long long> temp(heap);
//		for (long long i = 1; i < k; i++)
//			temp.pop();
//		ans += temp.top();
//	}
//	cout << ans;
//
//	return 0;
//}