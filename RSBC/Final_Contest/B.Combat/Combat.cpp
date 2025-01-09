#include <iostream>
#include <climits>
#include <cassert>
#include <stdexcept>
using namespace std;

const unsigned int int_size = 10;

template <typename T>
class Vector {
public:
    Vector();
    Vector(int size, const T& value);
    Vector(const Vector<T> &other);
    ~Vector();
    void push_back(const T& value);
    void pop_back();
    T& operator[](size_t index);
    const T& operator[](size_t index) const;
    size_t size() const { return _size; }
    size_t capacity() const { return _capacity; }
    bool empty() const { return _size == 0; }
    void reserve(size_t new_capacity);
    const T &back() const;

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
void Vector<T>::push_back(const T& value) {
    if (_size >= _capacity) {
        reserve(_capacity * 2);
    }
    _data[_size++] = value;
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

template<class T, class Compare = std::greater<T>>
class Heap
{
public:
    Heap(int sz = int_size);
    Heap(T* arr, size_t sz);
    void push(const T x);
    void pop();
    T top() { return *heap; }
    T* Heap_Sort();
    bool empty() { return _size == 0; }
    int size() { return _size; }

private:
    T* heap; // 堆元素存储数组
    int _size;     // 堆当前元素个数
    int _capacity;     // 堆最大容量
    Compare comp;        // 比较函数对象
    void siftDown(int start, int m); // 调整算法
    void siftUp(int start);          // 调整算法
    void reserve(size_t new_capacity);
};

template <class T, class Compare>
inline Heap<T, Compare>::Heap(int sz)
{
    _capacity = sz;
    heap = new T[_capacity];
    fill(heap, heap + _capacity, INT_MAX);
    _size = 0;
}

/// @brief 从数组建立堆、不更改原堆
/// @tparam T 数据类型
/// @param arr 数组
/// @param sz 数组大小
template <class T, class Compare>
inline Heap<T, Compare>::Heap(T* arr, size_t sz)
{
    _capacity = sz;
    _size = sz;
    heap=new T[_capacity];
    copy(arr, arr + _size, heap);
    for (int i = _size / 2; i >= 0; i--)
        siftDown(i, _size - 1);
}

template <class T, class Compare>
inline T* Heap<T, Compare>::Heap_Sort()
{
    for (int j = _size - 1; j > 0; j--)
    {
        siftDown(0, j);
        T temp = heap[0];
        heap[0] = heap[j];
        heap[j] = temp;
    }
    return heap;
}

template <class T, class Compare>
inline void Heap<T, Compare>::reserve(size_t new_capacity)
{
    if (new_capacity > _capacity) {
        T* new_data = new T[new_capacity];
        std::copy(heap, heap + _size, new_data);
        std::fill(new_data + _size, new_data + new_capacity, INT_MAX);
        delete[] heap;
        heap = new_data;
        _capacity = new_capacity;
    }
}

template <class T, class Compare>
inline void Heap<T, Compare>::push(const T x)
{
    if(_size >= _capacity)
        reserve(_capacity * 2);
    heap[_size] = x;
    siftUp(_size++);
}

template <class T, class Compare>
inline void Heap<T, Compare>::pop()
{
    assert(_size > 0);
    heap[0] = heap[--_size];
    siftDown(0, _size - 1);
}

template <class T, class Compare>
inline void Heap<T, Compare>::siftDown(int start, int m)
{
    int i = start, j = 2 * i + 1;
    T temp = heap[start];
    while (j <= m)
    {
        if (j < m && comp(heap[j + 1], heap[j]))
            j++;
        if (comp(heap[j], temp))
        {
            heap[i] = heap[j];
            i = j;
            j = j * 2 + 1;
        }
        else
            break;
    }
    heap[i] = temp;
}

template <class T, class Compare>
inline void Heap<T, Compare>::siftUp(int start)
{
    int j = start, i = (j - 1) / 2;
    T temp = heap[start];
    while (j > 0)
    {
        if (comp(temp, heap[i]))
        {
            heap[j] = heap[i];
            j = i;
            i = (j - 1) / 2;
        }
        else
            break;
    }
    heap[j] = temp;
}

int main(){
    //参数准备
    int n, m, r, k, q;
    cin >> n >> m >> r >> k >> q;
    int *result = new int[n];
    for (int i = 0; i < n; i++){
        cin >> result[i];
    }

    Heap<int> sorted(n + 1);
    //处理并输出
    for (int i = 0; i < q; i++)
    {
        int lookup;
        cin >> lookup;
        int Nm = lookup / m + (lookup % m != 0);
        Vector<int> arr;
        while (sorted.size() < lookup)
            sorted.push(result[sorted.size()]);
        for(int j = 1; j < Nm; j++)
        {
            arr.push_back(sorted.top());
            sorted.pop();
        }

        Heap<int> Q(r + 1);
        int index = lookup - 1, sum = 0, cnt = 0;
        while (index >= 0 && Q.size() < r)
            Q.push(result[index--]);
        while (cnt < k && !Q.empty())
        {
            sum += Q.top();
            Q.pop();
            cnt++;
        }
        cout << (k * sorted.top() + sum) << ' ';
        while(!arr.empty())
        {
            sorted.push(arr.back());
            arr.pop_back();
        }
    }
    delete[] result;
    return 0;
}