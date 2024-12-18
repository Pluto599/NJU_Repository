#include<iostream>
#include<vector>
using namespace std;

struct Train
{
    int price;
    int dest;
    Train *next;
    Train(int d, int p, Train *n = nullptr) : price(p), dest(d) { next = n; }
};

class Station{
public:
    Station() {  }
    Station(int n, int tran)
    {
        size_ = n;
        tran_max = tran;
        adjList.resize(n, nullptr);
    }
    ~Station(){
        for (int i = 0; i < size_; i++)
        {
            Train *cur = adjList[i];
            while (cur)
            {
                Train *temp = cur;
                cur = cur->next;
                delete temp;
            }
        }
    }
    void build(int sz);
    int solution(int school, int home);

private : 
    const int default_size = 2147483647;
    vector<Train*> adjList;
    int tran_max, size_;
};

int Station::solution(int school, int home)
{
    vector<int> cost(size_, default_size);
    cost[school] = 0;
    for (int cnt = 0; cnt <= tran_max; cnt++)
    {
        vector<int> pre(cost);
        for (int i = 0; i < size_; i++)
            if (cost[i] != default_size && cost[i] < cost[home])
            {
                Train *cur = adjList[i];
                while (cur)
                {
                    pre[cur->dest] = min(cost[i] + cur->price, pre[cur->dest]);
                    cur = cur->next;
                }
            }
        cost = pre;
    }
    if (cost[home] == default_size)
        cost[home] = -1;
    return cost[home];
}

void Station::build(int sz){
    int s, e, p;
    while (sz > 0)
    {
        cin >> s >> e >> p;
        adjList[s] = new Train(e, p, adjList[s]);
        sz--;
    }
}

int main(){
    int school, home, k;
    cin >> school >> home >> k;
    int n, m;
    cin >> n >> m;
    Station S(n, k);
    S.build(m);
    cout << S.solution(school, home);
    return 0;
}