#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

class UnionSets {
private:
    vector<int> parent, size;

public:
    vector<vector<int>> components;

    UnionSets(int n) {
        parent.resize(n);
        size.resize(n, 1);
        for (int i = 0; i < n; i++) {
            parent[i] = i;
        }

        //
        components.resize(n);
    }

    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        components[parent[x]].push_back(x);
        return parent[x];
    }

    void unionSets(int x, int y) {
        int root1 = find(x);
        int root2 = find(y);
        if (root1 != root2) {
            if (size[root1] > size[root2]) {
                parent[root2] = root1;
                size[root1] += size[root2];

                //
                components[parent[root1]].insert(components[parent[root1]].end(), components[parent[root2]].begin(), components[parent[root2]].end());
                components[parent[root2]].clear();
            }
            else {
                parent[root1] = root2;
                size[root2] += size[root1];

                //
                components[parent[root2]].insert(components[parent[root2]].end(), components[parent[root1]].begin(), components[parent[root1]].end());
                components[parent[root1]].clear();
            }
        }
    }
};

class Graph {
private:
    int n, m;
    vector<vector<int>> adj;
    UnionSets u;
    vector<bool> removed;

public:
    Graph(int n, int m) : n(n), m(m), u(n), removed(n, false) {
        adj.resize(n);
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    int count() {

        for (int i = 0; i < n; i++) {
            if (!removed[i]) {
                for (int neighbor : adj[i]) {
                    if (!removed[neighbor]) {
                        u.unionSets(i, neighbor);
                    }
                }
            }
        }

        int components = 0;
        vector<bool> visited(n, false);
        for (int i = 0; i < n; i++) {
            if (!visited[i] && !removed[i]) {
                components++;
                for (auto a : u.components[u.find(i)])
                    visited[a] = true;
            }
        }
        return components;
    }

    void fall(int v) {
        removed[v] = true;

        for (int neighbor : adj[v]) {
            adj[neighbor].erase(remove(adj[neighbor].begin(), adj[neighbor].end(), v), adj[neighbor].end());
        }

        adj[v].clear();

    }
};

int main() {
    int n, m, k;
    cin >> n >> m;
    Graph g(n, m);

    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        g.addEdge(u, v);
    }

    cin >> k;

    for (int i = 0; i < k; i++) {
        cout << g.count() << ' ';
        int v;
        cin >> v;
        g.fall(v);
    }
    cout << g.count() << ' ';

    return 0;
}
