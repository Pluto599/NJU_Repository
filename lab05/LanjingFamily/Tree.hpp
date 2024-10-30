#include<iostream>
using namespace std;
#include<cassert>
#include<queue>

template <class T>
struct BinTreeNode {	                  
	T data;	 		                           
	BinTreeNode<T>* leftChild, * rightChild;	

	BinTreeNode() { leftChild = nullptr;  rightChild = nullptr; }
	BinTreeNode(T x, BinTreeNode<T>* l = nullptr, BinTreeNode<T>* r = nullptr) { data = x;  leftChild = l;  rightChild = r; }
};

template <class T>
class BinaryTree
{
public:
	BinaryTree() :root(nullptr) {}	
	BinaryTree(T item, BinTreeNode<T>* lch, BinTreeNode<T>* rch) { root = new BinTreeNode<T>(item, lch, rch); }
	BinaryTree(T value) :RefValue(value), root(nullptr) {}
	~BinaryTree() { Destroy(root); }

	void CreateBinTree(int len) { CreateBinTree(len, root); }	
	void CreateBinTree(T* VLR, T* LVR, int n) { root = CreateBinTree(VLR, LVR, n, nullptr); }
						
	bool IsEmpty() { return root == nullptr; }		
	BinTreeNode<T>* Parent(BinTreeNode<T>* t) { return (root == nullptr || root == t) ? nullptr : Parent(root, t); }
	BinTreeNode<T>* LeftChild(BinTreeNode<T>* t) { return (t != nullptr) ? t->leftChild : nullptr; }
	BinTreeNode<T>* RightChild(BinTreeNode<T>* t) { return (t != nullptr) ? t->rightChild : nullptr; }
	BinTreeNode<T>* GetRoot() const { return root; }	

	void PreOrder(void (*visit) (BinTreeNode<T>* t)) { PreOrder(root, visit); }		
	void InOrder(void (*visit) (BinTreeNode<T>* t)) { InOrder(root, visit); }		
	void PostOrder(void (*visit) (BinTreeNode<T>* t)) { PostOrder(root, visit); }	

	int depth(BinTreeNode<T>* t, int& ans)
	{
		if (t == nullptr)
			return 0;
		else
		{
			int left = 0, right = 0;
			if (t->leftChild != nullptr)
				left = depth(t->leftChild, ans) + 1;
			if (t->rightChild != nullptr)
				right = depth(t->rightChild, ans) + 1;
			ans = max(ans, left + right);
			return max(left, right);
		}
	}

protected:
	BinTreeNode<T>* root; 	
	T RefValue;	 			

	void CreateBinTree(istream& in, BinTreeNode<T>*& subTree);	
	void CreateBinTree(int len, BinTreeNode<T>*& subTree);		
	BinTreeNode<T>* CreateBinTree(T* VLR, T* LVR, int n, BinTreeNode<T>* r);
				
	void Destroy(BinTreeNode<T>*& subTree);						

	void PreOrder(BinTreeNode<T>* subTree, void (*visit) (BinTreeNode<T>* t));	
	void InOrder(BinTreeNode<T>* subTree, void (*visit) (BinTreeNode<T>* t));	
	void PostOrder(BinTreeNode<T>* subTree, void (*visit) (BinTreeNode<T>* t));	
};

template<class T>
void BinaryTree<T>::CreateBinTree(istream& in, BinTreeNode<T>*& subTree)
{
	T item;
	if (!in.eof())
	{
		in >> item;
		if (item != RefValue)
		{
			subTree = new BinTreeNode<T>(item);
			if (subTree == nullptr)
			{
				cerr << "´æ´¢·ÖÅä´í!" << endl;
				exit(1);
			}
			CreateBinTree(in, subTree->leftChild);
			CreateBinTree(in, subTree->rightChild);
		}
		else subTree = nullptr;
	}
}

template<class T>
void BinaryTree<T>::CreateBinTree(int len, BinTreeNode<T>*& subTree)
{
	if (len <= 0)
		return;

	T item;
	cin >> item;
	if (item != RefValue)
	{
		subTree = new BinTreeNode<T>(item);
		if (subTree == nullptr)
		{
			cerr << "´æ´¢·ÖÅä´í!" << endl;
			exit(1);
		}
		CreateBinTree(len - 1, subTree->leftChild);
		CreateBinTree(len - 1, subTree->rightChild);
	}
	else
		subTree = nullptr;

}

template<class T>
BinTreeNode<T>* BinaryTree<T>::CreateBinTree(T* VLR, T* LVR, int n, BinTreeNode<T>* r)
{
	if (n <= 0)
		return nullptr;

	int k = 0;
	while (VLR[0] != LVR[k])
		k++;
	BinTreeNode<T>* t = new BinTreeNode<T>(VLR[0]);
	t->leftChild = CreateBinTree(VLR + 1, LVR, k, t);
	t->rightChild = CreateBinTree(VLR + k + 1, LVR + k + 1, n - k - 1, t);
	return t;
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
void BinaryTree<T>::PreOrder(BinTreeNode<T>* subTree, void(*visit)(BinTreeNode<T>* t))
{
	visit(subTree);
	if (subTree != nullptr)
	{
		PreOrder(subTree->leftChild, visit);
		PreOrder(subTree->rightChild, visit);
	}
}

template<class T>
void BinaryTree<T>::InOrder(BinTreeNode<T>* subTree, void(*visit)(BinTreeNode<T>* t))
{
	if (subTree != nullptr)
		PreOrder(subTree->leftChild, visit);
	visit(subTree);
	if (subTree != nullptr)
		PreOrder(subTree->rightChild, visit);
}

template<class T>
void BinaryTree<T>::PostOrder(BinTreeNode<T>* subTree, void(*visit)(BinTreeNode<T>* t))
{
	if (subTree != nullptr)
	{
		PreOrder(subTree->leftChild, visit);
		PreOrder(subTree->rightChild, visit);
	}
	visit(subTree);
}

