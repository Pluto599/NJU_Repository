#include<iostream>
#include<vector>
using namespace std;

#define MAX 2147483647

int main()
{
	int n, m, s;
	cin >> n >> m >> s;
	int** mtx = new int* [n + 1];
	bool* rcv = new bool[n + 1];
	int* dist = new int[n + 1];
	for (int i = 0; i < n + 1; i++)
	{
		mtx[i] = new int[n + 1];
		rcv[i] = 0;
		dist[i] = MAX;
		for (int j = 1; j < n + 1; j++)
		{
			if (j == i)
				mtx[i][j] = 0;
			else
				mtx[i][j] = MAX;
		}
	}

	for (int i = 0; i < m; i++)
	{
		int x, y, w;
		cin >> x >> y >> w;
		mtx[x][y] = w;
		if (x == s)
			dist[y] = w;
	}

	dist[s] = 0;
	rcv[s] = 1;
	for (int i = 0; i < n - 1; i++)
	{
		int min = MAX;
		int u = s;
		for (int j = 1; j < n + 1; j++)
		{
			if (rcv[j] == 0 && dist[j] < min)
			{
				u = j; min = dist[j];
			}
		}
		rcv[u] = 1;
		for (int k = 1; k < n + 1; k++)
		{
			int w = mtx[u][k];
			if (rcv[k] == 0 && w < MAX && dist[u] + w < dist[k])
				dist[k] = dist[u] + w;
		}
	}

	int t = 0;
	for (int i = 1; i < n + 1; i++)
	{
		if (dist[i] == MAX)
		{
			cout << -1;
			return 0;
		}
		t = max(t, dist[i]);
	}
	cout << t;

	return 0;
}