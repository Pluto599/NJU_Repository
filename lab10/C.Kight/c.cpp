#include<iostream>
#include<vector>
#include<stack>
using namespace std;

#define MAX 2147483647

struct edge
{
	int dest;
	int cost;
	edge* link;

	edge(int num, int weight) :dest(num), cost(weight), link(nullptr) {}
};

struct vertex
{
	int data;
	edge* adj;
};


class Graph
{
public:
	Graph();

	void Solution()
	{
		while (left <= right)
		{
			int mid = (left + right) / 2;
			if (maxWeight(s, mid))
			{
				ans = mid;
				right = mid - 1;
			}
			else
			{
				left = mid + 1;
			}
		}

		cout << ans;
	}

private:
	int n, m, s, t;
	vertex* blocks;

	vector<int> path;
	int ans = MAX;
	int left = 0, right = 0;

	bool maxWeight(int v, int weight);
};




int main()
{
	Graph* g = new Graph();
	g->Solution();

	return 0;
}

Graph::Graph()
{
	cin >> n >> m >> s >> t;
	blocks = new vertex[n + 1];
	for (int i = 0; i < n + 1; i++)
	{
		blocks[i].data = i;
		blocks[i].adj = nullptr;
	}
	for (int i = 0; i < m; i++)
	{
		int x, y, w;
		cin >> x >> y >> w;

		right = max(right, w);

		edge* p = blocks[x].adj;
		while (p != nullptr)
			p = p->link;
		p = new edge(y, w);
		p->link = blocks[x].adj;
		blocks[x].adj = p;

		edge* q = blocks[y].adj;
		while (q != nullptr)
			q = q->link;
		q = new edge(x, w);
		q->link = blocks[y].adj;
		blocks[y].adj = q;
	}

}

bool Graph::maxWeight(int v, int weight)
{
	bool* visited = new bool[n + 1];
	for (int i = 0; i < n + 1; i++)
		visited[i] = false;

	stack<int> st;
	st.push(s);
	visited[v] = true;

	while (!st.empty())
	{
		int v = st.top();
		st.pop();

		if (v == t)
			return true;

		edge* w = blocks[v].adj;
		while (w != nullptr)
		{
			if (!visited[w->dest] && w->cost <= weight)
			{
				visited[w->dest] = true;
				st.push(w->dest);
			}

			w = w->link;
		}
	}

	return false;
}
