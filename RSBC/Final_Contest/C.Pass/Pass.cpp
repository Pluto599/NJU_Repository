#include <iostream>
#include <cstring>
using namespace std;

const int default_size = 0x7fffffff;

// 边结构体
struct Edge {
    int to, val;
    Edge *next = nullptr;
    Edge() {}
    Edge(int t, int v) : to(t), val(v) {}
};

// 节点结构体
struct pa {
    int node;
    int val;
    pa(int n = 0, int v = 0) : node(n), val(v) {}
};

// 自定义最小堆
class MinHeap {
public:
    pa* heap;
    int size;
    int capacity;

    MinHeap(int cap) : size(0), capacity(cap) {
        heap = new pa[capacity + 1];
    }

    ~MinHeap() {
        delete[] heap;
    }

    bool empty() const {
        return size == 0;
    }

    void push(const pa& p) {
        if (size >= capacity) return;
        heap[size] = p;
        siftUp(size);
        size++;
    }

    pa pop() {
        pa top = heap[0];
        heap[0] = heap[size - 1];
        size--;
        siftDown(0);
        return top;
    }

private:
    void siftUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (heap[idx].val < heap[parent].val) {
                swap(heap[idx], heap[parent]);
                idx = parent;
            }
            else break;
        }
    }

    void siftDown(int idx) {
        while (true) {
            int smallest = idx;
            int left = 2 * idx + 1;
            int right = 2 * idx + 2;
            if (left < size && heap[left].val < heap[smallest].val)
                smallest = left;
            if (right < size && heap[right].val < heap[smallest].val)
                smallest = right;
            if (smallest != idx) {
                swap(heap[idx], heap[smallest]);
                idx = smallest;
            }
            else break;
        }
    }
};

// 构建图
void build(int cnt, Edge** Graph, Edge** rGraph) {
    for (int i = 0; i < cnt; i++) {
        int u, v, w;
        cin >> u >> v >> w;
        if(u == v) continue;
        Edge *e = new Edge(v, w);
        e->next = Graph[u];
        Graph[u] = e;
        Edge *re = new Edge(u, w);
        re->next = rGraph[v];
        rGraph[v] = re;
    }
}

// Dijkstra算法
int* Dijkstra(int start, int size, Edge **adjList, bool *inPath, int *dist) {
    // 预设堆容量为size的两倍
    MinHeap Q(size * 2);
    fill(dist, dist + size, default_size);
    fill(inPath, inPath + size, false);
    dist[start] = 0;
    Q.push(pa(start, 0));

    while (!Q.empty()) {
        pa current = Q.pop();
        int u = current.node;
        if (inPath[u]) continue;
        inPath[u] = true;
        Edge *currentEdge = adjList[u];
        while (currentEdge) {
            int v = currentEdge->to;
            int val = currentEdge->val;
            if (!inPath[v] && dist[u] + val < dist[v]) {
                dist[v] = dist[u] + val;
                Q.push(pa(v, dist[v]));
            }
            currentEdge = currentEdge->next;
        }
    }
    return dist;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(NULL);
    int n, m, s, t;
    cin >> n >> m >> s >> t;
    Edge **Graph = new Edge*[n + 1];
    Edge **rGraph = new Edge*[n + 1];
    memset(Graph, 0, sizeof(Edge*) * (n + 1));
    memset(rGraph, 0, sizeof(Edge*) * (n + 1));
    build(m, Graph, rGraph);

    bool *inPath = new bool[n + 1];
    int *dist = new int[n + 1];
    int *rdist = new int[n + 1];

    Dijkstra(s, n + 1, Graph, inPath, dist);
    Dijkstra(t, n + 1, rGraph, inPath, rdist);

    int direct = dist[t];
    int q;
    cin >> q;
    for (int i = 0; i < q; i++) {
        int u, v, w;
        cin >> u >> v >> w;
        if (dist[u] == default_size || rdist[v] == default_size || dist[u] + w + rdist[v] >= direct)
            cout << ((direct == default_size) ? -1 : direct) << "\n";
        else
            cout << dist[u] + w + rdist[v] << "\n";
    }

    // 释放内存
    for (int i = 0; i <= n; i++) {
        Edge *current = Graph[i];
        while (current) {
            Edge *temp = current;
            current = current->next;
            delete temp;
        }
        current = rGraph[i];
        while (current) {
            Edge *temp = current;
            current = current->next;
            delete temp;
        }
    }
    delete[] inPath;
    delete[] dist;
    delete[] rdist;
    delete[] Graph;
    delete[] rGraph;
    return 0;
}
