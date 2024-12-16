#include<iostream>
using namespace std;

struct infect
{
	int x, y, day;
	infect(int i, int j, int d) { x = i; y = j; day = d; }
};

int main()
{
	int n, m;
	cin >> n >> m;
	int* parent = new int[n + 1];
	for (int i = 0; i < n + 1; i++)
		parent[i] = -1;

	int p1, p2, d;
	for (int i = 0; i < m; i++)
	{
		cin >> p1 >> p2 >> d;
		infect newInfect(p1, p2, d);
	}

	

	return 0;
}