#include<cstring>
#include<iostream>
#include<vector>
using namespace std;

const int DefaultSize = 10;

class UFSets{
public:
    UFSets(int sz = DefaultSize);
    ~UFSets() { delete[] parent; }
    int Find(int x);
    void WeightedUnion(int R1, int R2);
    int CollapsingFind(int i);
    int *parent;
private:
    int size;
};

UFSets::UFSets(int sz) : size(sz)
{
    parent = new int[size];
    memset(parent, -1, sizeof(int) * size);
}

/// @brief 返回集合x的根
/// @param x 集合
/// @return 根
int UFSets::Find(int x){
    while (parent[x] >= 0)
        x = parent[x];
    return x;
}

void UFSets::WeightedUnion(int R1,int R2){
    int r1 = Find(R1), r2 = Find(R2), temp;
    if (r1 == r2)
        return;
    else
    {
        temp = parent[r1] + parent[r2];
        if (parent[r2] < parent[r1])
        {
            parent[r1] = r2;
            parent[r2] = temp;
        }
        else
        {
            parent[r1] = temp;
            parent[r2] = r1;
        }
    }
}

/// @brief 按照折叠规则压缩路径
/// @return 节点i的根节点
int UFSets::CollapsingFind(int i){
    //令j为根i
    int j = Find(i);
    //向上逐层压缩
    while (i != j){
        int temp = parent[i];
        parent[i] = j;
        i = temp;
    }
    return j;
}

int main(){
    int n;
    cin >> n;
    UFSets M(n * n);
    vector<bool> S(n * n, false);
    for (int i = 0; i < n * n; i++)
    {
        int flag;
        cin >> flag;
        if (flag)
        {
            S[i] = true;
            int row = i / n, col = i % n;
            if (row > 0 && S[i - n])
                M.WeightedUnion(i, i - n);
            if (col > 0 && S[i - 1])
                M.WeightedUnion(i, i - 1);
        }
    }

    int ans = -1;
    for(int i = 0; i < n * n; i++)
        if (!S[i])
        {
            int row = i / n, col = i % n;
            int cnt = 0;
            int up = -1, down = -1, left = -1, right = -1;

            if (row > 0 && S[i - n]) up = M.Find(i - n);
            if (col > 0 && S[i - 1]) left = M.Find(i - 1);
            if (row < n - 1 && S[i + n]) down = M.Find(i + n);
            if (col < n - 1 && S[i + 1]) right = M.Find(i + 1);

            if (up != -1)
                cnt -= M.parent[up];
            if (down != -1 && down != up)
                cnt -= M.parent[down];
            if (left != -1 && left != up && left != down)
                cnt -= M.parent[left];
            if (right != -1 && right != up && right != down && right != left)
                cnt -= M.parent[right];

            ans = max(ans, cnt + 1);
        }
    cout << ans;
    return 0;
}