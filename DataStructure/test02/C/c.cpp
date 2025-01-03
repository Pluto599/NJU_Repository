#include<iostream>
using namespace std;
#include<queue>

template <class T>
struct BinTreeNode {
	T data;
	BinTreeNode<T>* leftChild, * rightChild, * parent;

	BinTreeNode()
	{
		leftChild = nullptr;  rightChild = nullptr; parent = nullptr;
	}
	BinTreeNode(T x, BinTreeNode<T>* l = nullptr, BinTreeNode<T>* r = nullptr, BinTreeNode<T>* p = nullptr)
	{
		data = x;  leftChild = l;  rightChild = r; parent = p;
	}
};

template <class T>
class BinaryTree
{
public:
	BinaryTree() :root(nullptr) {}
	BinaryTree(T value) :RefValue(value), root(nullptr) {}
	~BinaryTree() { Destroy(root); }

	void CreateBinTree(int len) 
	{
		CreateBinTree(len, root); 
		root->parent = nullptr;
		setParent(root);
	}

	void Solution() {
		ans = 0;
		if (lamp(root) == 0)
			ans++;
		cout << ans;
	}

	int lamp(BinTreeNode<T>* p) {
		if (!p) return 1;

		int left = lamp(p->leftChild);
		int right = lamp(p->rightChild);

		if (left == 0 || right == 0) {
			ans++;
			return 2;
		}

		if (left == 2 || right == 2)
			return 1;

		return 0;
	}


protected:
	BinTreeNode<T>* root;
	T RefValue;
	void CreateBinTree(int len, BinTreeNode<T>*& subTree);
	void Destroy(BinTreeNode<T>*& subTree);
	void setParent(BinTreeNode<T>*& subTree)
	{
		if (subTree != nullptr)
		{
			if (subTree->leftChild)
			{
				subTree->leftChild->parent = subTree;
				setParent(subTree->leftChild);
			}
			if (subTree->rightChild)
			{
				subTree->rightChild->parent = subTree;
				setParent(subTree->rightChild);
			}
			
		}
	}

	int ans = 0;

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





int main()
{
	int n;
	cin >> n;
	BinaryTree<int>* tree = new BinaryTree<int>(0);
	tree->CreateBinTree(n);

	tree->Solution();

	return 0;
}