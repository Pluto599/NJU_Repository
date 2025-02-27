#include<iostream>
#include<algorithm>
using namespace std;

#define MAX 2147483647

const int DefaultSize = 128;

template<class T>
class MinHeap
{
public:
	MinHeap(int sz = DefaultSize);
	MinHeap(T arr[], int n);
	~MinHeap() { delete[]heap; }
	T top() { return heap[0]; }
	bool push(T& x);
	bool pop();
	bool pop(T& x);
	bool empty() const { return curSize == 0; }
	bool full() const { return curSize == maxHeapSize; }
	void clear() { curSize = 0; }

private:
	T* heap;
	int curSize;
	int maxHeapSize;
	void siftDown(int start, int m);
	void siftUp(int start);

};

struct HeapNode {
	int vertex;
	int distance;

	HeapNode(int v = 0, int d = 0) : vertex(v), distance(d) {};

	bool operator<(const HeapNode& h) const
	{
		return distance < h.distance;
	}

	bool operator<=(const HeapNode& h) const
	{
		return distance <= h.distance;
	}

	bool operator>(const HeapNode& h) const
	{
		return distance > h.distance;
	}

	bool operator>=(const HeapNode& h) const
	{
		return distance >= h.distance;
	}
};

struct edge
{
	int dest;
	int cost;
	edge* link;

	edge(int d, int c, edge* l = nullptr) :dest(d), cost(c), link(l) {};
};

struct vertex
{
	edge* adj;

	vertex(edge* a = nullptr) :adj(a) {};
};

class graph
{
public:
	graph();

	int getWeight(int u, int v);
	void solution();

private:
	int n, m, s, t, q;
	vertex* web;
	vertex* rweb;
	int* dist;
	int* rdist;

	void dijkstra(int v);
	void r_dijkstra(int v);
};


int main()
{
	ios::sync_with_stdio(false);
	cin.tie(NULL);

	graph p;
	p.solution();

	return 0;
}

graph::graph()
{
	cin >> n >> m >> s >> t;

	dist = new int[n + 1];
	rdist = new int[n + 1];

	web = new vertex[n + 1];
	web[0].adj = nullptr;

	rweb = new vertex[n + 1];
	rweb[0].adj = nullptr;

	for (int i = 0; i < m; i++)
	{
		int u, v, w;
		cin >> u >> v >> w;

		edge* p = new edge(v, w);
		p->link = web[u].adj;
		web[u].adj = p;

		edge* q = new edge(u, w);
		q->link = rweb[v].adj;
		rweb[v].adj = q;
	}
}

void graph::dijkstra(int v)
{
	fill(dist, dist + n + 1, MAX);
	// fill(path, path + n + 1, -1);
	dist[v] = 0;

	MinHeap<HeapNode> heap(n * 2);
	HeapNode h(v, 0);
	heap.push(h);

	while (!heap.empty())
	{
		HeapNode cur = heap.top();
		heap.pop();

		int u = cur.vertex;
		int cur_dist = cur.distance;

		edge* e = web[u].adj;
		while (e)
		{
			int v = e->dest;
			int w = e->cost;
			if (dist[u] < MAX && dist[u] + w < dist[v])
			{
				dist[v] = dist[u] + w;
				// path[v] = u;
				HeapNode node(v, dist[v]);
				heap.push(node);
			}
			e = e->link;
		}
	}
}

void graph::r_dijkstra(int v)
{
	fill(rdist, rdist + n + 1, MAX);
	rdist[v] = 0;

	MinHeap<HeapNode> heap(n * 2);
	HeapNode h(v, 0);
	heap.push(h);

	while (!heap.empty())
	{
		HeapNode cur = heap.top();
		heap.pop();

		int u = cur.vertex;
		int cur_dist = cur.distance;

		edge* e = rweb[u].adj;
		while (e)
		{
			int v = e->dest;
			int w = e->cost;
			if (rdist[u] < MAX && rdist[u] + w < rdist[v])
			{
				rdist[v] = rdist[u] + w;
				HeapNode node(v, rdist[v]);
				heap.push(node);
			}
			e = e->link;
		}
	}
}

int graph::getWeight(int u, int v)
{
	edge* p = web[u].adj;
	while (p != nullptr && p->dest != v)
		p = p->link;

	if (p == nullptr)
		return -1;
	else
		return p->cost;
}

void graph::solution()
{
	dijkstra(s);
	r_dijkstra(t);

	cin >> q;
	for (int i = 0; i < q; i++)
	{
		int u, v, w;
		cin >> u >> v >> w;

		int ans = dist[t];
		if (dist[u] < MAX && rdist[v] < MAX && dist[u] + w + rdist[v] < ans)
			ans = dist[u] + w + rdist[v];

		if (ans == MAX)
			cout << "- 1\n";
		else
			cout << ans << "\n";
	}
}

template<class T>
MinHeap<T>::MinHeap(int sz)
{
	maxHeapSize = (DefaultSize < sz) ? sz : DefaultSize;
	heap = new T[maxHeapSize];

	if (heap == nullptr)
	{
		cerr << "Heap storage allocation failed!" << endl;
		return;
	}

	curSize = 0;
}

template<class T>
MinHeap<T>::MinHeap(T arr[], int n)
{
	maxHeapSize = (DefaultSize < n) ? n : DefaultSize;
	heap = new T[maxHeapSize];

	if (heap == nullptr)
	{
		cerr << "Heap storage allocation failed!" << endl;
		return;
	}

	for (int i = 0; i < n; i++)
		heap[i] = arr[i];
	curSize = n;

	int curPos = (curSize - 2) / 2;
	while (curPos >= 0)
	{
		siftDown(curPos, curSize - 1);
		curPos--;
	}
}

template<class T>
bool MinHeap<T>::push(T& x)
{
	if (curSize == maxHeapSize)
	{
		cerr << "Heap full!" << endl;
		return false;
	}

	heap[curSize] = x;
	siftUp(curSize);
	curSize++;
	return true;
}

template<class T>
bool MinHeap<T>::pop()
{
	if (empty())
	{
		cout << "Heap empty!" << endl;
		return false;
	}

	heap[0] = heap[curSize - 1];
	curSize--;
	siftDown(0, curSize - 1);
	return true;
}

template<class T>
bool MinHeap<T>::pop(T& x)
{
	if (empty())
	{
		cout << "Heap empty!" << endl;
		return false;
	}

	x = heap[0];
	heap[0] = heap[curSize - 1];
	curSize--;
	siftDown(0, curSize - 1);
	return true;
}

template<class T>
void MinHeap<T>::siftDown(int start, int m)
{
	int i = start;
	int j = 2 * i + 1;
	T temp = heap[i];
	while (j <= m)
	{
		if (j < m && heap[j] > heap[j + 1])
			j++;
		if (temp <= heap[j])
			break;
		else
		{
			heap[i] = heap[j];
			i = j;
			j = 2 * j + 1;
		}
	}
	heap[i] = temp;
}

template<class T>
void MinHeap<T>::siftUp(int start)
{
	int j = start;
	int i = (j - 1) / 2;
	T temp = heap[j];
	while (j > 0)
	{
		if (heap[i] <= temp)
			break;
		else
		{
			heap[j] = heap[i];
			j = i;
			i = (i - 1) / 2;
		}
	}
	heap[j] = temp;
}