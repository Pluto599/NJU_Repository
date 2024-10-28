#include<iostream>
using namespace std;

int* next(string pat)
{
	int* next = new int[pat.size()];
	next[0] = -1;
	int i = 0;
	int k = next[0];
	while (1)
	{
		if (k == -1 || pat[i] == pat[k] || pat[i] == '*' || pat[k] == '*')
		{
			k++;
			i++;
			if (i == pat.size())
				break;
			next[i] = k;
		}
		else
		{
			k = next[k];
		}
	}

	return next;
}

bool find(string text, string pat)
{
	int* nxt = next(pat);
	int t = 0, p = 0;
	while (t < text.size() && p < (int)pat.size())
	{
		if (p == -1 || text[t] == pat[p] || pat[p] == '*')
		{
			t++;
			p++;
		}
		else
			p = nxt[p];
	}

	if (p == pat.size())
		return true;
	else
		return false;
}

int main()
{
	int n;
	cin >> n;
	string pat;
	cin >> pat;
	string* texts = new string[n];
	for (int i = 0; i < n; i++)
		cin >> texts[i];

	//int star = pat.find('*');
	//string pat_pre = "", pat_suff = "";
	//for (int i = 0; i < star; i++)
	//	pat_pre += pat[i];
	//for (int i = star + 1; i < pat.size(); i++)
	//	pat_suff += pat[i];

	for (int i = 0; i < n; i++)
	{
		if (find(texts[i], pat))
			cout << "true" << endl;
		else
			cout << "false" << endl;
	}

	return 0;
}