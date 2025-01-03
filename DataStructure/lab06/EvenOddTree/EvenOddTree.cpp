#include"tree.hpp"

int main()
{
	int n;
	cin >> n;
	BinaryTree<int>* tree = new BinaryTree<int>(-1);
	tree->CreateBinTree(n);

	tree->Solution(n);

	return 0;
}