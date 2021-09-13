//KMP模式匹配算法
#include <string>
#include <vector>

//求next数组
void getNext(const std::string& pattern, std::vector<int>& next)
{
    const int size = pattern.size();
    next.resize(size);
    next[0] = -1;
    int j = 0, k = -1;
    while (j < size - 1)
    {
        if (k == -1 || pattern[j] == pattern[k])
            next[++j] = ++k;
        else
            k = next[k];
    }
}

int kmp(const std::string& text, const std::string& pattern)
{
    std::vector<int> next;
    getNext(pattern, next);
    int i = 0, j = 0;
    while (i < (int)text.size() && j < (int)pattern.size())
    {
        if (j == -1 || text[i] == pattern[j])
        {
            ++i;
            ++j;
        }
        else
        {
            j = next[j];
        }
    }
    if (j == (int)pattern.size())
        return i - j;
    else
        return -1;
}


//测试
#include <iostream>

int main()
{
    using namespace std;
    cout << kmp("dasdddaad", "daad") << endl;
    return 0;
}
