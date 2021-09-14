
#include <iostream>


void move(char x, char y)
{
    std::cout << x << "->" << y << "  ";
}

void hanoiTower(int n, char start, char tmp, char end)
{
    if (n == 1)
    {
        move(start, end);
    }
    else
    {
        hanoiTower(n - 1, start, end, tmp);
        move(start, end);
        hanoiTower(n - 1, tmp, start, end);
    }
}

int main()
{
    using namespace std;
    hanoiTower(6, 'a', 'b', 'c');
    cout << endl;
    return 0;
}