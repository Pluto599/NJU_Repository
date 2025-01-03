#include<iostream>
using namespace std;
#include<queue>


struct TreeNode {
	int data;
	TreeNode* leftChild, * rightChild;

	TreeNode()
	{
		leftChild = nullptr;  rightChild = nullptr;
	}
	TreeNode(int x, TreeNode* l = nullptr, TreeNode* r = nullptr)
	{
		data = x;  leftChild = l;  rightChild = r;
	}
};


class Tree
{
public:
	Tree() :root(nullptr) {}
	Tree(int value) :RefValue(value), root(nullptr) {}
	~Tree() { Destroy(root); }

	void CreateTree(int len) { CreateTree(len, root); }
	void setBlood(int b) { bloodSum = b; }
	void Solution()
	{
		chooseWay(root, 0, 0);
		cout << ans;
	}

protected:
	TreeNode* root;
	int RefValue;
	int bloodSum;
	int ans = 0;

	void CreateTree(int len, TreeNode*& subTree);
	void Destroy(TreeNode*& subTree);
	void chooseWay(TreeNode* t, int curBlood, bool chosen)
	{
		if (t == nullptr)
			return;

		if (!chosen)
		{
			chooseWay(t->leftChild, curBlood, 0);
			chooseWay(t->rightChild, curBlood, 0);
		}

		curBlood += t->data;
		if (curBlood == bloodSum)
			ans++;

		chooseWay(t->leftChild, curBlood, 1);
		chooseWay(t->rightChild, curBlood, 1);

	}

};


void Tree::CreateTree(int len, TreeNode*& subTree)
{
	if (len <= 0)
		return;

	int item;
	cin >> item;
	subTree = new TreeNode(item);

	queue<TreeNode*> q;
	TreeNode* p;
	q.push(subTree);

	len--;

	while (len > 0)
	{
		p = q.front();
		q.pop();

		cin >> item;

		if (item != RefValue)
		{
			p->leftChild = new TreeNode(item);
			q.push(p->leftChild);
		}
		else
			p->leftChild = nullptr;

		if (len > 1)
		{
			cin >> item;
			if (item != RefValue)
			{
				p->rightChild = new TreeNode(item);
				q.push(p->rightChild);
			}
			else
				p->rightChild = nullptr;
		}

		len -= 2;
	}
}


void Tree::Destroy(TreeNode*& subTree)
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
	int bloodSum, n;
	cin >> bloodSum >> n;
	Tree* tree = new Tree(1000000);
	tree->CreateTree(n);
	tree->setBlood(bloodSum);
	tree->Solution();

	return 0;
}