#include "stack.hpp"
#include "job.hpp"

#include <thread>
#include <atomic>

struct Integral
{
    std::function <double(double)> func;

    double start = 0;
    double end = 0;

    double eps = 0;

    double res = 0;

    Integral (std::function <double(double)> func_, 
              double start_, double end_, double eps_): func(func_),
                                                        start(start_), end(end_), eps(eps_) {}
};

class Worker
{
    int count_threads;

    public:

    Worker (int count_): count_threads(count_) {}

    double run (Integral &integral)
    {
        double start = integral.start;
        double end = integral.end;
        double eps = integral.eps;
        std::function <double(double)> func = integral.func;

        Stack <Job> jobs;

        double h = (end - start) / count_threads;

        for (int i = 0; i < count_threads; ++i)
        {
            double f_start = func(start + i * h);
            double f_end   = func(start + i * h + h);

            double S = (f_start + f_end) * h / 2;

            jobs.push (Job (func, start + i * h, start + i * h + h, f_start, f_end, S, eps));
        }

        std::vector <std::thread> threads(count_threads);

        std::mutex mutex;
        double res = 0;

        std::atomic <int> thread_status(0);

        auto thread_job = [&jobs, &mutex, &res, &thread_status, copy_count_threads = count_threads]()
        {
            std::vector <Job> local_jobs;

            int have_next = 0;

            Job job;

            while (true)
            {
                if (have_next == 0 && !local_jobs.empty())
                {
                    job = local_jobs.back();
                    local_jobs.pop_back();
                }
                else if (have_next == 0 && !jobs.pop(job))
                {
                    thread_status.fetch_add(1);

                    while (!jobs.pop(job) && thread_status.load() != copy_count_threads) {}

                    if (thread_status.load() == copy_count_threads)
                        return;

                    thread_status.fetch_sub(1);
                }

                double job_res = job.run();

                if (job.success)
                {
                    std::lock_guard <std::mutex> guard(mutex);
                    
                    res += job_res;

                    have_next = 0;
                }

                else
                {
                    Job new_job1;
                    new_job1.init_with_first(job);
                    local_jobs.push_back(new_job1);

                    job.init_with_second(job); 
                    have_next = 1;
                }

                if (local_jobs.size() > copy_count_threads)
                {
                    jobs.push_interval (local_jobs.begin(), local_jobs.begin() + copy_count_threads / 2);
                    local_jobs.erase (local_jobs.begin(), local_jobs.begin() + copy_count_threads / 2);
                }
            }
        };

        for (auto &thread:threads)
            thread = std::thread(thread_job);

        for (auto &thread:threads)
            thread.join();

        integral.res = res;

        return res;
    }
};