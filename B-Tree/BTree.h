#pragma once
#include <vector>
#include <iostream>
#include <cmath>

template <class T>
class BTree
{
private:
	static size_t binSearchLessEqual(const std::vector<T>&, const T& val);
	static size_t binSearchGreaterEqual(const std::vector<T>&, const T& val);

	size_t m_order;
	
	struct Node {
		bool isLeaf = true;
		std::vector<T> keys;
		std::vector<Node*> children;
	};
	Node* m_root = nullptr;

	void m_splitChild(Node*&, size_t);
	void m_inorder(std::vector<T>&, const Node*) const;
	void m_inorderRange(std::vector<T>&, const Node*, const T&, const T&) const;
	void m_insert(Node*&, const T&);

public: 
	BTree(unsigned order = 2) : m_order(order) {}

	void Insert(const T&);
	bool Check(const T&) const;
	T Successor(const T&) const;
	T Predecessor(const T&) const;
	std::vector<T> Inorder() const;
	std::vector<T> InorderRange(const T&, const T&) const;
};

template<class T>
size_t BTree<T>::binSearchLessEqual(const std::vector<T>& vals, const T& val)
{
	// cauta cea mai mare valoare mai mica sau egala cu val
	// daca toate val sunt mai mari returneaza 0
	// daca vectorul e gol returneaza 0
	if (vals.empty())return 0;

	size_t pos = 0;

	// incep cu step ca cea mai mare putere a lui 2 mai mica sau egala cu vals.size
	size_t step = (1ULL << (size_t)(log2(vals.size())));
	for (; step; step >>= 1)
	{
		if (pos + step < vals.size() && vals[pos + step] <= val)
			pos += step;
	}
	return pos;
}

