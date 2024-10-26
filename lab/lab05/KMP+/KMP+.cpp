#include<iostream>
#include<string>
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
<<<<<<< HEAD:lab05/KMP+/KMP+.cpp
<<<<<<< HEAD:lab05/KMP+/KMP+.cpp
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

			//// ÓÅ»¯£º±ÜÃâ¶à´Î»ØÍËÖ¸Õë
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
=======

bool find(string text, string& pat, int start)	//ï¿½Ö·ï¿½ï¿½ï¿½ï¿½ï¿½Ä£Ê½Æ¥ï¿½ï¿½ï¿½ã·¨
{
	//KMPï¿½ã·¨
	int* next = Next(pat);
	int t = start, p = 0;
>>>>>>> cbfb347964def2b8469cde3aeea1a007826d561a:lab/lab05/KMP+/KMP+.cpp
=======

bool find(string text,  string& pat, int start)	//×Ö·û´®µÄÄ£Ê½Æ¥ÅäËã·¨
{
	//KMPËã·¨
	int* next = Next(pat);
	int t = start, p = 0;
>>>>>>> parent of 265fc8e (KMP+ 2024/10/26 19:22):lab/lab05/KMP+/KMP+.cpp
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
<<<<<<< HEAD:lab05/KMP+/KMP+.cpp
<<<<<<< HEAD:lab05/KMP+/KMP+.cpp
	if (p == pat.size())
		return true;
	else
=======
	if (p == pat.size())	//Æ¥ï¿½ï¿½É¹ï¿½
		return true;
	else					//Æ¥ï¿½ï¿½Ê§ï¿½ï¿½
>>>>>>> cbfb347964def2b8469cde3aeea1a007826d561a:lab/lab05/KMP+/KMP+.cpp
=======
	if (p == pat.size())	//Æ¥Åä³É¹¦
		return true;
	else					//Æ¥ÅäÊ§°Ü
>>>>>>> parent of 265fc8e (KMP+ 2024/10/26 19:22):lab/lab05/KMP+/KMP+.cpp
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

<<<<<<< HEAD:lab05/KMP+/KMP+.cpp
	int star = pat.find('*');
<<<<<<< HEAD:lab05/KMP+/KMP+.cpp
	string pat_pre, pat_suff;
	for (int i = 0; i < star; i++)
		pat_pre += pat[i];
	for (int i = star + 1; i < pat.size(); i++)
		pat_suff += pat[i];
	int* nxt = next(pat);
=======
	int star = pat.find('*');// star ????
	
>>>>>>> cbfb347964def2b8469cde3aeea1a007826d561a:lab/lab05/KMP+/KMP+.cpp
=======
	
>>>>>>> parent of 265fc8e (KMP+ 2024/10/26 19:22):lab/lab05/KMP+/KMP+.cpp

	for (int i = 0; i < n; i++)
	{
		if (find(texts[i], pat, 0))
			cout << "true" << endl;
		else
			cout << "false" << endl;
	}

	return 0;
}