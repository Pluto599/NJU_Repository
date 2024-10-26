#include<iostream>
using namespace std;

// ab*d
// -1 0 ? ?
// ababcd
// ab*de
// 
// ababcde
// 

int* next(string pat)
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
void update_next(string pat, int upd_i, int* next)
{
	int i = upd_i;
	int k = next[i];
	while (1)
	{
		if (k == -1 || pat[i] == pat[k])
		{
			k++;
			i++;
			if (i == pat.size())
				break;

			//// 优化：避免多次回退指针
			//if (pat[i] == pat[k])
			//	k = next[k];

			next[i] = k;
		}
		else
		{
			k = next[k];
		}
	}
}

bool find(string text,  string pat, int* nxt, string pat_pre, string pat_suff)
{
	int t = 0, p = 0;
	while (t < text.size() && p < pat.size())
	{
		if (p == -1 || text[t] == pat[p] || pat[p] == '*')
		{
			if (pat[p] == '*')
			{
				string upd_p;
				upd_p += pat_pre;
				upd_p += text[t];
				upd_p += pat_suff;
				update_next(upd_p, p - 1, nxt);
			}
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

	int star = pat.find('*');
	string pat_pre, pat_suff;
	for (int i = 0; i < star; i++)
		pat_pre += pat[i];
	for (int i = star + 1; i < pat.size(); i++)
		pat_suff += pat[i];
	int* nxt = next(pat);

	for (int i = 0; i < n; i++)
	{
		if (find(texts[i], pat, nxt,pat_pre, pat_suff))
			cout << "true" << endl;
		else
			cout << "false" << endl;
	}

	return 0;
}