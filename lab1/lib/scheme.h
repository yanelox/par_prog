#include "stash.h"
#include <cmath>
#include <numbers>
#include <assert.h>

double f (double t, double x)
{
    return x + t;
}

double phi (double x) // t = 0
{
    return std::cos (std::numbers::pi * x);
}

double psi (double t) // x =  0
{
    return std::exp (-t);
}

struct Scheme
{
    MStash <double> stash;

    int x_size;
    int t_size;

    double tau;
    double h;

    double X_start;
    double X_end;

    double T;

    double left = 0;
    double right = 0;

    Scheme () = default;

    Scheme (int t_size_, int x_size_, double tau_, double h_, 
            double T_, double X_start_, double X_end_): 
                                    t_size(t_size_), x_size(x_size_),
                                    stash(t_size_, x_size_),
                                    tau(tau_), h(h_),
                                    T(T_),
                                    X_start(X_start_), X_end(X_end_) {}

    double get_x (int k)
    {
        assert (0 <= k and k < x_size);

        return X_start + h * k;
    }

    double get_t (int m)
    {
        assert (0 <= m and m < t_size);

        return tau * m;
    }

    double get_u (int k, int m) // k - t index, m - x index
    {
        return stash (k, m);
    }   

    int fill_zero_line ()
    {
        for (int i = 0; i < x_size; ++i)
            stash(0, i) = phi (get_x (i));

        return 0;
    }

    // shoulb be used only for rank = 0
    int fill_zero_column ()
    {
        for (int i = 0; i < t_size; ++i)
            stash(i, 0) = psi (get_t (i)); 

        return 0;
    }

    int count_line_tps (int n, int start, int end) //three point scheme
    {
        assert (0 < n and n < t_size);
        assert (0 <= start and start <= end and end <= x_size);

        for (int i = start; i < end; ++i)
        {
            double tmp_left = (i == 0) ? left : stash(n - 1, i - 1);

            double tmp_right = (i == x_size - 1) ? right : stash(n - 1, i + 1);

            stash (n, i) = tau * (f (get_t (n - 1), get_x (i)) - (tmp_right - tmp_left) / (2 * h)) + (tmp_left + tmp_right) / 2; 
        }

        return 0;
    }

    int count_line_olc (int n, int start, int end)
    {
        assert (0 < n and n < t_size);
        assert (0 <= start and start <= end and end <= x_size);

        for (int i = start; i < end; ++i)
        {
            double tmp_under = stash (n - 1, i);
            double tmp_left = (i == 0) ? left : stash (n - 1, i - 1);

            stash (n, i) = tmp_under - tau / h * (tmp_under - tmp_left) + tau * f (get_t (n), get_x(i));
        }

        return 0;
    }

    int count_column_olc (int n, int start, int end) // "obvious left corner" scheme
    {
        assert (0 <= n and n < x_size);
        assert (0 <= start and start <= end and end <= t_size);

        for (int i = start; i < end; ++i)
        {
            double tmp_under = stash (i - 1, n);
            double tmp_left = stash (i - 1, n - 1);

            stash (i, n) = tmp_under - tau / h * (tmp_under - tmp_left) + tau * f (get_t (i), get_x(n));
        }

        return 0;
    }

    int set_sides (double left_, double right_)
    {
        left = left_;
        right = right_;

        return 0;   
    }

    int print ()
    {
        for (int i = 0; i < x_size; ++i)
        {
            for (int j = 0; j < t_size; ++j)
                std::cout << stash(j, i) << " ";

            std::cout << std::endl;
        }

        return 0;
    }   
};