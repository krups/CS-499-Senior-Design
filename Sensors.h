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
  unsigned int offset;
  unsigned int multiplier;

  SensorSettings();
  SensorSettings(unsigned int priority, unsigned int numSamplesPerDataPoint, unsigned int numBitsPerSample);
  SensorSettings(unsigned int priority, unsigned int numSamplesPerDataPoint, unsigned int numBitsPerSample, unsigned int offset);
  SensorSettings(unsigned int priority, unsigned int numSamplesPerDataPoint, unsigned int numBitsPerSample, unsigned int offset, unsigned int multiplier);
};

class SensorMap {
public:
  unsigned int sensorMapSize;
  std::map<unsigned int, SensorSettings*> sensorMap;

  SensorMap();
  ~SensorMap();
  void addSensor(unsigned int id, unsigned int priority, unsigned int numSamplesPerDataPoint, unsigned int numBitsPerSample);
  void addSensor(unsigned int id, unsigned int priority, unsigned int numSamplesPerDataPoint, unsigned int numBitsPerSample, unsigned int offset);
  void addSensor(unsigned int id, unsigned int priority, unsigned int numSamplesPerDataPoint, unsigned int numBitsPerSample, unsigned int offset, unsigned int multiplier);
};

#endif