#include<iostream>
using namespace std;

/// @brief 节点类
struct Node
{
    /// @param light 点亮状态
    bool light = true;
    int FirstChild = -1;
    int SecondChild = -1;
    /// @brief 按左->右顺序添加子树
    /// @param n 子树索引
    void push(int n){
        if (FirstChild == -1)
            FirstChild = n;
        else
            SecondChild = n;
    }
    /// @brief 判断是否为叶子节点 
    bool isleaf() { return FirstChild == -1 && SecondChild == -1; }
};

/// @brief 主递归函数
/// @param candle 存储蜡烛电量状态数组
/// @param status 记录对该节点的操作是点亮还是熄灭
/// @param cur 当前处理节点
/// @return 处理所需步数
int LightUp(Node *candle, int cur, bool status)
{
    if(candle[cur].isleaf()) return candle[cur].light != status;    //节点末尾 递归结束标志
    int cnt = 0;
    int l = candle[cur].FirstChild, r = candle[cur].SecondChild;

    if (candle[cur].light == status)
    {
        //当前状态与要求状态一致
        //子树保持与当前状态一致
        if (l != -1) cnt += LightUp(candle, l, status);
        if (r != -1) cnt += LightUp(candle, r, status);
    }
    else
    {
        //当前状态与需求状态相反
        //子树均需变为相反状态
        cnt++;
        if (l != -1) cnt += LightUp(candle, l, !status);
        if (r != -1) cnt += LightUp(candle, r, !status);
    }
    return cnt;
}

int main(){
    int n, m;
    cin >> n >> m;
    //完全二叉树使用数组存储
    Node *candle = new Node[n + 1];
    //构建二叉树    
    for (int i = 2; i <= n; i++)
    {
        int par;
        cin >> par;
        candle[par].push(i);
    }
    //熄灭相应蜡烛
    for (int i = 0; i < m; i++)
    {
        int no;
        cin >> no;
        candle[no].light = false;
    }
    cout << LightUp(candle, 1, true);
    return 0;
}