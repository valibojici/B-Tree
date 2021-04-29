#include <iostream>
#include <random>
#include <vector>
#include <time.h>
#include "BTree.h"

template<class T>
void print(const std::vector<T>& vals,const char* sep = " ", const char* end = "\n") {
    if (vals.empty())return;
    std::cout << vals[0];
    for (int i = 1; i < vals.size(); ++i)
        std::cout << sep << vals[i];
    std::cout << end;
}

int main()
{
    // TO DO m_splitChild 
    std::vector<int> v;
    srand(time(0));
    for (int i = 0; i < 1000; ++i)
        v.push_back(rand() % 3000);
    

    /*print(v);*/
    
    BTree<int> b(10);

    for (int i : v)
        b.Insert(i);
    
    std::sort(v.begin(), v.end());
 
    auto vals = b.InOrdine();
    /* print(vals); */
    
    std::cout << ((vals == v) ? "True" : "False") << ' ';
}