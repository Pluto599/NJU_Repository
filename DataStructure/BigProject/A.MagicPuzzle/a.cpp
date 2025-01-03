#include<iostream>
using namespace std;

struct TreeNode
{
	bool out;
	int left;
	int right;

	TreeNode() :out(false), left(-1), right(-1) {}
};

class Tree
{
public:
	Tree();
	void solution();

private:
	TreeNode* treeList;
	int n, m;
	int cnt;

	void reverse(int i);
	void check(int i);
};

int main()
{
	Tree tree;
	tree.solution();

	return 0;
}

Tree::Tree()
{
	cin >> n >> m;

	treeList = new TreeNode[n + 1];
	int p;
	for (int i = 2; i <= n; i++)
	{
		cin >> p;

		if (treeList[p].left == -1)
			treeList[p].left = i;
		else
			treeList[p].right = i;
	}

	int x;
	for (int i = 0; i < m; i++)
	{
		cin >> x;
		treeList[x].out = true;
	}
		
}

void Tree::solution()
{
	cnt = 0;

	check(1);
	cout << cnt;
}

void Tree::reverse(int i)
{
	if (i == -1)
		return;

	treeList[i].out = !treeList[i].out;
	reverse(treeList[i].left);
	reverse(treeList[i].right);
}

void Tree::check(int i)
{
	if (i == -1)
		return;

	if (treeList[i].out)
	{
		cnt++;
		reverse(i);
	}
	check(treeList[i].left);
	check(treeList[i].right);
}
