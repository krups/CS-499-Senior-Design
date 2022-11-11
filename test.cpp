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
#include "copyBits.h"

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
