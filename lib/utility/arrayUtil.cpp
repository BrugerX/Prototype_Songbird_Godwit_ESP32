//
// Created by DripTooHard on 08-07-2023.
//

#include "arrayUtil.h"

int contains(int element, int * arr, int size)
{
    for(int i = 0; i<size;i++)
    {
        if(arr[i] == element)
        {
            return 1;
        }
    }

    return 0;
}