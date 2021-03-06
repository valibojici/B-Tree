#include <iostream>
#include <vector>
#include <fstream>
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
	void m_joinChild(Node*&, size_t);
	void m_rotate(Node*&, size_t, size_t);
	void m_inorder(std::vector<T>&, const Node*) const;
	void m_inorderRange(std::vector<T>&, const Node*, const T&, const T&) const;
	void m_insert(Node*&, const T&);
	void m_erase(Node*&, const T&);
	T m_eraseMinimum(Node*&);
	T m_eraseMaximum(Node*&);
	void m_deleteTree(Node*&);

public:
	BTree(unsigned order = 2) : m_order(order) {}

	void Insert(const T&);
	void Erase(const T&);
	bool Check(const T&) const;
	T Successor(const T&) const;
	T Predecessor(const T&) const;
	std::vector<T> Inorder() const;
	std::vector<T> InorderRange(const T&, const T&) const;
	~BTree();
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

template<class T>
void BTree<T>::m_rotate(Node*& parent, size_t childIdx, size_t childSiblingIdx)
{
	Node* child = parent->children[childIdx];
	Node* childSibling = parent->children[childSiblingIdx];

	// daca fratele fiului e la dreapta atunci iau prima cheie si primul fiu din fratele fiului
	// alfel iau ultimul fiu si ultima cheie din fratele fiului
	if (childIdx < childSiblingIdx)
	{
		child->keys.push_back(parent->keys[childIdx]);					// iau cheia din parinte si o pun in fiu ca ultima cheie

		if (child->isLeaf == false)
			child->children.push_back(childSibling->children.front());	// iau primul fiu din fratele fiului si il pun in fiu ca ultimul fiu

		parent->keys[childIdx] = childSibling->keys.front();			// iau prima cheie din fratele fiului si o pun in locul cheii din parinte

		childSibling->keys.erase(childSibling->keys.begin());			// sterg prima cheie din fratele fiului

		if (child->isLeaf == false)
			childSibling->children.erase(childSibling->children.begin());	// sterg primul fiu din fratele fiului
	}
	else
	{
		child->keys.insert(child->keys.begin(), parent->keys[childIdx - 1]);				// iau cheia din parinte si o pun in fiu ca prima cheie

		if (child->isLeaf == false)														// iau ultimul fiu din fratele fiului si il pun in fiu ca primul fiu
			child->children.insert(child->children.begin(), childSibling->children.back());

		parent->keys[childIdx - 1] = childSibling->keys.back();							// iau ultima cheie din fratele fiului si o pun in locul cheii din parinte

		childSibling->keys.pop_back();													// sterg ultima cheie din fratele fiului
		if (child->isLeaf == false)
			childSibling->children.pop_back();											// sterg ultimul fiu din fratele fiului
	}

}

