// 利用SOR迭代法求解线性方程组的根
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <vector>
#include <iostream>

#define EPS 1e-13 // 精度


using VectorXd = std::vector<double>;
using MatrixXd = std::vector<VectorXd>;


// w: 松弛度
VectorXd SOR(const MatrixXd& A, const VectorXd& b, double w)
{
    assert(A.size() == A[0].size());
    assert(A.size() == b.size());

    int n = b.size();
    VectorXd x(n);
    int k = 0;
    while (++k < 1000)
    {
        double norm = 0;
        for (int i = 0; i < n; ++i)
        {
            double sum = 0;
            for (int j = 0; j < n; ++j)
            {
                if (j != i)
                    sum += A[i][j] * x[j];
            }
            double s = x[i];
            x[i] = (1 - w) * x[i] + w / A[i][i] * (b[i] - sum);
            norm = std::max(norm, fabs(s - x[i]));
        }
        if (norm < EPS)
        {
            return x;
        }
    }
    return {};
}


int main()
{
    using namespace std;
    MatrixXd A =
    {
        {4, -1, 0, -1, 0, 0},
        {-1, 4, -1, 0, -1, 0},
        {0, -1, 4, 0, 0, -1},
        {-1, 0, 0, 4, -1, 0},
        {0, -1, 0, -1, 4, -1},
        {0, 0, 1, 0, -1, 4}
    };
    VectorXd b = {2, 3, 2, 2, 1, 2};
    auto x = SOR(A, b, 1.1);
    for (const auto& val : x)
    {
        cout << val << " ";
    }
    cout << endl;
    return 0;
}