#include <cstdint>
#include <string.h>
#include <vector>
#include <iostream>

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
    u_int16_t getTimeStamp();
    std::vector<u_int16_t> getData();
    void printData();
    friend std::ostream &operator<<(std::ostream &out, const Data &x);

private:
    int type;
    int num_vals;
    u_int16_t time_stamp;
    std::vector<u_int16_t> data;
};