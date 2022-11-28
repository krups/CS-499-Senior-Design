#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "config.h"
#include "Sensors.h"
#include "copyBits.h"

SensorMap sensors;

int main() {
  sensors.addSensor(TC_ID, TC_PRIORITY, TC_NUM_SAMPLES_PER_DATA_POINT, TC_NUM_BITS_PER_SAMPLE);
  sensors.addSensor(IMU_ID, IMU_PRIORITY, IMU_NUM_SAMPLES_PER_DATA_POINT, IMU_NUM_BITS_PER_SAMPLE);
  sensors.addSensor(GPS_ID, GPS_PRIORITY, GPS_NUM_SAMPLES_PER_DATA_POINT, GPS_NUM_BITS_PER_SAMPLE);
  sensors.addSensor(RMC_ID, RMC_PRIORITY, RMC_NUM_SAMPLES_PER_DATA_POINT, RMC_NUM_BITS_PER_SAMPLE);
  sensors.addSensor(ACC_ID, ACC_PRIORITY, ACC_NUM_SAMPLES_PER_DATA_POINT, ACC_NUM_BITS_PER_SAMPLE);
  sensors.addSensor(PRES_ID, PRES_PRIORITY, PRES_NUM_SAMPLES_PER_DATA_POINT, PRES_NUM_BITS_PER_SAMPLE, PRES_MULT);
  sensors.addSensor(SPEC_ID, SPEC_PRIORITY, SPEC_NUM_SAMPLES_PER_DATA_POINT, SPEC_NUM_BITS_PER_SAMPLE);

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
      if (sensors.sensorMap[sensorId] != nullptr) {
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
        std::cout << "Unknown sensor type" << std::endl;

        moreData = false;
      }
    } else {
      std::cout << "End of packet" << std::endl;

      moreData = false;
    }
  }

  return 0;
}