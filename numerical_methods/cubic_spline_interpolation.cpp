// 三次样条插值
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include <vector>

#define EPS 0.5e-7 //精度


typedef std::vector<double> VectorXd;
typedef std::vector<VectorXd> MatrixXd;


//求一阶及二阶差商
MatrixXd differenceQuotient(const VectorXd& x, const VectorXd& y)
{
    MatrixXd form(3);
    form[0] = y;
    for (int i = 1; i < 3; i++)
    {
        for (int j = 0; j < x.size() - i; j++)
        {
            form[i].push_back((form[i - 1][j + 1] - form[i - 1][j]) / (x[i + j] - x[j]));
        }
    }
    return form;
}

//高斯列主元法解线性方组组
VectorXd gaussianElimination(MatrixXd Ab)
{
    int i, j, k;
    const int N = Ab.size();
    VectorXd x(N);

    /*消元过程*/
    for (k = 0; k < N; k++)
    {
        int r = k;
        //按列选主元
        for (i = k + 1; i < N; i++)
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
        //行交换
        if (r > k)
        {
            double tmp;
            for (j = 0; j < N + 1; j++)
            {
                tmp = Ab[k][j];
                Ab[k][j] = Ab[r][j];
                Ab[r][j] = tmp;
            }
        }
        //消元计算,化为上三角
        for (i = k + 1; i < N; i++)
        {
            double radio = Ab[i][k] / Ab[k][k];
            for (j = k; j < N + 1; j++)
            {
                Ab[i][j] -= radio * Ab[k][j];
            }
        }
    }
    /*回代过程*/
    for (k = N - 1; k >= 0; k--)
    {
        double sum = 0;
        for (j = k + 1; j < N; j++)
        {
            sum += Ab[k][j] * x[j];
        }
        x[k] = (Ab[k][N] - sum) / Ab[k][k];
    }
    return x;
}

//三次样条插值
MatrixXd cubicSplineInterpolation(const VectorXd& x, const VectorXd& y, const VectorXd& border)
{
    assert(x.size() == y.size());
    assert(x.size() > 1);
    int n = x.size() - 1;
    VectorXd h(n), u(n);

    for (int i = 0; i < n; i++)
    {
        h[i] = x[i + 1] - x[i];
    }
    for (int i = 1; i < n; i++)
    {
        u[i] = h[i - 1] / (h[i - 1] + h[i]);
    }

    MatrixXd form = differenceQuotient(x, y);//差商
    VectorXd d(n + 1);

    d[0] = (6 * ((y[1] - y[0]) / (x[1] - x[0]) - border[0]));
    d[n] = (6 * (border[1] - (y[n] - y[n - 1]) / (x[n] - x[n - 1])));
    for (int i = 0; i < form[2].size(); i++)
    {
        d[i + 1] = 6 * form[2][i];
    }

    MatrixXd Ab(n + 1, VectorXd(n + 2));//增广矩阵

    for (int i = 0; i < n + 1; i++)
    {
        Ab[i][i] = 2;
    }
    Ab[0][1] = 1;
    Ab[n][n - 1] = 1;
    for (int i = 1; i < n; i++)
    {
        Ab[i][i - 1] = u[i];
        Ab[i][i + 1] = 1 - u[i];
    }
    for (int i = 0; i < n + 1; i++)
    {
        Ab[i][n + 1] = d[i];
    }

    /*
        for (int i = 0; i < n + 1; i++)
        {
            for (int j = 0; j < n + 2; j++)
                printf("%6.3f ", Ab[i][j]);
            printf("\n");
        }
    */

    VectorXd M = gaussianElimination(Ab);//求解线性方程组
    assert(!M.empty());

    for (int i = 0; i < M.size(); i++)
    {
        printf("M%d: %f\n", i, M[i]);
    }

    MatrixXd coef(n, VectorXd(4));//插值函数系数矩阵
    for (int i = 0; i < n; i++)
    {
        coef[i][0] = y[i];
        coef[i][1] = form[1][i] - (1.0 / 3 * M[i] + 1.0 / 6 * M[i + 1] * h[i]);
        coef[i][2] = 0.5 * M[i];
        coef[i][3] = 1.0 / 6 / h[i] * (M[i + 1] - M[i]);
    }
    return coef;
}

//二分法搜索坐标区间
int binarySearch(double val, const VectorXd& x)
{
    int left = 0;
    int right = x.size() - 1;
    int mid;

    if (val < x[left] || val > x[right])
    {
        return -1;
    }
    while (right - left > 1)
    {
        mid = (left + right) / 2;
        val < x[mid] ? (right = mid) : (left = mid);
    }
    return left;
}

//已知横坐标，计算对应的插值函数值
inline double calculate(double xval, const MatrixXd& coef, const VectorXd& x)
{
    int idx = binarySearch(xval, x);
    assert(idx >= 0);
    double dx = xval - x[idx];
    return coef[idx][0] + coef[idx][1] * dx + coef[idx][2] * (dx * dx) + coef[idx][3] * (dx * dx * dx);
}

int main()
{
    const int N = 10;
    VectorXd x;
    for (int i = 0; i <= N; i++)
    {
        x.push_back(i);
    }
    VectorXd y = { 2.51, 3.30, 4.04, 4.70, 5.22, 5.54, 5.78, 5.40, 5.57, 5.70, 5.80 };
    VectorXd border = { 0.8, 0.2 };//边界条件

    MatrixXd coef = cubicSplineInterpolation(x, y, border);//三次样条插值，计算系数

    for (int i = 0; i < coef.size(); i++)
    {
        printf("S(x) = %.3f + (%6.3f)*(x-%.1f) + (%6.3f)*(x-%.1f)^2 + (%6.3f)*(x-%.1f)^3,  %.1f<=x<%.1f\n",
               coef[i][0], coef[i][1], x[i], coef[i][2], x[i], coef[i][3], x[i], x[i], x[i + 1]);
    }
    printf("\n");
    for (int i = 0; i < N; i++)
    {
        printf("S(%.1f) = %.6f\n", i + 0.5, calculate(i + 0.5, coef, x));
    }
    return 0;
}
