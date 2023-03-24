#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "config.h"
#include "Sensors.h"
#include "copyBits.h"

SensorMap sensors;

int main()
{
  // Setup sensor configuration
  sensors.addSensor(TC_ID, TC_PRIORITY, TC_NUM_SAMPLES_PER_DATA_POINT, TC_NUM_BITS_PER_SAMPLE, TC_OFFSET, TC_MULT);
  sensors.addSensor(IMU_ID, IMU_PRIORITY, IMU_NUM_SAMPLES_PER_DATA_POINT, IMU_NUM_BITS_PER_SAMPLE, IMU_OFFSET, IMU_MULT);
  sensors.addSensor(GPS_ID, GPS_PRIORITY, GPS_NUM_SAMPLES_PER_DATA_POINT, GPS_NUM_BITS_PER_SAMPLE, GPS_OFFSET, GPS_MULT);
  sensors.addSensor(RMC_ID, RMC_PRIORITY, RMC_NUM_SAMPLES_PER_DATA_POINT, RMC_NUM_BITS_PER_SAMPLE, RMC_OFFSET, RMC_MULT);
  sensors.addSensor(ACC_ID, ACC_PRIORITY, ACC_NUM_SAMPLES_PER_DATA_POINT, ACC_NUM_BITS_PER_SAMPLE, ACC_OFFSET, ACC_MULT);
  sensors.addSensor(PRES_ID, PRES_PRIORITY, PRES_NUM_SAMPLES_PER_DATA_POINT, PRES_NUM_BITS_PER_SAMPLE, PRES_OFFSET, PRES_MULT);
  sensors.addSensor(SPEC_ID, SPEC_PRIORITY, SPEC_NUM_SAMPLES_PER_DATA_POINT, SPEC_NUM_BITS_PER_SAMPLE, SPEC_OFFSET, SPEC_MULT);

  std::string answer;
  
  std::cout << "Would you like to decode a packet or data file? (say packet or data) ";
  std::cin >> answer;

  if (answer == "packet")
  {
  
    std::ifstream packetFile;
    std::string path = PACKET_DATA_PATH;

    std::string filename;

    // Request packet file to decode
    std::cout << "Please enter the name of the file that you want to decode: ";
    std::cin >> filename;

    path += filename;

    // Prepare a buffer to read in packet data
    // Add an extra null byte to make sure you don't accidentally read past the end of the packet when processing the packet data
    char data[PACKET_SIZE + 1];
    memset(data, '\0', PACKET_SIZE + 1);

    // Open the packet file and read the contents
    packetFile.open(path, std::ios_base::in);
    if (!packetFile.fail())
    {
      packetFile.read(data, PACKET_SIZE);
    }
    // Output error message if open fails
    else
    {
      std::cout << "Error opening packet file" << std::endl;
      return 1;
    }

    // Proceed until an error is detected
    bool moreData = true;

    unsigned int bitIndex = 0;

    while (moreData)
    {
      // Attempt to extract a sensor ID from the packet data
      unsigned int sensorId = 0;

      copyBitsB2L((uint8_t *)data, bitIndex, (uint8_t *)&sensorId, (sizeof(unsigned int) * 8) - SENSOR_ID_BITS, sizeof(unsigned int), SENSOR_ID_BITS);

      bitIndex += SENSOR_ID_BITS;

      // If the sensor ID is nonzero (it was modified, indicating that data exists and it's not the end of the packet)
      if (sensorId != 0)
      {
        // If the sensor ID is a valid sensor ID
        if (sensors.sensorMap[sensorId] != nullptr)
        {
          // Begin outputing info for this data point
          std::cout << sensorId;

          // Extract the timestamp and display it
          unsigned int timestamp = 0;
          copyBitsB2L((uint8_t *)data, bitIndex, (uint8_t *)&timestamp, (sizeof(unsigned int) * 8) - SENSOR_TIMESTAMP_BITS, sizeof(unsigned int), SENSOR_TIMESTAMP_BITS);
          bitIndex += SENSOR_TIMESTAMP_BITS;
          std::cout << ", " << timestamp;

          // For every sample that exists for the designated sensor
          for (unsigned i = 0; i < sensors.sensorMap[sensorId]->numSamplesPerDataPoint; i++)
          {
            // Read the sample value
            int value = 0;
            copyBitsB2L((uint8_t *)data, bitIndex, (uint8_t *)&value, (sizeof(unsigned int) * 8) - sensors.sensorMap[sensorId]->numBitsPerSample, sizeof(unsigned int), sensors.sensorMap[sensorId]->numBitsPerSample);
            bitIndex += sensors.sensorMap[sensorId]->numBitsPerSample;

            // If this sensor uses a multiplier for fixed precision floating point numbers
            if (sensors.sensorMap[sensorId]->multiplier != 1) {
              // Convert the value to a floating point number and remove the multiplier
              double valuePrecise = (double) value / (double) sensors.sensorMap[sensorId]->multiplier;

              // If this sensor uses an offset, remove the offset
              if (sensors.sensorMap[sensorId]->offset != 0) {
                valuePrecise -= (double) sensors.sensorMap[sensorId]->offset;
              }

              // Output the extracte value
              std:: cout << ", " << valuePrecise;
            // If this sensor doesn't use a multiplier
            } else {
              // Still check if it uses an offset and remove it if necessary
              if (sensors.sensorMap[sensorId]->offset != 0) {
                value -= sensors.sensorMap[sensorId]->offset;
              }

            // Display the extracted value
            std:: cout << ", " << value;
            }
          }

          // Add a newline at the end of that data point
          std::cout << std::endl;
        }
        // If the sensor ID is invalid
        else
        {
          // Output an error and quit
          std::cout << "Unknown sensor type" << std::endl;
          moreData = false;
        }
      }
      // If no data was read
      else
      {
        // Indicate the end of the packet
        std::cout << "End of packet" << std::endl;
        moreData = false;
      }
    }
  }
  else
  {
    std::ifstream dataFile;
    std::string path = SENSOR_DATA_PATH;

    std::string filename;

    // Request data file to decode
    std::cout << "Please enter the name of the file that you want to decode: ";
    std::cin >> filename;

    path += filename;

    // Prepare a buffer to read in lines of data
    // Add an extra null byte to make sure you don't accidentally read past the end of the packet when processing the packet data
    int numBytes = ((sensors.sensorMap[stoi(filename)]->numBitsPerDataPoint + 7) / 8) + 1;
    char *line = new char[numBytes];
    memset(line, '\0', numBytes);

    // Open the packet file and read the contents
    dataFile.open(path, std::ios_base::in);
    if (!dataFile.fail())
    {
      dataFile.read(line, numBytes);
    }
    // Output error message if open fails
    else
    {
      std::cout << "Error opening packet file" << std::endl;
      return 1;
    }

    // Proceed until an error is detected
    bool moreData = true;

    unsigned int bitIndex;

    while (moreData)
    {
      bitIndex = 0;
      // Attempt to extract a sensor ID from the packet data
      unsigned int sensorId = 0;

      copyBitsB2L((uint8_t *)line, bitIndex, (uint8_t *)&sensorId, (sizeof(unsigned int) * 8) - SENSOR_ID_BITS, sizeof(unsigned int), SENSOR_ID_BITS);

      bitIndex += SENSOR_ID_BITS;

      // If the sensor ID is nonzero (it was modified, indicating that data exists and it's not the end of the packet)
      if (sensorId != 0)
      {
        // If the sensor ID is a valid sensor ID
        if (sensors.sensorMap[sensorId] != nullptr)
        {
          // Begin outputing info for this data point
          std::cout << sensorId;

          // Extract the timestamp and display it
          unsigned int timestamp = 0;
          copyBitsB2L((uint8_t *)line, bitIndex, (uint8_t *)&timestamp, (sizeof(unsigned int) * 8) - SENSOR_TIMESTAMP_BITS, sizeof(unsigned int), SENSOR_TIMESTAMP_BITS);
          bitIndex += SENSOR_TIMESTAMP_BITS;
          std::cout << ", " << timestamp;

          // For every sample that exists for the designated sensor
          for (unsigned i = 0; i < sensors.sensorMap[sensorId]->numSamplesPerDataPoint; i++)
          {
            // Read the sample value
            int value = 0;
            copyBitsB2L((uint8_t *)line, bitIndex, (uint8_t *)&value, (sizeof(unsigned int) * 8) - sensors.sensorMap[sensorId]->numBitsPerSample, sizeof(unsigned int), sensors.sensorMap[sensorId]->numBitsPerSample);
            bitIndex += sensors.sensorMap[sensorId]->numBitsPerSample;

            // If this sensor uses a multiplier for fixed precision floating point numbers
            if (sensors.sensorMap[sensorId]->multiplier != 1) {
              // Convert the value to a floating point number and remove the multiplier
              double valuePrecise = (double) value / (double) sensors.sensorMap[sensorId]->multiplier;

              // If this sensor uses an offset, remove the offset
              if (sensors.sensorMap[sensorId]->offset != 0) {
                valuePrecise -= (double) sensors.sensorMap[sensorId]->offset;
              }

              // Output the extracte value
              std:: cout << ", " << valuePrecise;
            // If this sensor doesn't use a multiplier
            } else {
              // Still check if it uses an offset and remove it if necessary
              if (sensors.sensorMap[sensorId]->offset != 0) {
                value -= sensors.sensorMap[sensorId]->offset;
              }

            // Display the extracted value
            std:: cout << ", " << value;
            }
          }

          // Add a newline at the end of that data point
          std::cout << std::endl;
          // Reset the data buffer
          memset(line, '\0', numBytes);
          dataFile.read(line, numBytes);
        }
        // If the sensor ID is invalid
        else
        {
          // Output an error and quit
          std::cout << "Unknown sensor type" << std::endl;
          moreData = false;
        }
      }
      // If no data was read
      else
      {
        // Indicate the end of the packet
        std::cout << "End of data file" << std::endl;
        moreData = false;
      }
    }
  }

  return 0;
}


