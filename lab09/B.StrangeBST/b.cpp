#include<iostream>
#include<queue>
using namespace std;

#include<iostream>
#include<queue>
using namespace std;

template<class T>
struct BSTNode
{
	T data;
	BSTNode<T>* leftChild;
	BSTNode<T>* rightChild;

	BSTNode<T>(T x, BSTNode<T>* l = nullptr, BSTNode<T>* r = nullptr)
	{
		data = x;
		leftChild = l;
		rightChild = r;
	}
};

template<class T>
class BST
{
public:
	BST() :root(nullptr) {}
	BST(T ref) { refValue = ref; root = nullptr; }

	BSTNode<T>* search(T x) { return search(x, root); }
	bool insert(T x) { return insert(x, root); }
	bool remove(T x) { return remove(x, root); }
	void LevelOrderDisplay() { LevelOrderDisplay(root); }

protected:
	BSTNode<T>* root;
	T refValue;

	BSTNode<T>* search(T x, BSTNode<T>* p);
	bool insert(T x, BSTNode<T>*& p);
	bool remove(T x, BSTNode<T>*& p);
};

template<class T>
BSTNode<T>* BST<T>::search(T x, BSTNode<T>* p)
{
	if (p == nullptr)
		return nullptr;
	else if (x < p->data)
		search(x, p->leftChild);
	else if (x > p->data)
		search(x, p->rightChild);
	else
		return p;
}

template<class T>
bool BST<T>::insert(T x, BSTNode<T>*& p)
{
	if (p == nullptr)
	{
		p = new BSTNode<T>(x);
		return true;
	}
	else if (x < p->data)
		insert(x, p->leftChild);
	else if (x > p->data)
		insert(x, p->rightChild);
	else
		return false;

}

template<class T>
bool BST<T>::remove(T x, BSTNode<T>*& p)
{
	BSTNode<T>* temp;
	if (p != nullptr)
	{
		if (x < p->data)
			remove(x, p->leftChild);
		else if (x > p->data)
			remove(x, p->rightChild);
		else if (p->leftChild != nullptr && p->rightChild != nullptr)
		{
			temp = p->leftChild;
			while (temp->rightChild != nullptr)
				temp = temp->rightChild;
			p->data = temp->data;
			remove(p->data, p->leftChild);
		}
		else
		{
			temp = p;
			if (p->leftChild == nullptr)
				p = p->rightChild;
			else if (p->rightChild == nullptr)
				p = p->leftChild;
			delete temp;
			temp = nullptr;

			return true;
		}
	}

	return false;
}


int main()
{
	int n;
	cin >> n;
	int* a = new int[n];
	for (int i = 0; i < n; i++)
		cin >> a[i];
	BST<int>* tree = new BST<int>();
	tree->insert(a[0]);
	for (int i = 1; i < n; i++)
		tree->insert(a[i]);

	return 0;
}