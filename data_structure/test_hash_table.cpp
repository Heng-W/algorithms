
#include "hash_table.hpp"
#include <iostream>

int main()
{
    using namespace std;
    HashTable<int> ht;
    ht.insert(10);
    ht.insert(15);
    cout << ht.size() << endl;

    for (const auto& x : ht) cout << x << " ";
    cout << endl;

    return 0;
}
