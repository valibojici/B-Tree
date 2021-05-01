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
    BTree<int> t(2);

    
    srand(time(0));
    
   /* std::vector<int> v;
    for (int i = 0; i < 100000; ++i)
    {
        v.push_back(rand() % 1000);
    }

    for (int i : v)
        t.Insert(i);
    std::sort(v.begin(), v.end());
    std::cout << (t.InOrdine() == v);*/

   /* t.Insert(10);
    t.Insert(20);
    t.Insert(30);
    t.Insert(40);
    t.Insert(50);
    t.Insert(60);
    t.Insert(70);
    t.Insert(80);
    while (1)
    {
        int x;
        std::cin >> x;
        try
        {
            std::cout << "Predecessor: " << t.Predecessor(x) << "\n";
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << '\n';
        }

        try
        {
            std::cout << "Successor: " << t.Successor(x) << "\n\n";
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << "\n\n";
        }

    }*/

    std::vector<int> v;
    for (int i = 0; i < 10000; ++i)
        v.push_back(rand());

    for (int i : v)
        t.Insert(i);

    std::sort(v.begin(), v.end());
    
    for (int i = 0; i < 10000; ++i)
    {
        int x = rand();
        int treeSuc;
    
        int vSuc;
   
        try
        {
            treeSuc = t.Successor(x);
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << '\n';
        }

        try
        {
            auto it = std::lower_bound(v.begin(), v.end(), x);
            if (it == v.end())
                throw std::out_of_range("no succesor");
            vSuc = *it;
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << '\n';
            continue;
        }
        
        if(treeSuc != vSuc)
            std::cout << x << ' ' << treeSuc << ' ' << vSuc << '\n';
    }


    for (int i = 0; i < 10000; ++i)
    {
        int x = rand();
        int treePred;

        int vPred;

        try
        {
            treePred = t.Predecessor(x);
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << '\n';
        }

        try
        {
            int pos = 0;
            for (int step = (1 << 24); step; step >>= 1)
                if (pos + step < v.size() && v[step + pos] <= x)
                    pos += step;
            if (v[pos] > x)throw std::invalid_argument("no stl predecessor");
            vPred = v[pos];
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << '\n';
            continue;
        }

        if (treePred != vPred)
            std::cout << x << ' ' << treePred << ' ' << vPred << '\n';
    }
}