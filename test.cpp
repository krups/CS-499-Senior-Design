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

SensorMap sensors;
char tc_samp[256] = "2,333,0,1";
int bufBitPos = 0;

int main()
{
    sensors.addSensor(THERMOCOUPLE_ID, THERMOCOUPLE_PRIORITY, THERMOCOUPLE_NUM_SAMPLES_PER_DATA_POINT, THERMOCOUPLE_NUM_BITS_PER_SAMPLE);
    sensors.addSensor(SPECTROMETER_ID, SPECTROMETER_PRIORITY, SPECTROMETER_NUM_SAMPLES_PER_DATA_POINT, SPECTROMETER_NUM_BITS_PER_SAMPLE);

    try
    {
        Data data(tc_samp, &sensors);
        int size = data.getNumBytes();
        printf("sensor bytes: %d\n", size);
        char temp_buf[size];
        memset(&temp_buf, 0, size);
        data.printData();
        data.createBitBuffer(temp_buf);
        std::cout << "buffer: " << temp_buf << "\n";
    }

    catch (std::string error)
    {
        printf("%s\n", error.c_str());
    }

    return 0;
}
