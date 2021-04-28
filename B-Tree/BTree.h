#pragma once
#include <vector>
#include <iostream>

template <class T>
class BTree
{
private:
	size_t m_order;

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
	//if (node == nullptr)return;
	int n = node->keys.size();
	
	for (int i = 0; i < n; ++i)
	{
		if(node->isLeaf == false)
			m_inordine(out, node->children[i]);
		out << node->keys[i] << ' ';
	}

	if(node->isLeaf == false)
		m_inordine(out, node->children[n]);
}

template <class T>
void BTree<T>::InOrdine(std::ostream& out) const
{
	m_inordine(out, m_root);
}

template <class T>
void BTree<T>::Insert(const T& val)
{
	if (m_root == nullptr)
	{
		m_root = new Node();
		m_root->isLeaf = true;
		m_root->keys.push_back(val);
	}
	else
	{
		if (m_root->keys.size() == 2 * m_order - 1) // daca radacina are nr max de chei atunci fac split
		{
			Node* newRoot = new Node();
			newRoot->children.push_back(m_root);
			newRoot->isLeaf = false;

			m_root = newRoot;
		}
		m_insert(m_root, val);
	}
}

template <class T>
void BTree<T>::m_splitChild(Node*& parent, int childIdx)
{
	T median = parent->children[childIdx]->keys[m_order - 1]; // m_order - 1 e mijlocul dintre 0 si 2*m_order-1-1
}

template <class T>
void BTree<T>::m_insert(Node*& node, const T& val)
{
	if (node->isLeaf == true)
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
	else // daca nodul nu e frunza
	{
		int insertPos = 0;
		while (insertPos < node->keys.size() && node->keys[insertPos] > val)
			insertPos++;

		Node* child = node->children[insertPos];


		if (child->keys.size() == 2 * m_order - 1) // daca fiul e plin
		{
			T median = child->keys[m_order - 1];	// iau mediana din fiu
			Node* fiuDrMediana = child->children[m_order];
	
			Node* n = new Node();					// creez un fiu nou in care o sa pun jumatatea din dreapta a cheilor si a fiilor

			for (int i = m_order; i < 2 * m_order - 1; ++i)
			{
				n->keys.push_back(child->keys[i]);				// copiez cheile
				n->children.push_back(child->children[i]);		// copiez fii
			}
			n->children.push_back(child->children[2 * m_order - 1]); // copiez si fiul de la final de tot
			
			node->keys.insert(node->keys.begin() + insertPos, median); // inserez mediana inapoi in parinte inainte de poz insertPos
	
			node->children.insert(node->children.begin() + insertPos + 1, n); // inserez si fiul catre noul nod dupa poz insertPos

			child->keys.erase(child->keys.begin() + m_order - 1, child->keys.end());	// sterg jumatatea de chei pe care am pus-o in celalalt nod
			child->children.erase(child->children.begin() + m_order, child->children.end()); // sterg jumatatea de fii pe care am pus-o in celelalt nod
		}

		if (val <= node->keys[insertPos])
			m_insert(node->children[insertPos], val);
		else
			m_insert(node->children[insertPos+1], val);
	}
}