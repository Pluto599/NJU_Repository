#include<iostream>
using namespace std;

struct TreeNode
{
	bool out;	//表示蜡烛亮灭状态：灭 true，亮false
	int left;	//指向该节点的左子树
	int right;	//指向该节点的右子树

	TreeNode() :out(false), left(-1), right(-1) {}
};

class Tree
{
public:
	Tree();
	void solution();

private:
	TreeNode* treeList;	//数组存储二叉树，数组下标即为节点编号
	int n, m;	// n 表示蜡烛阵的蜡烛数量， m 表示集合 S 的大小
	int cnt;	//存储翻转操作的次数

	void reverse(int i);	//翻转操作：将蜡烛节点 i 及其子树中所有蜡烛节点的亮灭状态翻转
	void check(int i);	//从节点 i 开始依次向后检查蜡烛节点
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

void Tree::reverse(int i)	//翻转操作：将蜡烛节点 i 及其子树中所有蜡烛节点的亮灭状态翻转
{
	if (i == -1)	//若节点为空，则结束递归；否则按前序排序进行递归

		return;

	treeList[i].out = !treeList[i].out;	//改变当前节点的亮灭状态
	//依次对左右子树递归进行翻转操作
	reverse(treeList[i].left);
	reverse(treeList[i].right);
}

void Tree::check(int i)	//从节点 i 开始依次向后检查蜡烛节点
{
	if (i == -1)	//若节点为空，则结束递归；否则按前序排序进行递归
		return;

	if (treeList[i].out)	//若当前节点状态为灭，翻转该节点并计数加 1
	{
		cnt++;
		reverse(i);
	}
	// 依次对左右子树递归进行检查
	check(treeList[i].left);
	check(treeList[i].right);
}
