// 利用平方根法求解线性方程组的根
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <vector>
#include <iostream>


using VectorXd = std::vector<double>;
using MatrixXd = std::vector<VectorXd>;


VectorXd cholesky(const MatrixXd& A, const VectorXd& b)
{
    assert(A.size() == A[0].size());
    assert(A.size() == b.size());
    const int n = A.size();
    MatrixXd L(n, VectorXd(n));
    
    for (int k = 0; k < n; ++k) // 分解A得到A=L*L^T
    {
        double sum = 0;
        for (int i = 0; i < k; ++i)
            sum += L[k][i] * L[k][i];
        sum = A[k][k] - sum;
        L[k][k] = sqrt(sum > 0 ? sum : 0);
        for (int i = k + 1; i < n; ++i)
        {
            sum = 0;
            for (int j = 0; j < k; ++j)
                sum += L[i][j] * L[k][j];
            L[i][k] = (A[i][k] - sum) / L[k][k];
        }
        for (int j = 0; j < k; ++j)
        {
            L[j][k] = 0;
        }
    }
    // 回代过程
    VectorXd x(n);
    for (int i = 0; i < n; ++i)
        x[i] = b[i];
    for (int k = 0; k < n; ++k) // LY=b, solve Y
    {
        for (int i = 0; i < k; ++i)
            x[k] -= x[i] * L[k][i];
        x[k] /= L[k][k];
    }
    for (int k = n - 1; k >= 0; --k) // (L^T)X=Y, solve X
    {
        for (int i = k + 1; i < n; ++i)
            x[k] -= x[i] * L[i][k];
        x[k] /= L[k][k];
    }
    return x;
}


int main()
{
    using namespace std;
    
    MatrixXd A = {{4, 2, -2}, {2, 2, -3}, {-2, -3, 14}};
    VectorXd b = {10, 5, 4};
    auto x = cholesky(A, b);
    for (const auto& val : x)
    {
        cout << val << " ";
    }
    cout << endl;
    return 0;
}
