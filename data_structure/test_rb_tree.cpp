
#include "rb_tree.hpp"

#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>


int main()
{
    using namespace std;
    srand(time(nullptr));

    vector<int> vec;
    for (int i = 0; i < 15; ++i)
    {
        vec.push_back(rand() % 100);
    }

    RBTree<int> tree(&*vec.cbegin(), &*vec.cend());

    for (const auto& x : tree) cout << x << " ";
    cout << endl;

    for (int i = 0; i < 10; ++i)
    {
        tree.remove(vec[i]);
    }
    for (const auto& x : tree) cout << x << " ";
    cout << endl;

    return 0;
}
