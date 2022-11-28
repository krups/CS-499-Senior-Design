#include "data.h"

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s)
{
    return rtrim(ltrim(s));
}

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

        // add ptr to vector of tokens
        tokens.push_back(ptr);
        ptr = strtok(NULL, ",");
    }

    // Extract sensor type and validate sensor parameters
    type = atoi(tokens[0]);
    if (!(sensors->sensorMap.count(type)))
    {
        std::string msg = "Error: Sensor id ";
        msg.append(std::to_string(type));
        msg.append(" not found!");
        throw msg;
    }
    SensorSettings *params = sensors->sensorMap[type];
    if (tokens.size() != params->numSamplesPerDataPoint + 2)
    {
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
        int value = atoi(tokens[i]);

        if (params->multiplier != -1)
            value *= params->multiplier;
        data.push_back((int)value);
    }

    bits_per_sample = params->numBitsPerSample;
    num_bytes = ceil(((float)params->numBitsPerDataPoint + (params->numBitsPerDataPoint % 8)) / 8);
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
    // printf("%d\n", bufBitPos);
    copyBitsL2B((uint8_t *)&id, (sizeof(int) * 8) - SENSOR_ID_BITS, sizeof(int), (uint8_t *)buf, bufBitPos, SENSOR_ID_BITS);
    bufBitPos += SENSOR_ID_BITS;
    // printf("%d\n", bufBitPos);
    copyBitsL2B((uint8_t *)&ts, (sizeof(int) * 8) - SENSOR_TIMESTAMP_BITS, sizeof(int), (uint8_t *)buf, bufBitPos, SENSOR_TIMESTAMP_BITS);
    bufBitPos += SENSOR_TIMESTAMP_BITS;
    // printf("%d\n", bufBitPos);
    for (int i = 0; i < num_vals; i++)
    {
        int point = points[i];
        copyBitsL2B((uint8_t *)&point, (sizeof(int) * 8) - bits_per_sample, sizeof(int), (uint8_t *)buf, bufBitPos, bits_per_sample);
        bufBitPos += bits_per_sample;
        // printf("%d\n", bufBitPos);
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
