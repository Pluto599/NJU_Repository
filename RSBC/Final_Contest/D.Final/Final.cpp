#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <functional>
#include <climits>
#include <cassert>
using namespace std;

static const int INF = (int)0x7fffffff;

struct Edge {
    int to, val;
    Edge *next = nullptr;
    Edge() {}
    Edge(int t, int v) : to(t), val(v) {}
};

struct Pair {
    int first;
    int second;

    // 构造函数
    Pair(int f = 0, int s = 0) : first(f), second(s) {}

    // 自定义小于运算，用于最小堆比较
    // 比较逻辑：优先比较 first，若相等则比较 second
    bool operator<(const Pair &other) const {
        if (first < other.first) {
            return true;
        } else if (first == other.first) {
            return second < other.second;
        } else {
            return false;
        }
    }
};

// 交换函数，不能使用 std::swap，自行实现
void mySwap(Pair &a, Pair &b) {
    Pair temp = a;
    a = b;
    b = temp;
}

class MinHeap {
public:
    MinHeap(int cap = 16) : capacity(cap), heapSize(0) { data = new Pair[capacity]; }
    ~MinHeap() { delete[] data; }
    void push(const Pair &val);
    void pop();
    Pair top() const { return data[0]; }
    bool empty() const { return (heapSize == 0); }
    int size() const { return heapSize; }
private:
    Pair*  data;
    int    capacity;  // 数组容量
    int    heapSize;  // 当前堆中元素数量

    void siftUp(int idx);
    void siftDown(int idx);
    void reserve();
};

void MinHeap::pop()
{
    if (empty()) return;
    // 将末尾元素放到堆顶，再下沉
    data[0] = data[heapSize - 1];
    heapSize--;
    if (!empty()) {
        siftDown(0);
    }
}

void MinHeap::push(const Pair &val)
{        // 如果空间不足，先扩容
    if (heapSize == capacity) {
        reserve();
    }
    // 放到末尾，然后上浮
    data[heapSize] = val;
    siftUp(heapSize);
    heapSize++;
}

void MinHeap::reserve()
{
    int newCapacity = capacity * 2;
    Pair* newData   = new Pair[newCapacity];
    // 旧数据拷贝
    for (int i = 0; i < heapSize; i++) {
        newData[i] = data[i];
    }
    // 释放旧空间
    delete[] data;
    data = newData;
    capacity = newCapacity;
}

void MinHeap::siftUp(int idx){
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        // 若当前节点比父节点更小，则交换
        if (data[idx] < data[parent]) {
            mySwap(data[idx], data[parent]);
            idx = parent;  // 继续向上
        } else {
            break;
        }
    }   
}

void MinHeap::siftDown(int idx){
    while (true) {
        int leftChild  = 2 * idx + 1;
        int rightChild = 2 * idx + 2;
        int smallest   = idx;

        // 在左右子节点中找更小的
        if (leftChild < heapSize && data[leftChild] < data[smallest]) {
            smallest = leftChild;
        }
        if (rightChild < heapSize && data[rightChild] < data[smallest]) {
            smallest = rightChild;
        }

        // 如果有更小的子节点，则交换并继续
        if (smallest != idx) {
            mySwap(data[idx], data[smallest]);
            idx = smallest;
        } else {
            break;  // 已满足最小堆性质
        }
    }
}

template <typename T>
class Vector {
public:
    Vector();
    Vector(int size, const T& value);
    Vector(const Vector<T> &other);

    // 拷贝赋值运算符
    Vector<T>& operator=(const Vector<T> &other)
    {
        if (this != &other) {
            delete[] _data;
            _size = other._size;
            _capacity = other._capacity;
            _data = new T[_capacity];
            std::copy(other._data, other._data + _size, _data);
        }
        return *this;
    }

    ~Vector();

    void resize(int size, const T& value);
    void resize(int size);

