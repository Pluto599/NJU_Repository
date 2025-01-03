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
	BST(long long x) { k = x; root = nullptr; }

	BSTNode<T>* search(T x) { return search(x, root); }
	bool insert(T x) { return insert(x, root); }
	bool remove(T x) { return remove(x, root); }
	void LevelOrderDisplay() { LevelOrderDisplay(root); }

	void Solution(long long n)
	{

	}

protected:
	BSTNode<T>* root;
	T refValue;
	long long k;

	BSTNode<T>* search(T x, BSTNode<T>* p);
	bool insert(T x, BSTNode<T>*& p);
	bool remove(T x, BSTNode<T>*& p);
	void LevelOrderDisplay(BSTNode<T>* t);
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

template<class T>
void BST<T>::LevelOrderDisplay(BSTNode<T>* t)
{
	queue<BSTNode<T>*> q;
	BSTNode<T>* p = t;
	q.push(p);
	while (!q.empty())
	{
		p = q.front();
		T x = p->data;
		cout << x << ' ';
		q.pop();
		if (p->leftChild)
			q.push(p->leftChild);
		if (p->rightChild)
			q.push(p->rightChild);
	}
}



int main()
{
	long long m, n, k;
    cin >> m >> n >> k;
	long long* initialReelImpacts = new long long[m];
	for (long long i = 0; i < m; i++)
		cin >> initialReelImpacts[i];
	long long* newReelImpacts = new long long[n];
	for (long long i = 0; i < n; i++)
		cin >> newReelImpacts[i];

	BST<long long> impacts(k);
	for (long long i = 0; i < m; i++)
		impacts.insert(initialReelImpacts[i]);
	impacts.Solution(n);

	return 0;
}