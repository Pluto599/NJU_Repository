#include<iostream>
using namespace std;

int main()
{
	int n;
	cin >> n;
	int* edges = new int[n];
	int** table = new int* [n];
	for (int i = 0; i < n; i++)
	{
		table[i] = new int[n];
		for (int j = 0; i < n; j++)
			table[i][j] = -1;
	}
	int i = 0;
	for (; i < n; i++)
	{
		cin >> edges[i];
		if (edges[i] != -1)
		{
			table[i][edges[i]] = 1;
			if (table[edges[i]][i] == 1)
				break;
			table[edges[i]][i] = 0;
		}
	}
	if (i == n)
		cout << -1;
	else
	{
		int len = 0;
	}
	return 0;
}