// 列主元高斯消元法
#include <math.h>
#include <assert.h>

#include <vector>
#include <iostream>

#define EPS 0.5e-5 // 精度


using VectorXd = std::vector<double>;
using MatrixXd = std::vector<VectorXd>;


VectorXd gaussianElimination(MatrixXd Ab)
{
    assert(Ab.size() + 1 == Ab[0].size());
    const int N = Ab.size();
    VectorXd x(N);

    // 消元过程
    for (int k = 0; k < N; ++k)
    {
        int r = k;
        // 按列选主元
        for (int i = k + 1; i < N; ++i)
        {
            if (fabs(Ab[i][k]) > fabs(Ab[r][k]))
            {
                r = i;
            }
        }
        if (fabs(Ab[r][k]) < EPS)
        {
            return {};
        }
        // 行交换
        if (r > k)
        {
            double tmp;
            for (int j = 0; j < N + 1; ++j)
            {
                tmp = Ab[k][j];
                Ab[k][j] = Ab[r][j];
                Ab[r][j] = tmp;
            }
        }
        // 消元计算,化为上三角
        for (int i = k + 1; i < N; ++i)
        {
            double radio = Ab[i][k] / Ab[k][k];
            for (int j = k; j < N + 1; ++j)
            {
                Ab[i][j] -= radio * Ab[k][j];
            }
        }
    }
    // 回代过程
    for (int k = N - 1; k >= 0; --k)
    {
        double sum = 0;
        for (int j = k + 1; j < N; ++j)
        {
            sum += Ab[k][j] * x[j];
        }
        x[k] = (Ab[k][N] - sum) / Ab[k][k];
    }
    return x;
}


int main()
{
    using namespace std;
    // 增广矩阵
    MatrixXd Ab =
    {
        {31, -13, 0, 0, 0, -10, 0, 0, 0, -15},
        {-13, 35, -9, 0, -11, 0, 0, 0, 0, 27},
        {0, -9, 31, -10, 0, 0, 0, 0, 0, -23},
        {0, 0, -10, 79, -30, 0, 0, 0, -9, 0},
        {0, 0, 0, -30, 57, -7, 0, -5, 0, -20},
        {0, 0, 0, 0, -7, 47, -30, 0, 0, 12},
        {0, 0, 0, 0, 0, -30, 41, 0, 0, -7},
        {0, 0, 0, 0, -5, 0, 0, 27, -2, 7},
        {0, 0, 0, -9, 0, 0, 0, -2, 29, 10}
    };

    auto x = gaussianElimination(Ab);
    if (x.empty())
    {
        cout << "error: singular matrix" << endl;
        return -1;
    }
    for (const auto& val : x)
    {
        cout << val << " ";
    }
    cout << endl;
    return 0;
}