    void push_back(const T& value);
    void push_front(const T& value); // O(n)
    void pop_back();

    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    size_t size() const { return _size; }
    size_t capacity() const { return _capacity; }
    bool empty() const { return _size == 0; }

    void reserve(size_t new_capacity);
    const T &back() const;
    const T &front() const;

    void reverse() {
        std::reverse(_data, _data + _size);
    }

private:
    size_t _size;
    size_t _capacity;
    T* _data;
};

template <typename T>
Vector<T>::Vector() : _size(0), _capacity(1), _data(new T[_capacity]) {}

template <typename T>
inline Vector<T>::Vector(int size, const T &value)
{
    _size = size;
    _capacity = size;
    _data = new T[_capacity];
    std::fill(_data, _data + _size, value);
}

template <typename T>
inline Vector<T>::Vector(const Vector<T> &other)
{
    _size = other._size;
    _capacity = other._capacity;
    _data = new T[_capacity];
    std::copy(other._data, other._data + _size, _data);
}

template <typename T>
Vector<T>::~Vector() {
    delete[] _data;
}

template <typename T>
void Vector<T>::resize(int new_size, const T& value)
{
    if (new_size > (int)_capacity) {
        reserve(std::max(new_size, (int)_capacity * 2));
    }
    // 若 new_size > _size，需要填充 [ _size, new_size )
    for (int i = _size; i < new_size; i++) {
        _data[i] = value;
    }
    _size = new_size;
}

template <typename T>
void Vector<T>::resize(int new_size)
{
    if (new_size > (int)_capacity) {
        reserve(std::max(new_size, (int)_capacity * 2));
    }
    // 若 new_size > _size，则初始化
    for (int i = _size; i < new_size; i++) {
        _data[i] = T();
    }
    _size = new_size;
}

template <typename T>
void Vector<T>::push_back(const T& value) {
    if (_size >= _capacity) {
        reserve(_capacity * 2);
    }
    _data[_size++] = value;
}

template <typename T>
void Vector<T>::push_front(const T& value) {
    if (_size >= _capacity) {
        reserve(_capacity * 2);
    }
    // O(n) 移动
    for (int i = _size; i > 0; --i) {
        _data[i] = _data[i - 1];
    }
    _data[0] = value;
    ++_size;
}

template <typename T>
void Vector<T>::pop_back() {
    if (_size > 0) {
        --_size;
    }
}

template <typename T>
T& Vector<T>::operator[](size_t index) {
    if (index >= _size) {
        throw std::out_of_range("Index out of range");
    }
    return _data[index];
}

template <typename T>
const T& Vector<T>::operator[](size_t index) const {
    if (index >= _size) {
        throw std::out_of_range("Index out of range");
    }
    return _data[index];
}

template <typename T>
void Vector<T>::reserve(size_t new_capacity) {
    if (new_capacity > _capacity) {
        T* new_data = new T[new_capacity];
        // 拷贝旧的
        std::copy(_data, _data + _size, new_data);
        delete[] _data;
        _data = new_data;
        _capacity = new_capacity;
    }
}

template <typename T>
inline const T &Vector<T>::back() const
{
    if (_size == 0) {
        throw std::out_of_range("Vector is empty");
    }
    return _data[_size - 1];
}

template <typename T>
inline const T &Vector<T>::front() const
{
    if (_size == 0) {
        throw std::out_of_range("Vector is empty");
    }
    return _data[0];
}

class Final{
public:
    Final(int n, int m);
    void Solution();

private:
    /// @param size: 点数
    /// @param cnt: 边数
    /// @param P: 以太之滴数
    int cnt, size, P;
    int start, end;
    /// @brief 人物战斗力
    int Capacity;
    /// @brief 起始与终止点重要节点下标
    int SIDX, TIDX;

