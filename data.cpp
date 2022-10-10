#include <vector>
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
    time_stamp = (u_int16_t)atoi(tokens[1]);
    int num_vals = atoi(tokens[2]);
    data = new u_int16_t[num_vals];
    for (int i = 3; i < num_vals + 3; i++)
    {

        data[i] = (u_int16_t)atoi(tokens[i]);
    }
}

/**
 * @brief Get the Type object
 * 
 * @return int 
 */
int Data::getType() {
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
u_int16_t Data::getTimeStamp()
{
    return time_stamp;
}

/**
 * @brief Returns data array
 *
 * @return u_int16_t*
 */
u_int16_t *Data::getData()
{
    return data;
}

/**
 * @brief Destroy the Data:: Data object
 *
 */
Data::~Data()
{
    if (data != nullptr)
    {
        delete data;
    }
}