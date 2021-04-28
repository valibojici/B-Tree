#include <iostream>
#include "BTree.h"
int main()
{
    // TO DO m_splitChild 

    BTree<int> b(3);
    b.Insert(1);
    b.Insert(2);
    b.Insert(3);
    b.Insert(4);
    b.Insert(5);
    b.Insert(5);
    /*b.Insert(6);
    b.Insert(7);
    b.Insert(8);
    b.Insert(9);
    b.Insert(10);*/
    b.InOrdine(std::cout);
}