#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include "config.h"
#include "Sensors.h"
#include "copyBits.h"

SensorMap sensors;

int main() {
  initializeSensors(sensors);

  std::ifstream packetFile;
  std::string path = SENSOR_DATA_PATH;

  std::string filename;

  std::cout << "Please enter the name of the file that you want to decode: ";
  std::cin >> filename;

  path += filename;

  char data[PACKET_SIZE];
  memset(data, '\0', PACKET_SIZE);

  packetFile.open(path, std::ios_base::in);
  if (!packetFile.fail()) {
    packetFile.read(data, PACKET_SIZE);
  } else {
    std::cout << "Error opening packet file." << std::endl;
    return 1;
  }

  bool moreData = true;

  unsigned int bitIndex = 0;

  while (moreData) {
    unsigned int sensorId = 0;

    copyBitsB2L((uint8_t*) data, bitIndex, (uint8_t*) &sensorId, (sizeof(unsigned int) * 8) - SENSOR_ID_BITS, sizeof(unsigned int), SENSOR_ID_BITS);
    bitIndex += SENSOR_ID_BITS;

    if (sensorId != 0) {
      std::cout << sensorId << ", ";

      unsigned int timestamp = 0;
      copyBitsB2L((uint8_t*) data, bitIndex, (uint8_t*) &sensorId, (sizeof(unsigned int) * 8) - SENSOR_TIMESTAMP_BITS, sizeof(unsigned int), SENSOR_TIMESTAMP_BITS);
      bitIndex += SENSOR_TIMESTAMP_BITS;

      std::cout << timestamp << ", ";
      
      for (int i = 0; i < sensors.sensorMap[sensorId]->numSamplesPerDataPoint; i++) {
        unsigned int value = 0;

        copyBitsB2L((uint8_t*) data, bitIndex, (uint8_t*) &value, (sizeof(unsigned int) * 8) - sensors.sensorMap[sensorId]->numBitsPerSample, sizeof(unsigned int), sensors.sensorMap[sensorId]->numBitsPerSample);
        bitIndex += sensors.sensorMap[sensorId]->numBitsPerSample;

        std::cout << value << ", ";
      }

      std::cout << std::endl;
    } else {
      moreData = false;
    }
  }

  return 0;
}