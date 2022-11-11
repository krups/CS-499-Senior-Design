#include <stdio.h>
#include <errno.h>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <bitset>
#include <vector>

#include "config.h"
#include "data.h"
#include "serial_headers.h"
#include "DataSelector.h"

void copyBitsB(uint8_t *src, unsigned int srcStartBit, uint8_t *dst, unsigned int dstStartBit, unsigned int n)
{
    for (unsigned int i = 0; i < n; i++)
    {
        int srcPos = srcStartBit + i;
        int dstPos = dstStartBit + i;
        uint8_t desiredBit = (src[srcPos / 8] << (srcPos % 8)) & (1 << 7);
        dst[dstPos / 8] &= ~(1 << (7 - (dstPos % 8)));
        dst[dstPos / 8] |= (desiredBit >> (dstPos % 8));
    }
}

void copyBitsL(uint8_t *src, unsigned int srcStartBit, unsigned int srcSizeBytes, uint8_t *dst, unsigned int dstStartBit, unsigned int dstSizeBytes, unsigned int n)
{
    for (unsigned int i = 0; i < n; i++)
    {
        int srcPos = srcStartBit + i;
        int dstPos = dstStartBit + i;
        uint8_t desiredBit = (src[(srcSizeBytes - 1) - (srcPos / 8)] << (srcPos % 8)) & (1 << 7);
        dst[(dstSizeBytes - 1) - (dstPos / 8)] &= ~(1 << (7 - (dstPos % 8)));
        dst[(dstSizeBytes - 1) - (dstPos / 8)] |= (desiredBit >> (dstPos % 8));
    }
}

void copyBitsB2L(uint8_t *src, unsigned int srcStartBit, uint8_t *dst, unsigned int dstStartBit, unsigned int dstSizeBytes, unsigned int n)
{
    for (unsigned int i = 0; i < n; i++)
    {
        int srcPos = srcStartBit + i;
        int dstPos = dstStartBit + i;
        uint8_t desiredBit = (src[srcPos / 8] << (srcPos % 8)) & (1 << 7);
        dst[(dstSizeBytes - 1) - (dstPos / 8)] &= ~(1 << (7 - (dstPos % 8)));
        dst[(dstSizeBytes - 1) - (dstPos / 8)] |= (desiredBit >> (dstPos % 8));
    }
}

void copyBitsL2B(uint8_t *src, unsigned int srcStartBit, unsigned int srcSizeBytes, uint8_t *dst, unsigned int dstStartBit, unsigned int n)
{
    for (unsigned int i = 0; i < n; i++)
    {
        int srcPos = srcStartBit + i;
        int dstPos = dstStartBit + i;
        uint8_t desiredBit = (src[(srcSizeBytes - 1) - (srcPos / 8)] << (srcPos % 8)) & (1 << 7);
        dst[dstPos / 8] &= ~(1 << (7 - (dstPos % 8)));
        dst[dstPos / 8] |= (desiredBit >> (dstPos % 8));
    }
}
char tc_samp[256] = "1,33,4,1,8,16,5";
char buf[340];

int main()
{
    // memset(buf, '\0', sizeof(buf));
    Data data(tc_samp);
    std::cout << data;
    data.getData();
    copyBitsB((uint8_t *)tc_samp, 18, (uint8_t *)buf, 2, 14);
    
    std::cout << "buffer: " << buf << "\n";
    std::cout << "Size of buffer: " << sizeof(buf) << "\n";

    return 0;
}
