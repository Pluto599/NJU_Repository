#include<iostream>
#include<vector>
#include<queue>
#include<assert.h>
using namespace std;

struct cmp
{
    bool operator()(pair<int, int> &a, pair<int, int> &b)
    {
        return a.first > b.first;
    }
};
using MyQueue = priority_queue<pair<int, int>, vector<pair<int, int>>, cmp>;

struct Node{
    int to;
    int val;
    Node *next;
    Node(int d, int p, Node *n = nullptr) : val(p), to(d) { next = n; }
};

class Graph{
public:
    Graph() {  }
    ~Graph(){
        for (int i = 0; i < size_; i++)
        {
            Node *cur = adjList[i];
            while (cur)
            {
                Node *temp = cur;
                cur = cur->next;
                delete temp;
            }
        }
    }
    Graph(int n){
        size_ = n;
        adjList.resize(n, nullptr);
        inDegree.resize(n, 0);
    }

    //构造n条边的带权有向图
    void build(int);

    //获取连接顶点方法
    Node *getNeighbor(int i) { return adjList[i]; }
    Node* getNeighbor(int i, int j){
        Node *cur = adjList[i];
        while (cur && cur->to != j)
            cur = cur->next;
        return cur == nullptr ? nullptr : cur->next;
    }

    //拓扑排序 AOV
    vector<int> TopologicalSort();
    //关键路径 AOE
    MyQueue CriticalPath();

private : 
    vector<Node *> adjList;
    vector<int> inDegree;   //拓扑排序使用的入度
    int size_;  //节点数量
    int edge_size;
    const int default_size = 2147483647;
};

void Graph::build(int sz)
{
    int s, e, p;
    edge_size = sz;
    while (sz > 0)
    {
        cin >> s >> e >> p;
        adjList[s] = new Node(e, p, adjList[s]);
        inDegree[e]++;
        sz--;
    }
}

vector<int> Graph::TopologicalSort()
{
    int top = -1, ans_cnt = 0;
    vector<int> count(inDegree);
    vector<int> ans(size_);
    for (int i = 0; i < size_; i++)
        if (count[i] == 0){
            count[i] = top;
            top = i;
        }

    for (int i = 0; i < size_; i++)
    {
        assert(top != -1);
        int v = top;
        top = count[top];
        ans[ans_cnt++] = v;
        Node* w = getNeighbor(v);
        while (w)
        {
            count[w->to]--;
            if (count[w->to] == 0)
            { count[w->to] = top; top = w->to; }
            w = w->next;
        }
    }

    return ans;
}

MyQueue Graph::CriticalPath()
{
    MyQueue ans;
    //获得拓扑序
    vector<int> order = TopologicalSort();
    vector<int> Ve(size_, 0), Vl(size_, default_size);

    //按拓扑序正向计算Ve
    for (int &i : order)
    {
        Node *j = getNeighbor(i);
        while (j)
        {
            Ve[j->to] = max(Ve[i] + j->val, Ve[j->to]);
            j = j->next;
        }
    }

    //逆向计算Vl
    Vl[size_ - 1] = Ve[size_ - 1];
    for (int i = size_ - 2; i > 0; i--)
    {
        int j = order[i];
        Node *k = getNeighbor(j);
        while (k)
        {
            Vl[j] = min(Vl[k->to] - k->val, Vl[j]);
            k = k->next;
        }
    }

    //求各个活动的e,l
    for (int &i : order)
    {
        Node *j = getNeighbor(i);
        while (j)
        {
            int Ae = Ve[i], Al = Vl[j->to] - j->val;
            if (Ae == Al)
                ans.emplace(i, j->to);
            j = j->next;
        }
    }

    return ans;
}

int main(){
    int N, E;
    cin >> N >> E;
    Graph G(N);
    G.build(E);
    MyQueue Q = G.CriticalPath();
    while(!Q.empty())
    {
        pair<int, int> temp = Q.top();
        Q.pop();
        cout << temp.first << ' ' << temp.second << endl;
    }
    return 0;
}