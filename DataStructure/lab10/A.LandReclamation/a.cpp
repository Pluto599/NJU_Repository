#include<iostream>
#include<string.h>
using namespace std;

class UFSets
{
public:
	UFSets(int sz = 10);
	UFSets(UFSets& s);
	~UFSets() { delete[]parent; }
	int find(int x);
	void weightedUnion(int root1, int root2);

	int* parent;

private:
	int size;
};

int main()
{
	int n;
	cin >> n;
	int** area = new int* [n];
	UFSets land(n * n);

	for (int i = 0; i < n; i++)
	{
		area[i] = new int[n];
		for (int j = 0; j < n; j++)
		{
			cin >> area[i][j];

			if (area[i][j] && i > 0 && area[i - 1][j] == 1)
				land.weightedUnion(i * n + j, (i - 1) * n + j);
			if (area[i][j] && j > 0 && area[i][j - 1] == 1)
				land.weightedUnion(i * n + j, i * n + j - 1);
		}
	}

	int ans = 0;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
		{
			if (area[i][j] == 1)
				continue;

			int cur = 1;
			int up = -1, down = -1, left = -1, right = -1;

			if (i > 0 && area[i - 1][j] == 1)
			{
				up = land.find((i - 1) * n + j);
				cur -= land.parent[up];
			}
			if (i < n - 1 && area[i + 1][j] == 1)
			{
				down = land.find((i + 1) * n + j);
				if (down != up)
					cur -= land.parent[down];
			}
			if (j > 0 && area[i][j - 1] == 1)
			{
				left = land.find(i * n + j - 1);
				if (left != up && left != down)
					cur -= land.parent[left];
			}
			if (j < n - 1 && area[i][j + 1] == 1)
			{
				right = land.find(i * n + j + 1);
				if (right != up && right != down && right != left)
					cur -= land.parent[right];
			}

			ans = max(ans, cur);
		}

	cout << ans;

	return 0;
}

UFSets::UFSets(int sz)
{
	size = sz;
	parent = new int[size];
	memset(parent, -1, sizeof(int) * size);
}

UFSets::UFSets(UFSets& s)
{
	size = s.size;
	parent = new int[size];
	memcpy(parent, s.parent, sizeof(int) * size);
}

int UFSets::find(int x)
{
	if (parent[x] < 0)
		return x;
	else
		//return parent[x] = find(parent[x]);
		return find(parent[x]);
}

void UFSets::weightedUnion(int root1, int root2)
{
	int r1 = find(root1), r2 = find(root2), temp;
	if (r1 != r2)
	{
		temp = parent[r1] + parent[r2];
		if (parent[r2] < parent[r1])
		{
			parent[r1] = r2;
			parent[r2] = temp;
		}
		else
		{
			parent[r2] = r1;
			parent[r1] = temp;
		}
	}
}
