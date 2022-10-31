#include "data.h"

/**
 * @brief Construct a new Data:: Data object
 *        Based on format of incoming raw data:
 *        sensor type, time stamp, n number of sensors, value_1, ... , value_n
 *
 * @param line Line from serial stream of raw data
 */
Data::Data(char *line)
{
    std::vector<char *> tokens;
    char *ptr = strtok(line, ",");
    while (ptr != NULL)
    {
        tokens.push_back(ptr);
        ptr = strtok(NULL, " , ");
    }
    type = atoi(tokens[0]);
    time_stamp = (u_int32_t)atoi(tokens[1]);
    num_vals = atoi(tokens[2]);
    for (int i = 3; i < num_vals + 3; i++)
    {
        data.push_back((u_int16_t)atoi(tokens[i]));
    }
}

/**
 * @brief Construct a copy Data:: Data object
 *
 * @param x
 */
Data::Data(const Data &x)
{
    type = x.type;
    num_vals = x.num_vals;
    time_stamp = x.time_stamp;
    data = x.data;
}

/**
 * @brief Get the Type object
 *
 * @return int
 */
int Data::getType()
{
    return type;
}

/**
 * @brief Get the Num Vals object
 *
 * @return int
 */
int Data::getNumVals()
{
    return num_vals;
}

/**
 * @brief Get the Time Stamp object
 *
 * @return u_int16_t
 */
u_int32_t Data::getTimeStamp()
{
    return time_stamp;
}

/**
 * @brief Returns data array
 *
 * @return u_int16_t*
 */
std::vector<u_int16_t> Data::getData()
{
    return data;
}

/**
 * @brief Prints data to console for debugging
 *
 */
void Data::printData()
{
    std::cout << "Printing data for " << type << ": " << time_stamp << std::endl;
    for (int i = 0; i < num_vals; i++)
    {
        std::cout << "val_" << i << ": " << data[i] << std::endl;
    }
}

std::ostream &operator<<(std::ostream &out, const Data &x)
{
    out << x.type << "," << x.time_stamp << "," << x.num_vals;
    for (auto y : x.data) {
        out << "," << y;
    }
    out << "\n";
    return out;
}
