#include<iostream>
#include<vector>
#include<algorithm>
#include<stack>

using namespace std;

struct edge {
    int dest;
    edge* link;
    edge(int d) : dest(d), link(nullptr) {}
};

struct vertex {
    edge* adj;
};

class graph {
private:
    vertex* web;
    int n, m;
    vector<int> dfn, low, inStack;
    stack<int> stk;
    vector<vector<int>> strongC;
    void dfs(int v);

public:
    graph();
    void solution();
};

int main() {
    graph p;
    p.solution();
    return 0;
}

graph::graph() {
    cin >> n >> m;
    web = new vertex[n];
    for (int i = 0; i < n; i++) {
        web[i].adj = nullptr;
    }

    int u, v;
    for (int i = 0; i < m; i++) {
        cin >> u >> v;
        edge* p = new edge(v);
        p->link = web[u].adj;
        web[u].adj = p;
    }

    dfn.assign(n, -1);
    low.assign(n, -1);
    inStack.assign(n, 0);
}

void graph::dfs(int v) {
    static int time = 0;
    dfn[v] = low[v] = time++;
    stk.push(v);
    inStack[v] = 1;

    edge* p = web[v].adj;
    while (p != nullptr) {
        int w = p->dest;
        if (dfn[w] == -1) {
            dfs(w);
            low[v] = min(low[v], low[w]);
        }
        else if (inStack[w]) {
            low[v] = min(low[v], dfn[w]);
        }
        p = p->link;
    }

    if (dfn[v] == low[v]) {
        vector<int> component;
        while (true) {
            int u = stk.top();
            stk.pop();
            inStack[u] = 0;
            component.push_back(u);
            if (u == v) break;
        }
        strongC.push_back(component);
    }
}

void graph::solution() {
    for (int i = 0; i < n; i++) {
        if (dfn[i] == -1) {
            dfs(i);
        }
    }

    vector<int> largestComponent;
    for (auto& component : strongC) {
        if (component.size() > largestComponent.size()) {
            largestComponent = component;
        }
        else if (component.size() == largestComponent.size()) {
            sort(component.begin(), component.end());
            sort(largestComponent.begin(), largestComponent.end());
            if (component < largestComponent) {
                largestComponent = component;
            }
        }
    }

    sort(largestComponent.begin(), largestComponent.end());
    for (int i = 0; i < largestComponent.size(); i++) {
        if (i != 0) cout << " ";
        cout << largestComponent[i];
    }
    cout << endl;
}
