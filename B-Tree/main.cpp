#include <iostream>
#include <random>
#include <vector>
#include <time.h>
#include <set>
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
    srand(time(0));
     
    //std::vector<int> v;
    //const int n = 200000;
    //for (int i = 0; i < n; ++i)
    //{
    //    v.push_back(rand());
    //}

    //BTree<int> t(500);
    //for (int i = 0; i < v.size(); ++i)
    //{
    //    t.Insert(v[i]);
    //}

    //std::sort(v.begin(), v.end());

    ///*print(v);
    //print(t.InOrdine());*/

    //std::cout << (v == t.InOrdine());

    BTree<int> t(2);
    t.Insert(10);
    t.Insert(20);
    t.Insert(30);
    t.Insert(40);
    while (1)
    {
        int x;
        std::cin >> x;
        std::cout << t.Check(x) << '\n';
    }
}