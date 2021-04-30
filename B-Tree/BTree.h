#pragma once
#include <vector>
#include <iostream>
#include <cmath>

template <class T>
class BTree
{
private:
	static size_t binSearch(const std::vector<T>&, const T& val);

	unsigned m_order;
	
	struct Node {
		bool isLeaf = true;
		std::vector<T> keys;
		std::vector<Node*> children;
	};
	Node* m_root = nullptr;

	void m_splitChild(Node*&, size_t);
	void m_inordine(std::vector<T>&, const Node*) const;
	void m_insert(Node*&, const T&);

public: 
	BTree(unsigned order = 2) : m_order(order) {}

	void Insert(const T&);
	bool Check(const T&) const;
	T Succesor(const T&) const;
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
		m_insert(m_root, val);						// pot sa inserez in radacina pt ca nu plin
	}
}

template <class T>
void BTree<T>::m_splitChild(Node*& parent, size_t childIdx)
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

	newNode->keys.reserve(m_order - 1);
	newNode->children.reserve(m_order);

	for (size_t i = m_order; i < 2*m_order-1; ++i)
	{
		newNode->keys.push_back(child->keys[i]);				// pun cheile din jumatate dreapta in n
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
		size_t insertPos = node->keys.size();
		if (node->keys.size() > 0)
		{
			// caut binar cel mai din stanga element mai mare sau egala ca val
			// incep cu step de la cea mai mare putere a lui 2 mai <= cu nr de chei (ie dimensiunea vectorului)
			for (size_t step = (1ULL << size_t(log2(node->keys.size()))); step; step >>= 1)
			{
				if (insertPos >= step && node->keys[insertPos - step] >= val)
					insertPos -= step;
			}
		}
		
		// valoare trebuie inserata la stanga de cheia de la insertPos (daca exista)
		// adica in fiul de la insertPos

		Node* child = node->children[insertPos];

		if (child->keys.size() == 2 * m_order - 1) // daca fiul e plin
		{ 
			m_splitChild(node, insertPos); 
			// cheia de la insertPos = mediana din fiu
			// fiul de la insertPos = jumatatea stanga din fiul original
			// fiul de la insertPos+1 = jumatatea dreapta din fiul original

			if (val <= node->keys[insertPos])						// daca val e mai mic sau egal cu mediana ma duc in stanga
				m_insert(node->children[insertPos], val);
			else													// altfel ma duc in dreapta
				m_insert(node->children[insertPos + 1], val);
		}
		else // daca fiul nu e plin
		{
			m_insert(child, val);
		}

	}
}

template<class T>
bool BTree<T>::Check(const T& val) const
{
	Node* node = m_root;
	if (m_root == nullptr)return false;

	while (true)
	{
		// caut binar cea mai din dreapta cheie mai mica sau egala cu val
		size_t pos = binSearch(node->keys, val);
		
		if (node->keys[pos] == val)		// daca cheia == val atunci val e in BTree
		{
			return true;
		}
		else
		{
			if (node->isLeaf == true)	// daca nodul e frunza si nu s a gasit atunci nu e in BTree
			{
				return false;
			}
			else                        // daca nodul nu e frunza ma duc in fiu
			{
				node = val < node->keys[pos] ? node->children[pos] : node->children[pos+1]; // ma duc ori la stanga ori la dreapta cheii
			}
		}
	}
}

template<class T>
size_t BTree<T>::binSearch(const std::vector<T>& vals, const T& val)
{
	// cauta cea mai mare valoare mai mica sau egala cu val
	// daca vectorul e gol returneaza 0
	if (vals.empty())return 0;

	size_t pos = 0;

	// incep cu step ca cea mai mare putere a lui 2 mai mica sau egala cu vals.size
	size_t step = (1ULL << (size_t)(log2(vals.size())) );
	for (; step; step >>= 1)
	{
		if (pos + step < vals.size() && vals[pos + step] <= val)
			pos += step;
	}
	return pos;
}

template<class T>
T BTree<T>::Succesor(const T& val) const
{
	if (m_root == nullptr)
		throw std::out_of_range("tree is empty");

	T ancestor;
	bool hasAncestor = false;
	Node* node = m_root;

	while (true)
	{
		size_t pos = binSearch(node->keys, val);
		
		// daca e frunza succesorul e cea mai mare val mai mica sau egala cu val
		// sau poate sa fie primul stramos mai mare ca val
		if (node->isLeaf == true)
		{
			size_t pos = binSearch(node->keys, val);
			if (node->keys[pos] == val)return val;

			if (node->keys[pos] < val)
			{
				if (pos < node->keys.size() - 1)
					return node->keys[pos + 1];
				else if (hasAncestor)
					return ancestor;
				else
					throw std::invalid_argument("no succesor");
			}
			else								
				return node->keys[0];	// binSearch returneaza 0 daca toate valorile sunt mai mari
		}
		else // daca nu e frunza
		{
			size_t pos = binSearch(node->keys, val);
			if (val == node->keys[pos])return val;

			if (val < node->keys[pos])			// asta se intampla doar daca pos = 0 adica toate cheile sunt mai mari
			{	
				hasAncestor = true;
				ancestor = node->keys[pos];
				node = node->children[pos];
			}
			else
			{	
				if (pos + 1 < node->keys.size())	// ma duc in dreapta lui pos adica in stanga lui pos + 1 
				{									// (daca exista atunci cheia de pe pos + 1 e stramos si posibil succesor)
					hasAncestor = true;
					ancestor = node->keys[pos + 1];
				}
				node = node->children[pos + 1];
			}
		}
	}


}