#include<iostream>
#include<queue>
using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

class Tree{
public:
    Tree(int n);
    long long int Crusade();
private:
    TreeNode *head = nullptr;
    long long int Crusade(TreeNode *root, bool attack);
};

long long int Tree::Crusade(){
    return Crusade(head, false);
}

long long int Tree::Crusade(TreeNode *root, bool attack)
{
    if (root == nullptr)
        return 0;

    //叶子结点
    if (root->left == nullptr && root->right == nullptr){
        if (attack)
            return 0;
        else
            return root->val;
    }

    long long int sum = 0;
    //中间结点
    if (attack || root->val == 0)
    {
        long long int l = 0, r = 0;
        if(root->left)
            l = Crusade(root->left, false);
        if(root->right)
            r = Crusade(root->right, false);
        sum = l + r;
    }
    else
    {
        long long int l1 = 0, l2 = 0;
        long long int r1 = 0, r2 = 0;
        long long int sum1, sum2;
        if(root->left){
            l1 = Crusade(root->left, false);
            l2 = Crusade(root->left, true);
        }
        if(root->right){
            r1 = Crusade(root->right, false);
            r2 = Crusade(root->right, true);
        }
        sum1 = l1 + r1;
        sum2 = l2 + r2 + root->val;
        sum = max(sum1, sum2);
    }
    return sum;
}

Tree::Tree(int n){
    queue<TreeNode *> S;
    int v;
    cin >> v;
    TreeNode *r = new TreeNode(v);
    head = r;
    S.push(r);

    for (int i = 1; i < n;)
    {
        int f, s;
        cin >> f;
        i++;
        TreeNode *first = nullptr, *second = nullptr;
        if (f != -1){
            first = new TreeNode(f);
            S.push(first);
        }
        if (i < n)
        {
            cin >> s;
            i++;
            if (s != -1)
            {
                second = new TreeNode(s);
                S.push(second);
            }
        }
        S.front()->left = first;
        S.front()->right = second;
        S.pop();
    }
}

int main(){
    int n;
    cin >> n;
    Tree tree(n);
    cout << tree.Crusade();
    return 0;
}