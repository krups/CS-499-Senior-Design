#include <cstdint>
#include <string.h>
#include <vector>
#include <iostream>
#include <cmath>

#include "Sensors.h"
#include "copyBits.h"
/**
 * @brief Class for sensor data
 *
 */
class Data
{
public:
    Data(char *line);
    Data(const Data &x);
    int getType();
    int getNumVals();
    int getNumBytes();
    u_int32_t getTimeStamp();
    std::vector<int> getData();
    void createBitBuffer(char *buf);
    void printData();
    friend std::ostream &operator<<(std::ostream &out, const Data &x);

private:
    int type;
    int num_vals;
    u_int32_t time_stamp;
    int bits_per_sample;
    int num_bytes;
    std::vector<int> data;
    bool is_number(const std::string &s);
};