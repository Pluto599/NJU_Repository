#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <climits>

using namespace std;

class TarjanSCC {
public:
    TarjanSCC(int n) : n(n), adj(n), dfn(n, -1), low(n, -1), inStack(n, false), timestamp(0) {}

    void addEdge(int u, int v) {
        adj[u].push_back(v);
    }

    vector<vector<int>> findSCCs() {
        for (int i = 0; i < n; ++i) {
            if (dfn[i] == -1) {
                tarjan(i);
            }
        }
        return sccs;
    }

private:
    int n;
    vector<vector<int>> adj; // 邻接表
    vector<int> dfn, low;    // 时间戳数组
    vector<bool> inStack;    // 栈标记
    stack<int> st;           // 存储当前路径
    vector<vector<int>> sccs; // 强连通分量集合
    int timestamp;

    void tarjan(int u) {
        dfn[u] = low[u] = timestamp++;
        st.push(u);
        inStack[u] = true;

        for (int v : adj[u]) {
            if (dfn[v] == -1) { // 未访问过
                tarjan(v);
                low[u] = min(low[u], low[v]);
            } else if (inStack[v]) { // 在栈中，形成回路
                low[u] = min(low[u], dfn[v]);
            }
        }

        // 找到一个强连通分量
        if (dfn[u] == low[u]) {
            vector<int> scc;
            while (true) {
                int node = st.top();
                st.pop();
                inStack[node] = false;
                scc.push_back(node);
                if (node == u) break;
            }
            sort(scc.begin(), scc.end()); // 字典序排序
            sccs.push_back(scc);
        }
    }
};

int main() {
    int n, m;
    cin >> n >> m;

    TarjanSCC tarjan(n);
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        tarjan.addEdge(u, v);
    }

    vector<vector<int>> sccs = tarjan.findSCCs();

    // 找到最大强连通分量
    vector<int> largestSCC;
    for (const auto& scc : sccs) {
        if (scc.size() > largestSCC.size()) {
            largestSCC = scc;
        } else if (scc.size() == largestSCC.size() && scc < largestSCC) {
            largestSCC = scc; // 比较字典序
        }
    }

    // 输出结果
    for (int i = 0; i < largestSCC.size(); ++i) {
        if (i > 0) cout << " ";
        cout << largestSCC[i];
    }

    return 0;
}
