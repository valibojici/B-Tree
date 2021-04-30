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
    srand(time(0));
     
    std::vector<int> v;
    const int n = 200000;
    for (int i = 0; i < n; ++i)
    {
        v.push_back(rand());
    }

    BTree<int> t(500);
    for (int i = 0; i < v.size(); ++i)
    {
        t.Insert(v[i]);
    }

    std::sort(v.begin(), v.end());

    /*print(v);
    print(t.InOrdine());*/

    std::cout << (v == t.InOrdine());

    /*std::vector<int> v = {1,1 };

 
    int pos = v.size();
    int val = 0;
    for (int step = int(v.empty() ? 0 : log2(v.size())); step; step >>= 1)
    {
        if (pos - step >= 0 && v[pos - step] > val)
            pos -= step;
    }
    std::cout << pos;*/
}