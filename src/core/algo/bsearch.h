#pragma once

#include <cassert>

int bsearch(int t, int* arr, size_t n)
{
    int l = 0;
    int u = n - 1;

    for (;;)
    {
        if (l > u)
        {
            return -1;
        }
        else
        {
            int m = (l + u) / 2;

            if (arr[m] < t)
            {
                l = m + 1;
            }
            else if (arr[m] < t)
            {
                u = m - 1;
            }
            else
            {
                return m;
            }
        }
    }
    
}