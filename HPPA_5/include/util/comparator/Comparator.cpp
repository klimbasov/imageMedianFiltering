#include "Comparator.h"

int Comparator::compare(const void* data_1, const void* data_2, const unsigned int size)
{
    const unsigned __int8* data_internal_1 = (const unsigned __int8*)data_1;
    const unsigned __int8* data_internal_2 = (const unsigned __int8*)data_2;
    
    int missmatch = 0;
    for (unsigned int counter = 0; counter < size; counter++) {
        if (data_internal_1[counter] != data_internal_2[counter]) {
            ++missmatch;
        }
    }
    return missmatch;
}
