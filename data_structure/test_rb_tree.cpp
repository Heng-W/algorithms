
#include "rb_tree.hpp"

#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include <iomanip>


int main()
{
    using namespace std;
    srand(time(nullptr));

    vector<int> vec;
    for (int i = 0; i < 15; ++i)
    {
        vec.push_back(rand() % 100);
    }

    RBTree<int> tree;

    for (const auto& x : vec) tree.insert(x);

    for (const auto& x : tree) cout << setw(3) << x;
    cout << endl;
    for (auto it = tree.begin(); it != tree.end(); ++it)
    {
        cout << setw(3) << static_cast<int>(it.node->color);
    }
    cout << endl;

    for (int i = 0; i < (int)vec.size(); ++i)
    {
        tree.remove(vec[i]);
        for (const auto& x : tree) cout << setw(3) << x;
        cout << endl;
        for (auto it = tree.begin(); it != tree.end(); ++it)
        {
            cout << setw(3) << static_cast<int>(it.node->color);
        }
        cout << endl;
    }
    return 0;
}
