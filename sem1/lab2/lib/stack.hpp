#include <mutex>
#include <vector>

template <class T>
class Stack
{
    std::mutex mutex;

    std::vector <T> stash;

    public:

    Stack () = default;

    int push (T obj)
    {
        std::lock_guard <std::mutex> guard(mutex);
        
        stash.push_back(obj);

        return 0;
    }

    template <typename It>
    int push_interval (It first, It last)
    {
        std::lock_guard <std::mutex> guard(mutex);

        for (It i = first; i != last; ++i)
            stash.push_back(*i);

        return 0;
    }

    int pop (T &obj)
    {
        std::lock_guard <std::mutex> guard(mutex);

        if (!stash.empty())
        {
            obj = stash.back();
            stash.pop_back();

            return 1;
        }

        return 0;
    }
};