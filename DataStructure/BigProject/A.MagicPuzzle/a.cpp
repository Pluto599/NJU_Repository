#include<iostream>
using namespace std;

struct TreeNode
{
	bool out;	//��ʾ��������״̬���� true����false
	int left;	//ָ��ýڵ��������
	int right;	//ָ��ýڵ��������

	TreeNode() :out(false), left(-1), right(-1) {}
};

class Tree
{
public:
	Tree();
	void solution();

private:
	TreeNode* treeList;	//����洢�������������±꼴Ϊ�ڵ���
	int n, m;	// n ��ʾ����������������� m ��ʾ���� S �Ĵ�С
	int cnt;	//�洢��ת�����Ĵ���

	void reverse(int i);	//��ת������������ڵ� i ������������������ڵ������״̬��ת
	void check(int i);	//�ӽڵ� i ��ʼ�������������ڵ�
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

void Tree::reverse(int i)	//��ת������������ڵ� i ������������������ڵ������״̬��ת
{
	if (i == -1)	//���ڵ�Ϊ�գ�������ݹ飻����ǰ��������еݹ�

		return;

	treeList[i].out = !treeList[i].out;	//�ı䵱ǰ�ڵ������״̬
	//���ζ����������ݹ���з�ת����
	reverse(treeList[i].left);
	reverse(treeList[i].right);
}

void Tree::check(int i)	//�ӽڵ� i ��ʼ�������������ڵ�
{
	if (i == -1)	//���ڵ�Ϊ�գ�������ݹ飻����ǰ��������еݹ�
		return;

	if (treeList[i].out)	//����ǰ�ڵ�״̬Ϊ�𣬷�ת�ýڵ㲢������ 1
	{
		cnt++;
		reverse(i);
	}
	// ���ζ����������ݹ���м��
	check(treeList[i].left);
	check(treeList[i].right);
}
