#include<iostream>
using namespace std;

int main()
{
	int m, n;
	cin >> m >> n;
	int* parent = new int[m + 1];
	for (int i = 0; i < m + 1; i++)
		parent[i] = -1;

	int p, ch;
	int l, r;
	for (int i = 0; i < n; i++)
	{
		cin >> l >> r;

		p = l; 
		ch = r;
		while (parent[p] != -1) 
			p = parent[p];
		while (parent[ch] != -1) 
			ch = parent[ch];
			
		if (p != ch)
			parent[ch] = p;
	}

	int ans = -1;
	for (int i = 1; i < m + 1; i++)
		if (parent[i] == -1)
			ans++;
	cout << ans;

	return 0;
}