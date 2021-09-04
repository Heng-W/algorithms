
#include "rb_tree.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iterator>

int main()
{
    using namespace std;
    srand(time(nullptr));

    vector<int> vec;
    for (int i = 0; i < 10; ++i)
    {
        vec.push_back(rand() % 100);
    }

    RBTree<int> tree(&*vec.cbegin(), &*vec.cend());
    auto res = tree.inorder();
    copy(res.cbegin(), res.cend(), ostream_iterator<int>(cout, " "));
    cout << endl;

/*
    for (int i = 0; i < 5; ++i)
    {
        tree.remove(vec[i]);
    }
*/
    res = tree.inorder();
    copy(res.cbegin(), res.cend(), ostream_iterator<int>(cout, " "));
    cout << endl;

    return 0;
}
