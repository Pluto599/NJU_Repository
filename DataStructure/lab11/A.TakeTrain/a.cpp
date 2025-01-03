#include<iostream>
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
	edge* adj;
};

class graph
{
private:
	int school, home, k, n, m;
	vertex* web;

	int bfs();

public:
	graph();

	void solution();
};

int main()
{
	graph g;
	g.solution();

	return 0;
}

graph::graph()
{
	cin >> school >> home >> k >> n >> m;
	web = new vertex[n];
	for (int i = 0; i < n; i++)
		web[i].adj = nullptr;
	
	for (int i = 0; i < m; i++)
	{
		int x, y, w;
		cin >> x >> y >> w;

		edge* p = web[x].adj;
		while (p != nullptr)
			p = p->link;
		p = new edge(y, w);
		p->link = web[x].adj;
		web[x].adj = p;
	}

}

void graph::solution()
{
	cout << bfs();
}

int graph::bfs()
{
	int* cost = new int[n];
	for (int i = 0; i < n; i++)
		cost[i] = MAX;
	cost[school] = 0;

	for (int i = 0; i <= k; i++)
	{
		int* nxt_cost = new int[n];
		for (int i = 0; i < n; i++)
			nxt_cost[i] = cost[i];

		for (int j = 0; j < n; j++)
			if (cost[j] < MAX && cost[j] < cost[home])
			{
				edge* w = web[j].adj;
				while (w != nullptr)
				{
					nxt_cost[w->dest] = min(cost[j] + w->cost, nxt_cost[w->dest]);
					w = w->link;
				}
			}

		for (int j = 0; j < n; j++)
			cost[j] = nxt_cost[j];
	}

	if (cost[home] == MAX)
		cost[home] = -1;

	return cost[home];
}