    /// @param nodeIndexOf: 重要节点序号i->实际岛屿编号 
    /// 重要节点最多 P+2 个，下标约定：
    ///  0..(P-1) --> 存放有以太之滴的岛屿
    ///  SIDX --> 起点 s（这里令 SIDX = P）
    ///  TIDX --> 终点 t（这里令 TIDX = P+1）
    /// @param demon: 恶魔战斗力
    /// @param drop: 以太之滴具体数量
    Vector<int> nodeIndexOf, demon, drop;

    /// @param dist: dist[i][v]: 从“重要节点 i”出发到达 实际岛屿v 的最短距离
    /// @param path: path[i][v]: 用于还原路径
    /// @param important_dist: 重要节点 i -> j 的最短距离
    Vector<Vector<int>> dist, path, important_dist;

    /// @param dp: dp[S][i]: 从起点到达“重要节点 i”并且经过集合 S 的最短距离
    ///            S 是一个位掩码，表示已经收集的以太之滴
    ///            i是当前所在的重要节点
    /// @param pre: pre[S][i]: 用于还原路径
    Vector<Vector<int>> dp, pre;
    /// @param drop_sum: drop_sum[S]: 集合 S 中的以太之滴总数
    Vector<int> drop_sum;

    /// @param Graph: 邻接表
    Vector<Edge *> Graph;

    void build();
    void dijkstra(int idx);
    void deal_dp();
    Vector<int> getpath(int src, int dst);
};

Final::Final(int n, int m){
    size = n;
    cnt = m;
    build();
}

void Final::build() {
    //初始化连接表
    cin >> start >> end;
    Graph.resize(size + 1, nullptr);
    for (int i = 0; i < cnt; i++) {
        int u, v, w;
        cin >> u >> v >> w;
        if(u == v) continue;
        Edge *e = new Edge(v, w);
        e->next = Graph[u];
        Graph[u] = e;
    }

    cin >> Capacity >> P;
    nodeIndexOf.resize(P + 2);
    demon.resize(P + 2);
    drop.resize(P + 2);
    dist.resize(P + 2, Vector<int>(size + 1, INF));
    path.resize(P + 2, Vector<int>(size + 1, -1));
    important_dist.resize(P + 2, Vector<int>(P + 2, INF));

    dp.resize(1 << P, Vector<int>(P + 2, INF));
    pre.resize(1 << P, Vector<int>(P + 2, -1));
    drop_sum.resize(1 << P, 0);

    SIDX = P;
    TIDX = P + 1;
    nodeIndexOf[SIDX] = start;
    nodeIndexOf[TIDX] = end;
    demon[SIDX] = 0;
    demon[TIDX] = 0;

    for (int i = 0; i < P; i++)
    {
        int z, q, y;
        cin >> z >> q >> y;
        nodeIndexOf[i] = z;
        drop[i] = q;
        demon[i] = y;
    }
}

/// @brief 堆优化Dijkstra
void Final::dijkstra(int idx) {
    int st = nodeIndexOf[idx];
    for (int v = 1; v <= size; v++)
    {
        dist[idx][v] = INF;
        path[idx][v] = -1;
    }
    dist[idx][st] = 0;

    //最小堆用于比较
    MinHeap pq(size + 1);
    pq.push(Pair(0, st));

    while(!pq.empty()){
        Pair topVal = pq.top();
        pq.pop();
        int cd = topVal.first;
        int u = topVal.second;

        if(cd > dist[idx][u]) continue;

        Edge *e = Graph[u];
        while(e != nullptr){
            int vv = e->to;
            int nd = cd + e->val;
            if(nd < dist[idx][vv]) {
                dist[idx][vv] = nd;
                path[idx][vv] = u;
                pq.push(Pair(nd, vv));
            }
            e = e->next;
        }
    }
}

