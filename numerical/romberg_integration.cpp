// Romberg法求积分
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include <vector>
#include <functional>

#define EPS 0.5e-7  //精度


typedef std::vector<double> VectorXd;
typedef std::vector<VectorXd> MatrixXd;

using Function = std::function<double(double)>;


//Romberg法求积分
double rombergIntegration(const Function& f, double a, double b)
{
    assert(a < b);
    int n = 1;//区间等分数
    double h = b - a;//步长
    VectorXd Tn, Sn, Cn, Rn;

    Tn.push_back(0.5 * (b - a) * (f(a) + f(b)));

    for (int i = 1; i < 1000; i++)
    {
        double sum = 0;
        for (int k = 0; k < n; k++)
        {
            sum += f(a + h * (k + 0.5));
        }
        Tn.push_back(0.5 * (Tn[i - 1] + h * sum));
        Sn.push_back((4 * Tn[i] - Tn[i - 1]) / 3);
        if (i >= 2)
            Cn.push_back((16 * Sn[i - 1] - Sn[i - 2]) / 15);
        if (i >= 3)
            Rn.push_back((64 * Cn[i - 2] - Cn[i - 3]) / 63);
        n *= 2;
        h /= 2;

        //达到精度要求，则返回积分结果
        if (i >= 4 && (fabs(Rn[i - 3] - Rn[i - 4])) / 255 < EPS)
        {
            MatrixXd vec{ Tn, Sn, Cn, Rn };

            for (int i = 0; i < Tn.size(); i++)
            {
                printf("%3d  ", (int)pow(2, i));
                for (int j = 0; j < vec.size(); j++)
                {
                    if (i < vec[j].size())
                        printf("%.8f  ", vec[j][i]);
                }
                printf("\n");
            }
            return Rn.back();
        }
    }
    assert(-1);
}


int main()
{
    auto f = [](double x)
    {
        return 1.0 / (1 + 100 * x * x);
    };
    printf("\nresult: %.8f\n", rombergIntegration(f, -1, 1));
    return 0;
}

