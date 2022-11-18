#include "data.h"

/**
 * @brief Construct a new Data:: Data object
 *        Based on format of incoming raw data:
 *        sensor type, time stamp, n number of sensors, value_1, ... , value_n
 *
 * @param line Line from serial stream of raw data
 */
Data::Data(char *line, SensorMap *sensors)
{
    std::vector<char *> tokens;
    char *ptr = strtok(line, ",");
    while (ptr != NULL)
    {
        tokens.push_back(ptr);
        ptr = strtok(NULL, " , ");
    }
    type = atoi(tokens[0]);
    if (!(sensors->sensorMap.count(type)))
    {
        std::string msg = "Error: Sensor id ";
        msg.append(std::to_string(type));
        msg.append(" not found!");
        throw msg;
    }
    time_stamp = (u_int32_t)atoi(tokens[1]);
    num_vals = sensors->sensorMap[type]->numSamplesPerDataPoint;
    for (int i = 2; i < num_vals + 2; i++)
    {
        data.push_back((u_int16_t)atoi(tokens[i]));
    }
    bits_per_sample = sensors->sensorMap[type]->numBitsPerSample;
    num_bytes = ceil(((float)sensors->sensorMap[type]->numBitsPerDataPoint + (sensors->sensorMap[type]->numBitsPerDataPoint % 8)) / 8);
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
    bits_per_sample = x.bits_per_sample;
    num_bytes = x.num_bytes;
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

int Data::getNumBytes()
{
    return num_bytes;
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

void Data::createBitBuffer(char *buf)
{
    int bufBitPos = 0;
    int id = type;
    int ts = time_stamp;
    std::vector<u_int16_t> points = data;
    printf("%d\n", bufBitPos);
    copyBitsL2B((uint8_t *)&id, (sizeof(int) * 8) - SENSOR_ID_BITS, sizeof(int), (uint8_t *)buf, bufBitPos, SENSOR_ID_BITS);
    bufBitPos += SENSOR_ID_BITS;
    printf("%d\n", bufBitPos);
    copyBitsL2B((uint8_t *)&ts, (sizeof(int) * 8) - SENSOR_TIMESTAMP_BITS, sizeof(int), (uint8_t *)buf, bufBitPos, SENSOR_TIMESTAMP_BITS);
    bufBitPos += SENSOR_TIMESTAMP_BITS;
    printf("%d\n", bufBitPos);
    for (int i = 0; i < num_vals; i++)
    {
        int point = points[i];
        copyBitsL2B((uint8_t *)&point, (sizeof(int) * 8) - bits_per_sample, sizeof(int), (uint8_t *)buf, bufBitPos, bits_per_sample);
        bufBitPos += bits_per_sample;
        printf("%d\n", bufBitPos);
    }
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
    out << x.type << "," << x.time_stamp;
    for (auto y : x.data)
    {
        out << "," << y;
    }
    out << "\n";
    return out;
}
