#include<iostream>
using namespace std;

//#include<vector>
//#include<queue>
//struct cmp
//{
//	bool operator()(pair<int, int>& a, pair<int, int>& b)
//	{
//		return a.first > b.first;
//	}
//};
//using MyQueue = priority_queue<pair<int, int>, vector<pair<int, int>>, cmp>;

void sort(int** a, int n)
{
	for (int i = 0; i < n - 1; i++)
	{
		int min = i;
		int j;
		for (j = i + 1; j < n; j++)
		{
			if (a[j][0] < a[min][0])
				min = j;
		}
		if (min != i)
		{
			int temp[2] = { a[i][0],a[i][1] };
			a[i][0] = a[min][0];
			a[i][1] = a[min][1];
			a[min][0] = temp[0];
			a[min][1] = temp[1];
		}
	}
}
void sort_sec(int** a, int n)
{
	int left = 0, right = 0;
	int cur = a[0][0];
	for (int i = 1; i < n; i++)
	{
		if (cur == a[i][0])
			right = i;
		else
		{
			if (right - left > 0)
			{
				for (int j = left; j < right; j++)
				{
					int min = j;
					for (int k = j + 1; k <= right; k++)
					{
						if (a[k][1] < a[min][1])
							min = k;
					}
					if (min != j)
					{
						int temp = a[j][1];
						a[j][1] = a[min][1];
						a[min][1] = temp;
					}
				}
			}
			left = i;
			cur = a[i][0];
		}
	}
}

struct edge
{
	int dest;
	int cost;
	edge* link;
	edge(int d, int c) :dest(d), cost(c), link(nullptr) {}
};

struct vertex
{
	int count;
	edge* adj;
};

class AOE
{
public:
	AOE();
	void solution()
	{
		TopologicalSort();
		CrucialPath();

		sort(ans, cnt);
		sort_sec(ans, cnt);

		for (int i = 0; i < cnt; i++)
			cout << ans[i][0] << ' ' << ans[i][1] << endl;
	
		//while (!ans.empty())
		//{
		//	pair<int, int> temp = ans.top();
		//	ans.pop();
		//	cout << temp.first << ' ' << temp.second << endl;
		//}
	}

private:
	int n, e;
	vertex* web;

	int* count;
	int* order;

	int** ans;
	int cnt;
	//MyQueue ans;

	int getFirstNeighbor(int v);
	int getNextNeighbor(int v, int w);
	int getWeight(int v, int w);

	void TopologicalSort();
	void CrucialPath();

};

int main()
{
	AOE* g = new AOE();
	g->solution();

	return 0;
}

AOE::AOE()
{
	cin >> n >> e;
	web = new vertex[n];
	for (int i = 0; i < n; i++)
	{
		web[i].count = i;
		web[i].adj = nullptr;
	}

	order = new int[n];
	count = new int[n];
	for (int i = 0; i < n; i++)
		count[i] = 0;

	int from, to, weight;
	for (int i = 0; i < e; i++)
	{
		cin >> from >> to >> weight;
		
		edge* p = new edge(to, weight);
		p->link = web[from].adj;
		web[from].adj = p;

		count[to]++;
	}

	ans = new int* [n];
	for (int i = 0; i < n; i++)
		ans[i] = new int[2];
}

int AOE::getFirstNeighbor(int v)
{
	edge* p = web[v].adj;
	return (p == nullptr) ? -1 : p->dest;
}

int AOE::getNextNeighbor(int v, int w)
{
	edge* p = web[v].adj;
	while (p != nullptr && p->dest != w)
		p = p->link;
	return (p == nullptr || p->link == nullptr) ? -1 : p->link->dest;
}

int AOE::getWeight(int v, int w)
{
	edge* p = web[v].adj;
	while (p != nullptr && p->dest != w)
		p = p->link;
	return (p == nullptr) ? -1 : p->cost;
}

void AOE::TopologicalSort()
{
	int top = -1;    
	
	for (int i = 0; i < n; i++)      	
		if (count[i] == 0)        	
		{
			count[i] = top;  
			top = i;
		}
	for (int i = 0; i < n; i++)      		                             	
	{
		int v = top;  
		top = count[top];
		// 
		order[i] = v;
		// 
		edge* w = web[v].adj;
		while (w != nullptr) 
		{
			count[w->dest]--;
			if (!count[w->dest])
			{
				count[w->dest] = top;  
				top = w->dest;
			}
			w = w->link;
		}
	}
};

void AOE::CrucialPath()
{
	int Ae, Al, dur;
	int* Ve = new int[n];  
	int* Vl = new int[n];
	for (int i = 0; i < n; i++) 
	{
		Ve[i] = 0;
		Vl[i] = 2147483647;
	}

	for (int k = 0; k < n; k++)
	{  
		int i = order[k];
		edge* j = web[i].adj;
		while (j != nullptr) 
		{
			dur = getWeight(i, j->dest);
			if (Ve[i] + dur > Ve[j->dest]) 
				Ve[j->dest] = Ve[i] + dur;
			j = j->link;
		}
	}

	Vl[n - 1] = Ve[n - 1];
	for (int i = n - 2; i > 0; i--) 
	{ 
		int j = order[i];
		edge* k = web[j].adj;
		while (k != nullptr) 
		{
			dur = getWeight(j, k->dest);
			if (Vl[k->dest] - dur < Vl[j])
				Vl[j] = Vl[k->dest] - dur;
			k = k->link;
		}
	}

	 cnt = 0;
	for (int k = 0; k < n; k++) 
	{
		int i = order[k];
		edge* j = web[i].adj;
		while (j != nullptr) 
		{
			Ae = Ve[i];  
			Al = Vl[j->dest] - getWeight(i, j->dest);
			if (Al == Ae)
			{
				ans[cnt][0] = i;
				ans[cnt][1] = j->dest;
				cnt++;
				//ans.emplace(i, j->dest);
			}
			j = j->link;
		}
	}
	delete[] Ve;  delete[] Vl;

};