template<class T>
void BTree<T>::m_joinChild(Node*& parent, size_t childIdx)
{
	// iau fiul de la pozitia childIdx si il unesc cu fiul de la childIdx + 1
	// cheia de la childIdx din parinte vine intre cheile din primul fiu si al doilea
	// pointerul la fiul stang se sterge in locul lui vine fiul din dreapta care o sa fie de fapt noul nod

	Node* child1 = parent->children[childIdx];
	Node* child2 = parent->children[childIdx + 1];

	child1->keys.push_back(parent->keys[childIdx]);														// pun cheia din parinte in fiu
	child1->keys.insert(child1->keys.end(), child2->keys.begin(), child2->keys.end());					// iau restul de chei din celalalt fiu
	child1->children.insert(child1->children.end(), child2->children.begin(), child2->children.end());	// iau restul de fii din celalalt fiu

	//sterg celalalt fiu ca nu m ai am nevoie de el
	delete child2;

	parent->keys.erase(parent->keys.begin() + childIdx);			// sterg cheia pe care am trimis o in fiu

	if (parent == m_root && m_root->keys.size() == 0)				// s ar putea ca parintele sa fie radacina si sa ramana goala
	{
		delete m_root;											// radacina ramane goala => sterg radacina
		m_root = child1;										//acum radacina e noul fiu format
	}
	else
	{
		parent->children.erase(parent->children.begin() + childIdx);    // elimin fiul pe care l am sters
		parent->children[childIdx] = child1;							// fiul este acum noul fiu format
	}

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

template<class T>
void BTree<T>::m_erase(Node*& node, const T& val)
{
	size_t pos = binSearchLessEqual(node->keys, val);
	if (node->isLeaf)
	{
		// daca nodul e frunza atunci caut si sterg val
		if (node->keys[pos] == val)
			node->keys.erase(node->keys.begin() + pos);
	}
	else // daca nodul nu e frunza
	{
		// daca val e mai mic ca toate cheile ma duc in primul fiu altfel ma duc in fiul de la pos + 1

		if (node->keys[pos] != val) // daca val nu e in nodul asta
		{
			Node* child = nullptr;
			Node* leftSibling = nullptr;
			Node* rightSibling = nullptr;
			size_t childIdx;

			if (val < node->keys.front())
			{
				childIdx = 0;
				child = node->children[0];			// daca val e mai < ca toate cheilie ma duc in primul fiu
				rightSibling = node->children[1];	// are doar frate la dreapta
			}
			else
			{
				childIdx = pos + 1;

				child = node->children[pos + 1];	// cheia de la pos e mai < decat val deci ma duc in dreapta
				leftSibling = node->children[pos];	// are sigur frate la stanga
				if (pos < node->keys.size() - 1)	// are frate la dreapta doar daca nu trebuie sa ma duc in ultimul fiu
					rightSibling = node->children[pos + 2];
			}

			if (child->keys.size() == m_order - 1)	// fiul e minimal nu pot sa ma duc in el
			{
				if (leftSibling != nullptr && leftSibling->keys.size() > m_order - 1)		 // daca are frate la stanga si nu e minimal iau de la el o cheie
				{
					m_rotate(node, childIdx, childIdx - 1);
				}
				else if (rightSibling != nullptr && rightSibling->keys.size() > m_order - 1) // daca are frate la dreapta si nu e minimal iau de la el o cheie
				{
					m_rotate(node, childIdx, childIdx + 1);
				}
				else																		// altfel daca are fratii minimali unesc un frate cu fiul
				{
					bool rootHasBeenDeleted = false;			// daca radacina are 1 cheie atunci o sa fie inlocuita cu fiul combinat
					if (node == m_root && m_root->keys.size() == 1)rootHasBeenDeleted = true;

					if (leftSibling != nullptr)
					{
						m_joinChild(node, childIdx - 1);		// unesc fratele la stanga cu fiul
						m_erase(leftSibling, val);				// acum fratele la stanga s a combinat cu fiul si stergerea continua in fratele la stanga
						return;
					}
					else
					{
						m_joinChild(node, childIdx);			// unesc fiul cu fratele la dreapta
						m_erase(child, val);					// acum fiul s a combinat cu fratele la dreapta si stergerea continua in fiu
						return;
					}

					if (rootHasBeenDeleted)						// daca radacina e modificata apelez din nou m_erase din radacina
					{
						m_erase(m_root, val);
						return;
					}
				}
			}
			// fiul nu e minimal
			m_erase(child, val);
		}
		else
		{
			// val e in nodul asta

			// daca fiul din stanga nu e minimal sterg predecesorul lui val de acolo si il pun in locul lui val
			if (node->children[pos]->keys.size() > m_order - 1)
			{
				node->keys[pos] = m_eraseMaximum(node->children[pos]);
				return;
			}

			// daca fiul din dreapta nu e minimal sterg succesorul lui val de acolo si il pun in locul lui val
			if (node->children[pos + 1]->keys.size() > m_order - 1)
			{
				node->keys[pos] = m_eraseMinimum(node->children[pos + 1]);
				return;
			}

			// daca radacina are 1 cheie atunci o sa fie inlocuita cu fiul combinat
			bool rootHasBeenDeleted = false;
			if (m_root->keys.size() == 1)rootHasBeenDeleted = true;

			// daca ambi fii sunt minimali ii unesc
			m_joinChild(node, pos);

			if (rootHasBeenDeleted)
			{
				m_erase(m_root, val);
			}
			else
			{
				// acum val e in fiul de la pos
				m_erase(node->children[pos], val);
			}
		}
	}
}

template<class T>
T BTree<T>::m_eraseMaximum(Node*& node)
{
	if (node->isLeaf == true) // daca e frunza maximul e ultima cheie si o sterg (node are cel putin m_order chei)
	{
		T temp = node->keys.back();
		node->keys.pop_back();
		return temp;
	}
	// altfel ma duc in ultimul fiu

	Node* child = node->children.back();
	Node* leftSibling = node->children.rbegin()[1];
	if (child->keys.size() == m_order - 1)					// daca ultimul fiu e minimal
	{
		if (leftSibling->keys.size() == m_order - 1)		// daca si fratele din stanga e minimal
		{
			m_joinChild(node, node->children.size() - 2);	// unesc fratele la stanga cu fiul
			return m_eraseMaximum(node->children.back());	// acum ultimul fiu e combinatia dintre cei doi
		}
		// daca fratele din stanga nu e minimal iau de la el o cheie
		m_rotate(node, node->children.size() - 1, node->children.size() - 2);
		return m_eraseMaximum(node->children.back());
	}
	// daca ultimul fiu nu e minimal pot sa ma duc in el direct
	return m_eraseMaximum(node->children.back());
}

template<class T>
T BTree<T>::m_eraseMinimum(Node*& node)
{
	if (node->isLeaf == true)	// daca e frunza minimul e prima cheie si o sterg (node are cel putin m_order chei)
	{
		T temp = node->keys[0];
		node->keys.erase(node->keys.begin());
		return temp;
	}

	// altfel ma duc in primul fiu
	Node* child = node->children[0];
	Node* rightSibling = node->children[1];

	if (child->keys.size() == m_order - 1)				// daca primul fiu e minimal
	{
		if (rightSibling->keys.size() == m_order - 1)	// daca si fratele la dreapta e minimal 
		{
			m_joinChild(node, 0);						// unesc fiul cu fratele
			return m_eraseMinimum(node->children[0]);	// acum primul fiu e combinatia dintre cei doi
		}
		// daca fratele la dreapta nu e minimal iau de la el o cheie
		m_rotate(node, 0, 1);
		return m_eraseMinimum(node->children[0]);
	}
	// daca primul fiu nu e minimal pot sa ma duc in el direct
	return m_eraseMinimum(node->children[0]);
}

template<class T>
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
void BTree<T>::m_inorderRange(std::vector<T>& vals, const Node* node, const T& min, const T& max) const
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
	if (min < node->keys[pos1])
		m_inorderRange(vals, node->children[pos1], min, max);

	// dupa ce ma duc in stanga lui pos1 sau daca nu ma duc, retin cheia de pe pos1 daca min <= cheie
	if (min <= node->keys[pos1])
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
		m_inorderRange(vals, node->children[pos2 + 1], min, max);
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
void BTree<T>::Erase(const T& val)
{
	if (m_root == nullptr)
		throw std::invalid_argument("Btree is empty");

	m_erase(m_root, val);
	if (m_root->keys.size() == 0)
	{
		delete m_root;
		m_root = nullptr;
	}
}

template<class T>
bool BTree<T>::Check(const T& val) const
{
	if (m_root == nullptr)
		return 0;

	Node* node = m_root;
	while (true)
	{
		// caut binar cea mai din dreapta cheie mai mica sau egala cu val
		size_t pos = binSearchLessEqual(node->keys, val);

		if (node->keys[pos] == val)			// daca o gasesc returnez true
			return true;

		if (node->isLeaf == true)			// daca e frunza si n am gasit val atunci returnez false
			return false;

		if (val < node->keys.front())		// daca val e mai mica decat toate cheile ma duc in primul fiu
			node = node->children[0];
		else
			node = node->children[pos + 1];	// altfel ma duc in dreapta cheii de pe pozitia pos

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
				node = node->children[pos + 1];			// ma duc in dreapta cheii si iau minimul de acolo
			}
		}
	}
}

template<class T>
void BTree<T>::m_deleteTree(Node*& node)
{
	if (node == nullptr)return;

	if (node->isLeaf == false)
		for (size_t i = 0; i < node->children.size(); ++i)
			m_deleteTree(node->children[i]);

	delete node;
}

template<class T>
BTree<T>::~BTree() {
	m_deleteTree(m_root);
}
 
int main()
{ 
    std::ifstream f("abce.in");
    std::ofstream g("abce.out");

    BTree<int> btree(10);
    int n, t, x, y;
    f >> n;
    for (int i = 0; i < n; ++i)
    {
        f >> t >> x;
        switch (t)
        {
        case 1:
            btree.Insert(x); break;
        case 2:
            btree.Erase(x); break;
        case 3:
            g << btree.Check(x) << '\n'; break;
        case 4:
            g << btree.Predecessor(x) << '\n'; break;
        case 5:
            g << btree.Successor(x) << '\n'; break;
        case 6:
        {
            f >> y;
            std::vector<int> temp = btree.InorderRange(x, y);
            for (int k : temp)
                g << k << ' ';
            g << '\n';
            break;
        }
        }
    }
}