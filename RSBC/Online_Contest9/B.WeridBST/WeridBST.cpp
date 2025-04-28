#include<iostream>
#include<cassert>
using namespace std;

template <class T>
struct TreeNode {
    T val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

template <class T>
class BST{
public:
    BST() { t_min = t_max = root = nullptr; }
    BST(int sz);
    bool insert(const T &x);
    const int size() { return _size; }

private:
    bool insert(TreeNode<T> *&cur, const T &x);
    TreeNode<T> *root, *t_min, *t_max;
    int _size;
};

template <class T>
BST<T>::BST(int sz) : _size(sz), t_min(nullptr), t_max(nullptr)
{
    int val;
    root = nullptr;
    while (sz > 0)
    {
        cin >> val;
        assert(insert(val));
        sz--;
    }
}

template <class T>
bool BST<T>::insert(const T &x)
{
    if (t_min && x < t_min->val){
        insert(t_min, x);
        t_min = t_min->left;
        return true;
    }
    else if (t_max && x > t_max->val){
        insert(t_max, x);
        t_max = t_max->right;
        return true;
    }
    else
        return insert(root, x);
}

template <class T>
bool BST<T>::insert(TreeNode<T> *&cur, const T &x)
{
    if(cur==nullptr)
    {
        cur = new TreeNode<T>(x);
        _size++;
        if (root == cur)
            t_min = t_max = root;
        return true;
    }
    
    if (cur->val < x)
    {
        bool r = (cur->right == nullptr);
        bool in = insert(cur->right, x);
        if (in && r)
            cout << cur->val << ' ';
        return in;
    }
    else if (cur->val > x)
    {
        bool l = (cur->left == nullptr);
        bool in = insert(cur->left, x);
        if (in && l)
            cout << cur->val << ' ';
        return in;
    }
    return false;
}

int main(){
    int n;
    cin >> n;
    BST<int> tree(n);
    return 0;
}