#include<iostream>
using namespace std;
#include<cassert>

const int Defaultsize = 26;

template<class T>
class MinHeap
{
public:
	MinHeap(int sz = Defaultsize);
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
	maxHeapSize = (Defaultsize < sz) ? sz : Defaultsize;
	heap = new T[maxHeapSize];

	if (heap == nullptr)
	{
		cerr << "¶Ñ´æ´¢·ÖÅäÊ§°Ü" << endl;
		return;
	}

	currentSize = 0;
}

template<class T>
MinHeap<T>::MinHeap(T arr[], int n)
{
	maxHeapSize = (Defaultsize < n) ? n : Defaultsize;
	heap = new T[maxHeapSize];

	if (heap == nullptr)
	{
		cerr << "¶Ñ´æ´¢·ÖÅäÊ§°Ü£¡" << endl;
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
		cerr << "Heap full£¡" << endl;
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

template<class T>
struct HuffmanNode
{
	T data;
	HuffmanNode<T>* parent;
	HuffmanNode<T>* leftChild, * rightChild;

	HuffmanNode() :parent(nullptr), leftChild(nullptr), rightChild(nullptr) {}
	HuffmanNode(T elem, HuffmanNode<T>* pr = nullptr, HuffmanNode<T>* left = nullptr, HuffmanNode<T>* right = nullptr)
		:data(elem), parent(pr), leftChild(left), rightChild(right) {}

	bool operator <=(HuffmanNode& R) { return data <= R.data; }
	bool operator >(HuffmanNode& R) { return data > R.data; }
};

template<class T>
class HuffmanTree
{
public:
	HuffmanTree(T w[], int n);
	~HuffmanTree() { deleteTree(root); }

	void getWPL() { cout << WPL; }

protected:
	HuffmanNode<T>* root;
	void deleteTree(HuffmanNode<T>* t);
	void mergeTree(HuffmanNode<T>& ht1, HuffmanNode<T>& ht2, HuffmanNode<T>*& parent);

	int WPL = 0;
};

template<class T>
HuffmanTree<T>::HuffmanTree(T w[], int n)
{
	MinHeap<HuffmanNode<T>> hp;
	HuffmanNode<T>* parent = nullptr, first, second;
	HuffmanNode<T>* NodeList = new HuffmanNode<T>[n];
	for (int i = 0; i < n; i++)
	{
		NodeList[i].data = w[i];
		NodeList[i].leftChild = nullptr;
		NodeList[i].rightChild = nullptr;
		NodeList[i].parent = nullptr;
		hp.push(NodeList[i]);
	}
	for (int i = 0; i < n - 1; i++)
	{
		hp.pop(first);
		hp.pop(second);
		mergeTree(first, second, parent);
		hp.push(*parent);
	}
	root = parent;
}

template<class T>
void HuffmanTree<T>::deleteTree(HuffmanNode<T>* t)
{
	if (t == nullptr)
		return;

	deleteTree(t->leftChild);
	deleteTree(t->rightChild);
	delete t;
	t = nullptr;
}

template<class T>
void HuffmanTree<T>::mergeTree(HuffmanNode<T>& ht1, HuffmanNode<T>& ht2, HuffmanNode<T>*& parent)
{
	parent = new HuffmanNode<T>;
	parent->leftChild = &ht1;
	parent->rightChild = &ht2;
	parent->data = ht1.data + ht2.data;
	ht1.parent = ht2.parent = parent;

	WPL += parent->data;
}


int main()
{
	string str;
	cin >> str;
	int letter_num[26] = { 0 };
	for (int i = 0; i < str.size(); i++)
		letter_num[str[i] - 'a']++;

	int n = 0;
	int node[26] = { 0 };
	for (int i = 0; i < 26; i++)
		if (letter_num[i] > 0)
		{
			node[n] = letter_num[i];
			n++;
		}

	HuffmanTree<int>* tree = new HuffmanTree<int>(node, n);
	tree->getWPL();

	return 0;
}