#include <iostream>
#include <random>
#include <vector>
#include <time.h>
#include <set>
#include "BTree.h"
#include <fstream>

template<class T>
void print(const std::vector<T>& vals,const char* sep = " ", const char* end = "\n") {
    if (!vals.empty())
    {
        std::cout << vals[0];
        for (int i = 1; i < vals.size(); ++i)
            std::cout << sep << vals[i];
    }
    std::cout << end;
}

std::vector<int> getRange(const std::set<int>& v, int min, int max)
{
    std::vector<int> result;
    for (int i : v)
        if (min <= i && i <= max)result.push_back(i);
    return result;
}

void test_inorder()
{
    srand(time(0));
    std::set<int> v;
    BTree<int> t(5);

    for (int i = 0; i < 20000; ++i)
    {
        int x = rand();
        t.Insert(x);
        v.insert(x);
    }
     
 

    while (1)
    {
        int x, y;
        //std::cin >> x >> y;
        x = rand(), y = rand();

        if (x > y)std::swap(x, y);


        bool result = (t.InorderRange(x, y) == getRange(v, x, y));
        
        //std::cout << x << ' ' << y << ' ' << result << '\n';
        
        if (!result)
        {
            std::vector<int> temp;
            for (int i : v)
                temp.push_back(i);

            std::cout << (t.Inorder() == temp) << '\n';

            std::ofstream g("out.out");
            std::cout << "vector: ";

            for (int i : getRange(v, x, y))
                g << i << ' ';
            g << '\n';

            print(getRange(v, x, y));
            std::cout << "btree: ";
            for (int i : t.InorderRange(x, y))
                g << i << ' ';

            temp = t.Inorder();

            for (int i = 1; i < temp.size(); ++i)
                if (temp[i] == temp[i - 1])
                    std::cout << "---------------> " << temp[i] << '\n';

            print(t.InorderRange(x, y));

            
            break;
        }
        //std::cout << (t.InorderRange(x, y) == getRange(v, x, y)) << '\n';
    }
}

int main()
{
    

    
    srand(time(0));

    /*int i = 1;
    while (i++ < 300000)
    {
        std::cout << i << ":\n";
        BTree<int> t(2);
        std::vector<int> temp;
        for (int i = 0; i < 1000; ++i)
            temp.push_back(rand());

        for (int k : temp)
            t.Insert(k);

        std::vector<int> v = t.Inorder();
        for (int k = 1; k < v.size(); ++k)
            if (v[k] == v[k - 1])
            {
                std::cout << v[k] << '\n';
                print(temp);
                i = INT_MAX;
            }
    }*/

    /*BTree<int> t(2);

    for (int i : std::vector<int>{ 11,11,12 })
        t.Insert(i);
 


    print(t.Inorder());*/
    

    test_inorder();
    
    /*std::vector<int> v;
    for (int i = 0; i < 100000; ++i)
    {
        v.push_back(rand());
    }

    for (int i : v)
        t.Insert(i);
    std::sort(v.begin(), v.end());
    std::cout << (t.Inorder() == v);*/


   /* for (int i = 1; i < 200; ++i)
        t.Insert(i);

    print(t.InorderRange(9, 26));*/
    /*
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

    /*std::vector<int> v;
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
    }*/
}