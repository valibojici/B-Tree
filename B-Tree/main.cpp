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

    // TO DO refactor succesor
    srand(time(0));
     
    BTree<int> t(2);

    t.Insert(10);
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
            std::cout << t.Succesor(x) << '\n';
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << '\n';
        }
    }
}