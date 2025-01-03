#include<iostream>
#include<queue>
using namespace std;

struct TreeNode
{
	int data;
	TreeNode* left;
	TreeNode* right;
	TreeNode* parent;

	TreeNode(int d, TreeNode* l = nullptr, TreeNode* r = nullptr, TreeNode* p = nullptr) :data(d), left(l), right(r), parent(p) {}
};

class Tree
{
public:
	Tree() :root(nullptr) {}
	Tree(int v) :RefValue(v), root(nullptr) {}

	void CreateTree(int len) { CreateTree(len, root); }

	void solution();

private:
	TreeNode* root;
	int RefValue;

	//
	queue<TreeNode*> leaves;

	void CreateTree(int len, TreeNode*& subTree);
	void findLeaves(TreeNode* subTree);
};

int main()
{
	int n;
	cin >> n;
	Tree* tree = new Tree(0);
	tree->CreateTree(n);
	tree->solution();

	return 0;
}

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
			p->left = new TreeNode(item);
			q.push(p->left);

			//
			p->left->parent = p;
		}
		else
			p->left = nullptr;

		if (len > 1)
		{
			cin >> item;
			if (item != RefValue)
			{
				p->right = new TreeNode(item);
				q.push(p->right);

				//
				p->right->parent = p;
			}
			else
				p->right = nullptr;
		}

		len -= 2;
	}
}

void Tree::findLeaves(TreeNode* subTree)
{
	if (subTree != nullptr)
	{
		if (subTree->left == nullptr && subTree->right == nullptr)
			leaves.push(subTree);
		findLeaves(subTree->left);
		findLeaves(subTree->right);
	}
}


void Tree::solution()
{
	findLeaves(root);

	while (!leaves.empty())
	{
		TreeNode* p = leaves.front();
		TreeNode* pr = p->parent;
		leaves.pop();

		cout << p->data << ' ';
		if (pr != nullptr)
		{
			if (pr->left == p)
				pr->left = nullptr;
			else
				pr->right = nullptr;

			if (pr->left == nullptr && pr->right == nullptr)
				leaves.push(pr);
		}
		delete p;
		p = nullptr;

		
	}
}