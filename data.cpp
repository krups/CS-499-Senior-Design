#include "data.h"

/**
 * @brief Construct a new Data:: Data object
 *        Based on format of incoming raw data:
 *        sensor type, time stamp, value_1, ... , value_n
 *
 * @param line Line from serial stream of raw data
 */
Data::Data(char *line, SensorMap *sensors)
{
    // Tokenize line
    std::vector<char *> tokens;
    char *ptr = strtok(line, ",");
    while (ptr != NULL)
    {
        // if \n or \r are found replace with null character
        char *nl = strchr(ptr, '\n');
        if (nl)
        {
            *nl = '\0';
        }
        nl = strchr(ptr, '\r');
        if (nl)
        {
            *nl = '\0';
        }
        if (*ptr == ' ')
        {
            ptr++;
        }
        // add ptr to vector of tokens
        tokens.push_back(ptr);
        ptr = strtok(NULL, ",");
    }

    // Extract sensor type and validate sensor parameters
    type = atoi(tokens[0]);
    if (!(sensors->sensorMap.count(type)))
    { // sensor id has not been added
        std::string msg = "Error: Sensor id ";
        msg.append(std::to_string(type));
        msg.append(" not found!");
        throw msg;
    }
    SensorSettings *params = sensors->sensorMap[type];
    if (tokens.size() != params->numSamplesPerDataPoint + 2)
    { // number of sensors read does not match the configuration
        std::string msg = "Error: Invalid number of sensor values\n";
        msg.append("\tExpected: ");
        msg.append(std::to_string(params->numSamplesPerDataPoint));
        msg.append(" Detected: ");
        msg.append(std::to_string(tokens.size() - 2));
        msg.append("\n");
        throw msg;
    }

    time_stamp = (u_int32_t)atoi(tokens[1]);
    num_vals = params->numSamplesPerDataPoint;

    for (int i = 2; i < num_vals + 2; i++)
    {
        // Check if all values are numbers
        if (!is_number(tokens[i]))
        {
            std::string msg = "Error: value at ";
            msg.append(std::to_string(time_stamp));
            msg.append(" on sensor ID ");
            msg.append(std::to_string(type));
            msg.append(" improper format: ");
            msg.append(tokens[i]);
            throw msg;
        }
        double value = std::stod(tokens[i]);

        // allows for representation of negative values in custom representation
        if (params->offset != 0)
            value += (double)params->offset;
        // multiplier for fixed precision floats
        if (params->multiplier != 1)
            value *= (double)params->multiplier;

        data.push_back((int)value);
    }

    bits_per_sample = params->numBitsPerSample;

    // rounds up number of bits to the nearest byte for saving to sensor data to file
    num_bytes = (params->numBitsPerDataPoint + 7) / 8;
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
 * @brief Determines if string s only contains numerical values
 *
 * @param s
 * @return true
 * @return false
 */
bool Data::is_number(const std::string &s)
{
    return (strspn(s.c_str(), "-.0123456789") == s.size());
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
 * @brief Get the Num Bytes object
 *
 * @return int
 */
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
std::vector<int> Data::getData()
{
    return data;
}

/**
 * @brief Fills buffer, buf, with minimum binary
 *        value representation of values
 *
 * @param buf
 */
void Data::createBitBuffer(char *buf)
{
    int bufBitPos = 0;
    int id = type;
    int ts = time_stamp;
    std::vector<int> points = data;

#ifdef DATA_P
    printf("%d\n", bufBitPos);
#endif

    // Copy sensor ID into buf
    copyBitsL2B((uint8_t *)&id, (sizeof(int) * 8) - SENSOR_ID_BITS, sizeof(int), (uint8_t *)buf, bufBitPos, SENSOR_ID_BITS);
    bufBitPos += SENSOR_ID_BITS;

#ifdef DATA_P
    printf("%d\n", bufBitPos);
#endif

    // Copy timestamp into buf
    copyBitsL2B((uint8_t *)&ts, (sizeof(int) * 8) - SENSOR_TIMESTAMP_BITS, sizeof(int), (uint8_t *)buf, bufBitPos, SENSOR_TIMESTAMP_BITS);
    bufBitPos += SENSOR_TIMESTAMP_BITS;

#ifdef DATA_P
    printf("%d\n", bufBitPos);
#endif

    // Copies values into buf
    for (int i = 0; i < num_vals; i++)
    {
        int point = points[i];
        copyBitsL2B((uint8_t *)&point, (sizeof(int) * 8) - bits_per_sample, sizeof(int), (uint8_t *)buf, bufBitPos, bits_per_sample);
        bufBitPos += bits_per_sample;

#ifdef DATA_P
        printf("%d\n", bufBitPos);
#endif
    }
}

/**
 * @brief Prints data to console for debugging
 *
 */
void Data::printData()
{
    std::cout << "Printing data for sensor:" << type << "at t = " << time_stamp << std::endl;
    for (int i = 0; i < num_vals; i++)
    {
        std::cout << "val_" << i << ": " << data[i] << std::endl;
    }
}

/**
 * @brief Overload << operator
 *
 * @param out ostream
 * @param x Data object
 * @return std::ostream&
 */
std::ostream &operator<<(std::ostream &out, const Data &x)
{
    out << x.type << "," << x.time_stamp;
    for (auto y : x.data)
    {
        out << "," << y;
    }
    // out << "\n";
    return out;
}