template<class T>
size_t BTree<T>::binSearchGreaterEqual(const std::vector<T>& vals, const T& val)
{
	// cauta cea mai mare valoare mai mare sau egala cu val
	// daca toate val sunt mai mari returneaza vals.size
	if (vals.empty())return 0;

	size_t pos = vals.size();

	// incep cu step ca cea mai mare putere a lui 2 mai mica sau egala cu vals.size
	size_t step = (1ULL << (size_t)(log2(vals.size())));
	for (; step; step >>= 1)
	{
		if (pos >= step && vals[pos - step] >= val)
			pos -= step;
	}
	return pos;
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

	for (size_t i = m_order; i < 2 * m_order - 1; ++i)
	{
		newNode->keys.push_back(child->keys[i]);				// pun cheile din jumatate dreapta in n
		if (newNode->isLeaf == false)
		{
			newNode->children.push_back(child->children[i]);	// pun fii din jumatatea dreapta in n
		}
	}

	if (newNode->isLeaf == false)
	{
		newNode->children.push_back(child->children[2 * m_order - 1]); // pun si ultimul fiu care e la 2*m_order-1
	}

	// sterg jumatatea de chei din fiu pe care le am pus in celalalt nod
	child->keys.erase(child->keys.begin() + m_order - 1, child->keys.end());

	// sterg jumatatea de fii din fiu pe care i am pus in celalalt nod
	// nu trebuie sters fiul din stanga medianei si din cauza asta incep de la jumatate+1 (m_order)
	if (child->isLeaf == false)
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
		//daca am ajuns aici sigur nu adaug duplicate

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
		// caut binar cel mai din stanga element mai mare sau egala ca val
		size_t insertPos = binSearchGreaterEqual(node->keys, val);

		// daca val se afla deja in nod atunci return
		if (insertPos < node->keys.size() && node->keys[insertPos] == val)return;	

		// valoare trebuie inserata la stanga de cheia de la insertPos (daca exista)
		// adica in fiul de la insertPos
		Node* child = node->children[insertPos];

		// daca val se afla deja in fiu atunci return
		if (child->keys[binSearchLessEqual(child->keys, val)] == val)return;		

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

template <class T> 
void BTree<T>::m_inorder(std::vector<T>& vals, const Node* node) const
{
	if (node == nullptr)return;

	size_t n = node->keys.size();

	if (node->isLeaf == true)	// daca nodul e frunza iau toate cheile
	{
		vals.insert(vals.end(), node->keys.begin(), node->keys.end());
		return;
	}
	
	for (int i = 0; i < n; ++i)
	{
		// ma duc recursiv intr-un fiu si dupa retin cheia curenta
		// la sfarsit o sa fie un fiu in care o sa ma duc separat
		
		m_inorder(vals, node->children[i]); 	
		vals.push_back(node->keys[i]);
	}
	m_inorder(vals, node->children[n]); // iau si ultimul fiu
}

template<class T>
void BTree<T>::m_inorderRange(std::vector<T>& vals,const Node* node, const T& min, const T& max) const
{
	if (node == nullptr)return;

	size_t pos1 = binSearchLessEqual(node->keys, min);
	size_t pos2 = binSearchLessEqual(node->keys, max);

	if (node->isLeaf)
	{
		if (min > node->keys.back() || max < node->keys.front())return;
		
		// daca min > cheia de pe pozitia pos1 atunci nu o iau
		if (min > node->keys[pos1])
			pos1++;

		// iau toate cheile din [pos1, pos2+1)
		vals.insert(vals.end(), node->keys.begin() + pos1, node->keys.begin() + pos2 + 1);
		return;
	}


	if (min > node->keys.back()) 
	{
		// daca min mai mare decat toate cheile ma duc in ultimul copil
		m_inorderRange(vals, node->children.back(), min, max);
		return;
	}

	if (max < node->keys.front())
	{
		// daca max e mai mic decat toate cheile ma duc in primul copil
		m_inorderRange(vals, node->children.front(), min, max);
		return;
	}
	//cheia de pe pos1 e cea mai din dreapta cheie mai mica sau egala cu min
	//cheia de pe pos2 e cea mai din dreapta cheie mai mica sau egala cu max

	// fac separat cazul pentru cheia de pe pos1

	// daca min e mai mic strict decat cheia de pe pos1 ma duc in stanga lui pos1
	// asta se intampla doar daca min e mai mic decat prima cheie
	if(min < node->keys[pos1])
		m_inorderRange(vals, node->children[pos1], min, max);
	
	// dupa ce ma duc in stanga lui pos1 sau daca nu ma duc, retin cheia de pe pos1 daca min <= cheie
	if(min <= node->keys[pos1])
		vals.push_back(node->keys[pos1]);

	for (size_t i = pos1 + 1; i <= pos2; ++i)
	{
		// iau toate cheile de la pos1 + 1 pana la pos2 inclusiv
		// ma duc mai intai in copilul din stanga lor cheii apoi retin cheia
		m_inorderRange(vals, node->children[i], min, max);
		vals.push_back(node->keys[i]);
	}

	// daca max e mai mare decat cheia de pe pos2 ma duc si in copilul din dreapta
	if (max > node->keys[pos2])
		m_inorderRange(vals, node->children[pos2+1], min, max);
}

template <class T>
std::vector<T> BTree<T>::Inorder() const
{
	std::vector<T> vals;
	m_inorder(vals, m_root);
	return vals;
}

template <class T>
std::vector<T> BTree<T>::InorderRange(const T& min, const T& max) const
{
	std::vector<T> vals;
	m_inorderRange(vals, m_root, min, max);
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
		// daca val e in radacina atunci return
		if (m_root->keys[binSearchLessEqual(m_root->keys, val)] == val)return;


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

template<class T>
bool BTree<T>::Check(const T& val) const
{
	Node* node = m_root;
	if (m_root == nullptr)return false;

	while (true)
	{
		// caut binar cea mai din dreapta cheie mai mica sau egala cu val
		size_t pos = binSearchLessEqual(node->keys, val);
		
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
T BTree<T>::Successor(const T& val) const
{
	if (m_root == nullptr)
		throw std::out_of_range("tree is empty");

	T ancestor;
	bool hasAncestor = false;
	Node* node = m_root;

	while (true)
	{
		// iau cea mai din stanga cheie mai mare sau egala cu val
		size_t pos = binSearchGreaterEqual(node->keys, val);
		
		// daca e val e pe pozitia pos atunci chiar val e succesor
		if (pos < node->keys.size() && node->keys[pos] == val)
			return val;

		// daca e frunza succesorul e cheia de pe pozitia pos
		// sau, daca nu exista, primul stramos mai mare ca val (daca exista si el)
		if (node->isLeaf == true)
		{
			if (pos == node->keys.size())					// daca toate cheile din nod sunt mai mici ca val
			{
				if (hasAncestor)							// atunci daca exista un stramos mai mare ca val atunci el e succesor
					return ancestor;

				throw std::invalid_argument("no succesor"); // altfel toate valorile sunt mai mici si nu exista succesor
			}
			else
				return node->keys[pos];						// altfel cheia de pe pos e succesor
		}
		else								// daca nu e frunza
		{
			if (pos == node->keys.size())	// daca toate cheile sunt mai mici ma duc in ultimul fiu
				node = node->children[pos]; // ultimul fiu e pe pozitia pos
			else
			{	
				hasAncestor = true;			// altfel ma duc in stanga cheii de pe pozitia pos
				ancestor = node->keys[pos];	// deci cheia de pe pozitia pos e un stramos mai mare ca val si o retin
				node = node->children[pos];
			}
		}
	}
}

template<class T>
T BTree<T>::Predecessor(const T& val) const
{
	if (m_root == nullptr)
		throw std::out_of_range("tree is empty");

	T ancestor;
	bool hasAncestor = false;
	Node* node = m_root;

	while (true)
	{
		size_t pos = binSearchLessEqual(node->keys, val);

		// daca e val e pe pozitia pos atunci chiar val e predecesor
		if (node->keys[pos] == val)
			return val;

		if (node->isLeaf == true)		
		{
			// binSearchLessEqual returneaza chiar daca toate elem sunt mai mari, din cauza asta testez daca cheia este < ca val

			if (node->keys[pos] < val)	// daca e frunza si cheie de pe pos e mai mica ca val atunci ea e predecesor
				return node->keys[pos];
			
			// altfel toate cheile sunt mai mari si verific daca am stramos
			if (hasAncestor)
				return ancestor;
			
			// altfel n am stramos deci n am predecesor (toate val din btree sunt mai mari ca val)
			throw std::invalid_argument("no predecessor");
		}
		else // daca nu e frunza
		{
			if (node->keys[pos] > val) // daca toate val sunt mai mari ma duc in primul copil
				node = node->children[0];
			else
			{
				// cheia de pe pozitia pos e < decat val deci o retin ca posibil predecesor
				hasAncestor = true;
				ancestor = node->keys[pos];
				node = node->children[pos+1];			// ma duc in dreapta cheii si iau minimul de acolo
			}
		}
	}
}