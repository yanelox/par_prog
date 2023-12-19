#include <omp.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <cstring>
#include <assert.h>

const unsigned m = 9;
const unsigned n = (1 << m) - 1;
const double h = 1.0 / n;

const int rightBoundCount = 11;
const double rightBoundStart = 0;
const double rightBoundStep = 0.1;

const unsigned numIterations = 5000;

double eps = 1e-5;

void solve (double* a, double* b, double* c, double* f, double* x)
{
	#pragma omp parallel
    {
        unsigned stride = 1;
        for(unsigned nn = n, low = 2; nn > 1; nn /= 2, low *= 2, stride *= 2) {
            #pragma omp for
            for(int i = low - 1; i < n; i += stride * 2) {
                float alpha = -a[i] / b[i - stride];
                float gamma = -c[i] / b[i + stride];
                a[i] = alpha * a[i - stride];
                b[i] = alpha * c[i - stride] + b[i] + gamma * a[i + stride];
                c[i] = gamma * c[i + stride];
                f[i] = alpha * f[i - stride] + f[i] + gamma * f[i + stride];
            }
        }

        #pragma omp barrier

        x[n / 2] = f[n / 2] / b[n / 2];
        for(stride /= 2; stride >= 1; stride /= 2) {
            #pragma omp for
            for(unsigned i = stride - 1; i < n; i += stride * 2) {
                x[i] = (f[i] - ((i > stride) ? (a[i] * x[i - stride]) : 0.0) - ((i + stride < n) ? (c[i] * x[i + stride]) : 0.0)) / b[i];
            }
        }
    }
}

double diff (double* y1, double* y2)
{
	double res = 0.0;

	for (int i = 0; i < n; ++i)
		res += std::abs(y1[i] - y2[i]);

	return res;
}

int main(int argc, char* argv[])
{
	assert(argc == 2);

	int num_threads = atoi(argv[1]);

	double* a = new double[n];
	double* b = new double[n];
	double* c = new double[n];

	double* x = new double[n];
	for (int i = 0; i < n; ++i)
		x[i] = i * h;

	double* y_prev = new double[n];
	double* y_cur = new double[n];

	double* f = new double[n];

	omp_set_num_threads(num_threads);

	double rightBound = rightBoundStart;
	for (int rightBoundNum = 0; rightBoundNum < rightBoundCount; rightBoundNum++, rightBound += rightBoundStep)
	{
		// начальная апроксимация: 	y = 1 + (b - 2) * x + x^2
		for (int i = 0; i < n; ++i)
			y_prev[i] = 1 + (rightBound - 2) * x[i] + x[i] * x[i];

		double time_start = omp_get_wtime();

		int i = 0;
		for (i = 0; i < numIterations; ++i)
		{
			a[0] = 0;
			b[0] = 1;
			c[0] = 0;
			f[0] = 1;

			#pragma omp parallel for
			for (int j = 1; j < n - 1; ++j)
			{
				double e_y_minus = std::exp (y_prev[j - 1]);
				double e_y = std::exp (y_prev[j]);
				double e_y_plus = std::exp (y_prev[j + 1]);

				a[j] = 1.0 - h * h / 12.0 * e_y_plus;
				b[j] = -2.0 - 5.0 * h * h / 6.0 * e_y;
				c[j] = 1.0 - h * h / 12 * e_y_minus;

				f[j] = h * h / 12.0 * (e_y_plus * (1 - y_prev[j + 1]) + 10 * e_y * (1 - y_prev[j]) + e_y_minus * (1 - y_prev[j - 1]));
			}

			a[n - 1] = 0;
			b[n - 1] = 1;
			c[n - 1] = 0;
			f[n - 1] = rightBound;

			solve(a, b, c, f, y_cur);

			if (diff (y_cur, y_prev) < eps)
				break;

			std::memcpy (y_prev, y_cur, n * sizeof(double));
		}

		double time_end = omp_get_wtime();

		std::cerr << "b = " << rightBound << ". Elapsed time " << time_end - time_start << ". Num iterations " << i << std::endl;

		std::cout << rightBound << " ";
		for (int i = 0; i < n; ++i)
			std::cout << y_prev[i] << " ";

		std::cout << "\n";
	}

	delete[] a;
	delete[] b;
	delete[] c;
	delete[] x;
	delete[] y_prev;
	delete[] y_cur;
	delete[] f;
}