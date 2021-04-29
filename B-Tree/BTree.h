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
	void m_inordine(std::vector<T>&, const Node*) const;
	void m_insert(Node*& node, const T& val);

public: 
	BTree(int order = 2) : m_order(order) {}
	void Insert(const T& val);
	std::vector<T> InOrdine() const;
};

template <class T> 
void BTree<T>::m_inordine(std::vector<T>& vals, const Node* node) const
{
	if (node == nullptr)return;

	size_t n = node->keys.size();
	
	for (int i = 0; i < n; ++i)
	{
		// ma duc recursiv intr-un fiu si dupa retin cheia curenta
		// la sfarsit o sa fie un fiu in care o sa ma duc separat

		if(node->isLeaf == false)
			m_inordine(vals, node->children[i]); 
		
		vals.push_back(node->keys[i]);
	}

	if(node->isLeaf == false) 
		m_inordine(vals, node->children[n]); // iau si ultimul fiu
}

template <class T>
std::vector<T> BTree<T>::InOrdine() const
{
	std::vector<T> vals;
	m_inordine(vals, m_root);
	return vals;
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
			Node* newRoot = new Node();				// creez noua radacina
			newRoot->children.push_back(m_root);    // ii dau ca fiu radacina curenta
			newRoot->isLeaf = false;			    // noua radacina nu o sa fie frunza
			m_splitChild(newRoot, 0);				// dau split la radacina

			m_root = newRoot;						// radacina e acum noua radacina

		 
		}
		m_insert(m_root, val);
	}
}

template <class T>
void BTree<T>::m_splitChild(Node*& parent, int childIdx)
{
	// desparte fiul de la childIdx care e plin in 2 noduri
	// jumatatea din stanga o sa ramana fiul original
	// jumatatea din dreapta o sa fie inserat in parinte inainte de fiul de la childIdx + 1 (dupa fiul original)
	// in parinte o sa se introduca mediana din fiu

	Node* child = parent->children[childIdx];

	// iau mediana din fiu, m_order - 1 e mijlocul dintre 0 si 2*m_order-2  
	T median = child->keys[m_order - 1]; 

	Node* newNode = new Node();
	newNode->isLeaf = child->isLeaf;

	for (int i = m_order; i < 2*m_order-1; ++i)
	{
		newNode->keys.push_back(child->keys[i]);			// pun cheile din jumatate dreapta in n
		if(newNode->isLeaf == false)
		{
			newNode->children.push_back(child->children[i]);	// pun fii din jumatatea dreapta in n
		}
	}
	
	if(newNode->isLeaf == false)
	{
		newNode->children.push_back(child->children[2 * m_order - 1]); // pun si ultimul fiu care e la 2*m_order-1
	}

	// sterg jumatatea de chei din fiu pe care le am pus in celalalt nod
	child->keys.erase(child->keys.begin() + m_order - 1, child->keys.end()); 

	// sterg jumatatea de fii din fiu pe care i am pus in celalalt nod
	// nu trebuie sters fiul din stanga medianei si din cauza asta incep de la jumatate+1 (m_order)
	if(child->isLeaf == false)
		child->children.erase(child->children.begin() + m_order, child->children.end());

	// pun mediana in parinte
	parent->keys.insert(parent->keys.begin() + childIdx, median);

	// legatura cu fiul din stanga a ramas mai trebuie legatura cu noul fiu creat
	// trebuie inserat dupa fiul din stanga adica inainte de childIdx + 1
	parent->children.insert(parent->children.begin() + childIdx + 1, newNode);
}

template <class T>
void BTree<T>::m_insert(Node*& node, const T& val)
{
	if (node->isLeaf == true)
	{
		// pun valoare pe ultima poz si fac insertion sort
		node->keys.push_back(val);

		size_t i = node->keys.size() - 1;
		while (i > 0 && node->keys[i - 1] > val)
		{
			node->keys[i] = node->keys[i - 1];
			i--;
		}
		node->keys[i] = val;
	}
	else // daca nodul nu e frunza
	{
		size_t insertPos = 0;
		while (insertPos < node->keys.size() && val > node->keys[insertPos])
			insertPos++;
		
		// valoare trebuie inserata la stanga de cheia de la insertPos (daca exista)
		// adica in fiul de la insertPos

		Node* child = node->children[insertPos];

		if (child->keys.size() == 2 * m_order - 1) // daca fiul e plin
		{ 
			m_splitChild(node, insertPos); 
			// cheia de la insertPos = mediana din fiu
			// fiul de la insertPos = jumatatea stanga din fiul original
			// fiul de la insertPos+1 = jumatatea dreapta din fiul original

			if (val <= node->keys[insertPos])
				m_insert(node->children[insertPos], val);
			else
				m_insert(node->children[insertPos + 1], val);
		}
		else // daca fiul nu e plin
		{
			m_insert(child, val);
		}

	}
}