#pragma once
#include <vector>
#include <iostream>

template <class T>
class BTree
{
private:
	int m_order;

	struct Node {
		bool isLeaf = true;
		std::vector<T> keys;
		std::vector<Node*> children;
	};
	Node* m_root = nullptr;

	void m_splitChild(Node*&, int);
	void m_inordine(std::ostream&, const Node*) const;
	void m_insert(Node*& node, const T& val);

public: 
	BTree(int order = 2) : m_order(order) {}

	void Insert(const T& val);
	void InOrdine(std::ostream&) const;
};

template <class T> 
void BTree<T>::m_inordine(std::ostream& out, const Node* node) const
{
	int n = node->keys.size();
	for (int i = 0; i < n; ++i)
	{
		m_inordine(out, node->children[i]);
		out << node->keys[i];
	}
	m_inordine(node->children[i + 1]);
}

template <class T>
void BTree<T>::InOrdine(std::ostream& out) const
{
	m_inordine(out, m_root);
}

template <class T>
void BTree<T>::Insert(const T& val)
{
	if (root->keys.size() == 2 * m_order - 1) // daca radacina are nr max de chei atunci fac split
	{
		Node* newRoot = new Node();
		newRoot->children.push_back(root);
		newRoot->leaf = false;

		m_splitChild(newRoot, 0);
		root = newRoot;
	}
	m_insert(root, val);
}

template <class T>
void BTree<T>::m_splitChild(Node*& parent, int childIdx)
{
	T median = parent->children[childIdx][m_order - 1]; // m_order - 1 e mijlocul dintre 0 si 2*m_order-1-1
}

template <class T>
void BTree<T>::m_insert(Node*& node, const T& val)
{
	if (node->leaf == true)
	{
		node->keys.push_back(node->keys.back()); // dummy

		int i = node->keys.size() - 1;
		while (i > 0 && node->keys[i - 1] > val)
		{
			node->keys[i] = node->keys[i - 1];
			i--;
		}

		node->keys[i] = val;
	}
}