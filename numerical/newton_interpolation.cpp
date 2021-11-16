// 利用m次牛顿插值法计算出某一点的函数值
#include <stdio.h>
#include <assert.h>

#include <vector>
#include <iostream>


using VectorXd = std::vector<double>;
using MatrixXd = std::vector<VectorXd>;


// m: 插值次数(m<n)
double newtonInterpolation(const VectorXd& x, const VectorXd& y, int m, double xval)
{
    int n = x.size(); // 观测数据个数
    MatrixXd form(n, VectorXd(n));
    double yval = 0, mul = 1;
    // 计算均差表
    for (int i = 0; i < n; ++i)
        form[i][0] = y[i];
    for (int j = 1; j < n; ++j)
        for (int i = j; i < n; ++i)
            form[i][j] = (form[i][j - 1] - form[i - 1][j - 1]) / (x[i] - x[i - j]);
    // 输出均差表
    puts("table: ");
    for (int i = 0; i < n; ++i, puts(""))   
    {
        printf("%f ", x[i]);
        for (int j = 0; j < n; ++j)
        {
            if (i >= j)
            {
                printf("%f ", form[i][j]);
            }
        }
    }
    for (int i = 0; i < m + 1; ++i)
    {
        yval += form[i][i] * mul;
        mul *= (xval - x[i]);
    }
    return yval;
}


int main()
{
    VectorXd x = {0.25, 0.30, 0.36, 0.39, 0.45};
    VectorXd y = {0.2231436, 0.2623643, 0.3074847, 0.3293037, 0.3715636};

    double xval = 0.275;

    printf("f(%g): %.10f\n", xval, newtonInterpolation(x, y, x.size() - 1, xval));
    return 0;
}
