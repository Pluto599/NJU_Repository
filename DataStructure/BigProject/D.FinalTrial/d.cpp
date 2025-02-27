#include <iostream>
using namespace std;

#define MAX 2147483647
const int DefaultSize = 128;

template<class T>
class MinHeap {
public:
    MinHeap(int sz = DefaultSize);
    MinHeap(T arr[], int n);
    ~MinHeap() { delete[] heap; }
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

    bool operator<(const HeapNode& h) const {
        return distance < h.distance;
    }

    bool operator<=(const HeapNode& h) const {
        return distance <= h.distance;
    }

    bool operator>(const HeapNode& h) const {
        return distance > h.distance;
    }

    bool operator>=(const HeapNode& h) const {
        return distance >= h.distance;
    }
};

struct EtherDrops {
    int index;
    int num;
    int demon;

    EtherDrops(int i = 0, int n = 0, int d = 0) : index(i), num(n), demon(d) {};
};

struct edge {
    int dest;
    int cost;
    edge* link;

    edge(int d, int c, edge* l = nullptr) : dest(d), cost(c), link(l) {};
};

struct vertex {
    edge* adj;

    vertex(edge* a = nullptr) : adj(a) {};
};

class graph {
public:
    graph(int n, int m);

    void solution();

private:
    int n, m, s, t, x0, p;
    vertex* web;
    int* dist;
    EtherDrops* etherInfo;

    void dijkstra(int v);
    int fightTime(int x, int y) {
        return max(100 - (x - y), 0);
    }
};


graph::graph(int n, int m) : n(n), m(m) {
    web = new vertex[n + 1];
    dist = new int[n + 1];
    for (int i = 0; i <= n; ++i) {
        dist[i] = MAX;
    }
    etherInfo = nullptr;
}

void graph::solution() {
    cin >> s >> t;
    cin >> x0 >> p;

    etherInfo = new EtherDrops[p];
    for (int i = 0; i < p; ++i) {
        int idx, num, demon;
        cin >> idx >> num >> demon;
        etherInfo[i] = EtherDrops(idx, num, demon);
    }

    for (int i = 0; i < m; ++i) {
        int u, v, w;
        cin >> u >> v >> w;
        web[u].adj = new edge(v, w, web[u].adj);
    }

    dijkstra(s);

    if (dist[t] == MAX) {
        cout << -1 << endl;
        return;
    }
    else
        cout << dist[t] << endl;
}

void graph::dijkstra(int v) {
    MinHeap<HeapNode> pq;
    dist[v] = 0;
    HeapNode h(v, 0);
    pq.push(h);

    while (!pq.empty()) {
        HeapNode node = pq.top();
        pq.pop();
        int u = node.vertex;
        int currDist = node.distance;

        if (currDist > dist[u]) continue;

        for (edge* e = web[u].adj; e != nullptr; e = e->link) {
            int newDist = currDist + e->cost;
            if (newDist < dist[e->dest]) {
                dist[e->dest] = newDist;
                HeapNode h(e->dest, newDist);
                pq.push(h);
            }
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m;
    cin >> n >> m;
    graph g(n, m);
    g.solution();
    return 0;
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