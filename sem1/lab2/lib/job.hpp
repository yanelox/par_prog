#include <functional>
#include <cmath>
#include <limits>

struct Job
{
    std::function <double(double)> func;

    double start = 0.0;
    double end = 0.0;

    double f_start = 0.0;
    double f_end = 0.0;

    double S_all = 0.0;

    // 1 - run succesfull, 0 - it's not
    int success = 0;

    double eps;

    double mid = 0.0;
    double f_mid = 0.0;

    double S_start_mid = 0.0;
    double S_mid_end = 0.0;

    Job () = default;

    Job (std::function <double(double)> &func_, double start_, double end_,
         double f_start_, double f_end_, double S, double eps_):
         func(func_), start(start_), end(end_), 
         f_start(f_start_), f_end(f_end_), 
         S_all(S), eps(eps_) {}
    
    void init_with_first (Job job)
    {
        func  = job.func;
        start = job.start;
        end   = job.mid;
        f_start = job.f_start;
        f_end   = job.f_mid;
        S_all   = job.S_start_mid;
        eps     = job.eps / 2;
    }

    void init_with_second (Job job)
    {
        func  = job.func;
        start = job.mid;
        end   = job.end;
        f_start = job.f_mid;
        f_end   = job.f_end;
        S_all   = job.S_mid_end;
        eps     = job.eps / 2;
    }

    double run ()
    {
        mid = (start + end) / 2;
        f_mid = func(mid);

        S_start_mid = (f_start + f_mid) * (mid - start) / 2;
        S_mid_end   = (f_mid   + f_end) * (end - mid) / 2;

        double S_sum = S_start_mid + S_mid_end;

        if (std::abs (S_all - S_sum) >= eps * std::abs(S_sum) && std::abs (S_all - S_sum) > std::numeric_limits<double>::epsilon())
            return 0.0;
        
        success = 1;
        
        return S_all;
    }
};