/// @brief 状态压缩DP
void Final::deal_dp()
{
    for (int S = 0; S < (1 << P); S++)
    {
        for (int i = 0; i <= P; i++)  // i in [0..P-1], or SIDX
        {
            int curCost = dp[S][i];
            if(curCost == INF) continue;

            for (int k = 0; k < P; k++)
            {
                if ((S & (1 << k)) || (important_dist[i][k] >= INF)) continue;
                int CurCapcity = Capacity + drop_sum[S];
                int DemonCapacity = demon[k];
                int fightCost = max(100 - (CurCapcity - DemonCapacity), 0);
                int newCost = curCost + important_dist[i][k] + fightCost;
                int newS = S | (1 << k);

                if(newCost < dp[newS][k]){
                    dp[newS][k] = newCost;
                    pre[newS][k] = i;
                }
            }
        }
    }
}

/// @brief 还原从重要节点 src 到原图节点 dst 的最短路
Vector<int> Final::getpath(int src, int dst){
    if(dist[src][dst] == INF) return {};
    Vector<int> temp;
    int cur = dst;
    while(cur != -1) {
        temp.push_back(cur);
        if(cur == nodeIndexOf[src]) break;
        cur = path[src][cur];
    }
    if(!temp.empty() && temp.back() != nodeIndexOf[src]) return {};
    temp.reverse();
    return temp;
}

void Final::Solution(){
    int total_p = P + 2;
    for (int i = 0; i < total_p; i++)
        dijkstra(i);

    for (int i = 0; i < total_p; i++)
        for (int j = 0; j < total_p; j++)
            important_dist[i][j] = dist[i][ nodeIndexOf[j] ];

    dp[0][SIDX] = 0;
    for (int S = 0; S < (1 << P); S++)
    {
        int sdrop = 0;
        for (int k = 0; k < P; k++){
            if(S & (1 << k)) {
                sdrop += drop[k];
            }
        }
        drop_sum[S] = sdrop;
    }

    deal_dp();

    long long ans = INF;
    int bestLast = -1;
    int done = (1 << P) - 1;
    for (int i = 0; i < P; i++){
        if (dp[done][i] >= INF || important_dist[i][TIDX] >= INF) continue;
        long long tot = dp[done][i] + important_dist[i][TIDX];
        if(tot < ans){
            ans = tot;
            bestLast = i;
        }
    }

    if(ans == INF){
        cout << -1 << "\n";
        return;
    }

    // 还原重要节点访问顺序
    Vector<int> keyRoute;
    int S = done;
    int i = bestLast;
    keyRoute.push_back(i);
    while(S){
        int prev = pre[S][i];
        if(prev == -1) break;
        S = S & ~(1 << i);
        keyRoute.push_back(prev);
        i = prev;
    }
    keyRoute.reverse();
    if(!keyRoute.empty() && keyRoute.front() != SIDX)
        keyRoute.push_front(SIDX);

    // 还原完整路径
    Vector<int> fullPath;
    for (int idx = 0; idx < (int)keyRoute.size() - 1; idx++){
        int src = keyRoute[idx];
        int dst = keyRoute[idx+1];
        auto segment = getpath(src, nodeIndexOf[dst]);
        if(segment.empty()){
            cout << -1 << "\n";
            return;
        }
        if(fullPath.empty()){
            fullPath = segment; 
        } else {
            // 去掉首元素避免重复
            for (int i = 1; i < (int)segment.size(); i++)
                fullPath.push_back(segment[i]);
        }
    }

    // 最后从 keyRoute.back() -> TIDX
    auto segment = getpath(keyRoute.back(), nodeIndexOf[TIDX]);
    if(segment.empty()){
        cout << -1 << "\n";
        return;
    }
    if(fullPath.empty()) {
        fullPath = segment;
    } else {
        for (int i = 1; i < (int)segment.size(); i++)
            fullPath.push_back(segment[i]);
    }

    // 输出
    for (int i = 0; i < (int)fullPath.size(); i++){
        cout << fullPath[i] << ( (i+1 < (int)fullPath.size()) ? ' ' : '\n' );
    }
    cout << ans << "\n";
}

int main(){
    int n, m;
    cin >> n >> m;
    Final f(n, m);
    f.Solution();
    return 0;
}
