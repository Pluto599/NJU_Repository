#include<iostream>
using namespace std;
#include<cassert>

template <class T>
struct ThreadNode
{
    int ltag, rtag;
    ThreadNode<T>* leftChild, * rightChild;
    T data;
    ThreadNode(const T item) : data(item), leftChild(nullptr), rightChild(nullptr), ltag(0), rtag(0) {}
};

template <class T>
class ThreadTree
{
protected:
    ThreadNode<T>* root;
    T RefValue;

    void CreateTree(istream& in, ThreadNode<T>*& subTree);
    void createInThread(ThreadNode<T>* current, ThreadNode<T>*& pre);

    void InOrder(ThreadNode<T>* subTree);

public:
    ThreadTree() : root(nullptr) {}
    ThreadTree(T value) :RefValue(value), root(nullptr) {}

    void CreateTree(istream& in) { CreateTree(in, root); }
    void createInThread();

    void InOrder() { InOrder(root); }
};

template<class T>
void ThreadTree<T>::CreateTree(istream& in, ThreadNode<T>*& subTree)
{
    T item;
    if (!in.eof())
    {
        in >> item;

        if (item != RefValue)
        {
            subTree = new ThreadNode<T>(item);
            if (subTree == nullptr)
            {
                cerr << "´æ´¢·ÖÅä´í!" << endl;
                exit(1);
            }

            CreateTree(in, subTree->leftChild);
            CreateTree(in, subTree->rightChild);
        }
        else
            subTree = nullptr;
    }
}

template <class T>
void ThreadTree<T>::createInThread()
{
    ThreadNode<T>* pre = nullptr;
    if (root != nullptr)
    {
        createInThread(root, pre);
        // pre = last, set last
        pre->rightChild = nullptr;
        pre->rtag = 1;
    }
}

template <class T>
void ThreadTree<T>::createInThread(ThreadNode<T>* current, ThreadNode<T>*& pre)
{
    if (current == nullptr)
        return;

    createInThread(current->leftChild, pre);

    if (current->leftChild == nullptr)
    {
        current->leftChild = pre;
        current->ltag = 1;
    }
    if (pre != nullptr && pre->rightChild == nullptr)
    {
        pre->rightChild = current;
        pre->rtag = 1;
    }

    pre = current;
    createInThread(current->rightChild, pre);
}

template<class T>
void ThreadTree<T>::InOrder(ThreadNode<T>* subTree)
{
    if (subTree != nullptr)
    {
        if (subTree->ltag == 0)
            InOrder(subTree->leftChild);

        cout << subTree->ltag << ' ';
        if (subTree->leftChild != nullptr)
            cout << subTree->leftChild->data << ' ';
        else
            cout << RefValue << ' ';
        cout << subTree->data << ' ';
        if (subTree->rightChild != nullptr)
            cout << subTree->rightChild->data << ' ';
        else
            cout << RefValue << ' ';
        cout << subTree->rtag << endl;

        if (subTree->rtag == 0)
            InOrder(subTree->rightChild);
    }
}

int main()
{
    ThreadTree<char> tree('0');
    tree.CreateTree(cin);
    tree.createInThread();
    tree.InOrder();

    return 0;
}