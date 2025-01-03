#include<iostream>
#include<queue>
using namespace std;

struct BSTNode
{
	int key;
	BSTNode* leftChild;
	BSTNode* rightChild;

	BSTNode(int k, BSTNode* l = nullptr, BSTNode* r = nullptr)
	{
		key = k;
		leftChild = l;
		rightChild = r;
	}
};

class BST
{
public:
	BST() :root(nullptr) {}

	bool insert(int k) { return insert(k, root); }
	bool remove(int k) { return remove(k, root); }
	void LevelOrderDisplay() { LevelOrderDisplay(root); }

protected:
	BSTNode* root;
	bool insert(int k, BSTNode*& p);
	bool remove(int k, BSTNode*& p);
	void LevelOrderDisplay(BSTNode* t);
};

bool BST::insert(int k, BSTNode*& p)
{
	if (p == nullptr)
	{
		p = new BSTNode(k);
		return true;
	}
	else if (k < p->key)
		insert(k, p->leftChild);
	else if (k > p->key)
		insert(k, p->rightChild);
	else
		return false;

}

bool BST::remove(int k, BSTNode*& p)
{
	BSTNode* temp;
	if (p != nullptr)
	{
		if (k < p->key)
			remove(k, p->leftChild);
		else if (k > p->key)
			remove(k, p->rightChild);
		else if (p->leftChild != nullptr && p->rightChild != nullptr)
		{
			temp = p->leftChild;
			while (temp->rightChild != nullptr)
				temp = temp->rightChild;
			p->key = temp->key;
			remove(p->key, p->leftChild);
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

void BST::LevelOrderDisplay(BSTNode* t)
{
	queue<BSTNode*> q;
	BSTNode* p = t;
	q.push(p);
	while(!q.empty())
	{
		p = q.front();
		int k = p->key;
		cout << k << ' ';
		q.pop();
		if (p->leftChild)
			q.push(p->leftChild);
		if (p->rightChild)
			q.push(p->rightChild);
	}
}


int main()
{
	int n, m;
	cin >> n >> m;
	BST tree;
	for (int i = 0; i < n; i++)
	{
		int x;
		cin >> x;
		tree.insert(x);
	}
	for (int i = 0; i < m; i++)
	{
		int x;
		cin >> x;
		tree.remove(x);
	}
	tree.LevelOrderDisplay();

	return 0;
}