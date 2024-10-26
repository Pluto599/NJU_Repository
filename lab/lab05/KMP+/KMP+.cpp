#include<iostream>
using namespace std;

// ab*d
// -1 0 ? ?
// ababcd
// ab*de
// 
// ababcde
// 

int* Next(string& pat)
{
	int* next = new int[pat.size()];
	next[0] = (pat[0] == '*') ? -2 : -1;
	int i = 0;
	int k = next[0];
	while (1)
	{
		if (k == -1 || pat[i] == pat[k] || pat[k] == '*')
		{
			k++;
			i++;
			if (i == pat.size())
				break;

			// 优化：避免多次回退指针
			if (pat[i] == pat[k])
				k = next[k];

			next[i] = (pat[i] == '*') ? -2 : k;
		}
		else
		{
			k = next[k];
		}
	}

	return next;
}

bool find(string text,  string& pat, int start)	//字符串的模式匹配算法
{
	//KMP算法
	int* next = Next(pat);
	int t = start, p = 0;
	while (t < text.size() && p < pat.size())
	{
		if (p == -1 || text[t] == pat[p] || pat[p] == '*')
		{
			t++;
			p++;
		}
		else
			p = next[p];
	}
	if (p == pat.size())	//匹配成功
		return true;
	else					//匹配失败
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

	int star = pat.find('*');
	

	for (int i = 0; i < n; i++)
	{
		if (find(texts[i], pat, 0))
			cout << "true" << endl;
		else
			cout << "false" << endl;
	}

	return 0;
}