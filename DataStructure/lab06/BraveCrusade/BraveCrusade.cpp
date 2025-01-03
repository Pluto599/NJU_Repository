#include<iostream>
using namespace std;
#include<queue>

template <class T>
struct BinTreeNode {
	T data;
	BinTreeNode<T>* leftChild, * rightChild;

	BinTreeNode()
	{
		leftChild = nullptr;  rightChild = nullptr;
	}
	BinTreeNode(T x, BinTreeNode<T>* l = nullptr, BinTreeNode<T>* r = nullptr)
	{
		data = x;  leftChild = l;  rightChild = r;
	}
};

template <class T>
class BinaryTree
{
public:
	BinaryTree() :root(nullptr) {}
	BinaryTree(T value) :RefValue(value), root(nullptr) {}
	~BinaryTree() { Destroy(root); }

	void CreateBinTree(int len) { CreateBinTree(len, root); }

	void Solution()
	{
		//ans = 0;

		// recursion: this node Y/N -- child N/Y/N
		// how to get res ?
		// 

		 val = 0;
		 crusade(root, 0);

		 val = 0;
		 crusade(root, 1);

		cout << ans;
	}

protected:
	BinTreeNode<T>* root;
	T RefValue;

	long long ans;
	long long val;

	void CreateBinTree(int len, BinTreeNode<T>*& subTree);
	void Destroy(BinTreeNode<T>*& subTree);

	//void crusade(BinTreeNode<int>* t, bool b);
	//long long crusade(BinTreeNode<int>* t);
	void crusade(BinTreeNode<int>* t, bool b);

};

template<class T>
void BinaryTree<T>::CreateBinTree(int len, BinTreeNode<T>*& subTree)
{
	if (len <= 0)
		return;

	T item;
	cin >> item;
	subTree = new BinTreeNode<T>(item);

	queue<BinTreeNode<T>*> q;
	BinTreeNode<T>* p;
	q.push(subTree);

	len--;

	while (len > 0)
	{
		p = q.front();
		q.pop();

		cin >> item;

		if (item != RefValue)
		{
			p->leftChild = new BinTreeNode<T>(item);
			q.push(p->leftChild);
		}
		else
			p->leftChild = nullptr;

		cin >> item;
		if (item != RefValue)
		{
			p->rightChild = new BinTreeNode<T>(item);
			q.push(p->rightChild);
		}
		else
			p->rightChild = nullptr;

		len -= 2;
	}
}

template<class T>
void BinaryTree<T>::Destroy(BinTreeNode<T>*& subTree)
{
	if (subTree != nullptr)
	{
		Destroy(subTree->leftChild);
		Destroy(subTree->rightChild);
		delete subTree;
		subTree = nullptr;
	}
}

template<class T>
void BinaryTree<T>::crusade(BinTreeNode<int>* t, bool b)
{
	ans = ans > val ? ans : val;

	if (t == nullptr)
		return;

	if (b)
	{
		val += t->data;
		crusade(t->leftChild, 0);
		crusade(t->rightChild, 0);
	}
	else
	{
		int curV = val;
		crusade(t->leftChild, 1);
		int newV = val;
		val = curV;
		crusade(t->leftChild, 0);
		val = val > newV ? val : newV;		
		
		curV = val;
		crusade(t->rightChild, 1);
		newV = val;
		val = curV;
		crusade(t->rightChild, 0);
		val = val > newV ? val : newV;
	}
}

int main()
{
	int n;
	cin >> n;
	BinaryTree<int>* tree = new BinaryTree<int>(-1);
	tree->CreateBinTree(n);

	tree->Solution();

	return 0;
}