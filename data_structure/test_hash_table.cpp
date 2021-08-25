#include "hash_table.hpp"

int main(){
    using namespace std;
    HashTable<int,std::_Identity<int>, std::hash<int>> ht(20);
    ht.insertUnique(10);
    ht.insertUnique(15);
    cout << ht.nodeCnt() << endl;
    return 0;
}
