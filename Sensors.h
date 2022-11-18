#ifndef SENSORS_H
#define SENSORS_H

#include <map>
#include "config.h"


class SensorSettings {
public:
  uint8_t priority;
  uint8_t numSamplesPerDataPoint;
  uint8_t numBitsPerSample;
  uint16_t numBitsPerDataPoint;
  int multiplier;
  SensorSettings();
  SensorSettings(uint8_t priority, uint8_t numSamplesPerDataPoint, uint8_t numBitsPerSample);
  SensorSettings(uint8_t priority, uint8_t numSamplesPerDataPoint, uint8_t numBitsPerSample, int multiplier);
};

class SensorMap {
public:
  SensorMap();
  ~SensorMap();
  unsigned int sensorMapSize;
  std::map<int, SensorSettings*> sensorMap;
  void addSensor(int id, uint8_t priority, uint8_t numSamplesPerDataPoint, uint8_t numBitsPerSample);
  void addSensor(int id, uint8_t priority, uint8_t numSamplesPerDataPoint, uint8_t numBitsPerSample, int multiplier);
};

#endif