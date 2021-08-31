#include "hash_table.hpp"

int main()
{
    using namespace std;
    HashTable<int, std::hash<int>, std::_Identity<int>> ht(20);
    ht.insertUnique(10);
    ht.insertUnique(15);
    cout << ht.nodeCnt() << endl;
    cout << ht << endl;
    return 0;
}
