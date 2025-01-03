#include<iostream>
#include<iomanip>
using namespace std;

int main()
{
	int n, prime, m;
	cin >> n >> prime >> m;
	int* hash = new int[n];
	for (int i = 0; i < n; i++)
		hash[i] = 0;
	float ASL_succ = 0;
	float ASL_unsucc = 0;

	int index, sl;
	for (int i = 0; i < m; i++)
	{
		int x;
		cin >> x;
		index = x % prime;
		sl = 1;
		while (hash[index] != 0)
		{
			index = (index + 1) % n;
			sl++;
		} 
		hash[index] = x;
		ASL_succ += sl;
	}
	ASL_succ /= m;

	for (int i = 0; i < prime; i++)
	{
		index = i;
		sl = 1;
		while (hash[index] != 0)
		{
			index = (index + 1) % n;
			sl++;
		}
		ASL_unsucc += sl;
	}
	ASL_unsucc /= prime;

	cout << fixed << setprecision(3) << ASL_succ << ' ' << fixed << setprecision(3) << ASL_unsucc;

	return 0;
}