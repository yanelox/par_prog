#include <vector>
#include <cassert>
#include <iostream>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <random>
#include <chrono>

class field
{
    int width  = 1;
    int height = 1;
    int size = 1;
    std::vector <int> stash;
    std::vector <int> tmp_stash;
    std::vector <int> up;
    std::vector <int> down;

    int update_tmp (int first, int second)
    {
        assert (first < height and second < width);

        int old_state = get (first, second);
        int new_state = old_state + update_state (first, second);

        tmp_stash[(first * width + second) % (width * height)] = new_state;

        return old_state == new_state;
    }

    // 1  - клетка оживает
    // -1 - клетка умирает
    // 0  - клетка без изменений
    int update_state (int first, int second) const
    {
        int state = get(first, second);
        int res = 0;

        for (int i = -1; i <= 1; ++i)
            for (int j = -1; j <= 1; ++j)
                if (! (i == 0 and j == 0))
                    res += get (first + i, second + j);
                  
        if (state == 0 and res == 3)
            return 1;

        if (state == 1 and (res < 2 or res > 3))
            return -1;

        return 0;
    }

    int get (int first, int second) const
    {
        assert (-1 <= first  <= height);
        assert (-1 <= second <= width);

        if (first == -1)
            // first = height - 1;
            return up[(second + width) % width];

        if (first == height)
            // first = 0;
            return down[(second + width) % width];

        return stash[(first * width + second + size) % size];
    }

    public:
    
    int get_w () const {return width;}
    int get_h () const {return height;}

    field () = default;

    field (int w, int h): width(w), height(h), size(w * h), 
                          stash(w * h), tmp_stash (w * h),
                          up (w), down (w) {}
    
    field (int w, int h, std::vector <int> &init): width(w), height(h), size(w * h), 
                                                   tmp_stash (w * h), stash (init),
                                                   up (w), down (w) {}
    
    template <class InputIt>
    int init_field (InputIt source)
    {
        std::copy (source, source + size, stash.begin());

        return 0;
    }

    template <class InputIt>
    int get_str (InputIt dest, int num)
    {
        assert (0 <= num and num < height);

        for (int i = 0; i < width; ++i)
            dest[i] = get (num, i);

        return 0;
    }

    int update_updown (std::vector <int> &up_, std::vector <int> &down_)
    {
        std::copy (up_.begin(), up_.end(), up.begin());
        std::copy (down_.begin(), down_.end(), down.begin());

        return 0;
    }

    // 1 - нет обновлений
    // 0 - есть обновление
    int update ()
    {
        int res = 1;

        for (int i = 0; i < height; ++i)
            for (int j = 0; j < width; ++j)
                res *= update_tmp(i, j);
        
        std::swap (stash, tmp_stash);

        return res;
    }

    const std::vector <int> &get_stash () const
    {
        return stash;
    }
};