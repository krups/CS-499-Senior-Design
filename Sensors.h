#ifndef SENSORS_H
#define SENSORS_H

#include <map>
#include "config.h"


class SensorSettings {
public:
  unsigned int priority;
  unsigned int numSamplesPerDataPoint;
  unsigned int numBitsPerSample;
  unsigned int numBitsPerDataPoint;
  int multiplier;
  SensorSettings();
  SensorSettings(unsigned int priority, unsigned int numSamplesPerDataPoint, unsigned int numBitsPerSample);
  SensorSettings(unsigned int priority, unsigned int numSamplesPerDataPoint, unsigned int numBitsPerSample, int multiplier);
};

class SensorMap {
public:
  SensorMap();
  ~SensorMap();
  unsigned int sensorMapSize;
  std::map<unsigned int, SensorSettings*> sensorMap;
  void addSensor(unsigned int id, unsigned int priority, unsigned int numSamplesPerDataPoint, unsigned int numBitsPerSample);
  void addSensor(unsigned int id, unsigned int priority, unsigned int numSamplesPerDataPoint, unsigned int numBitsPerSample, int multiplier);
};

#endif