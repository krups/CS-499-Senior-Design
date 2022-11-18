#ifndef SENSORS_H
#define SENSORS_H

#include <map>
#include "config.h"

typedef uint8_t sensor_id_t;
typedef uint16_t sensor_timestamp_t;

class SensorSettings {
public:
  uint8_t priority;
  uint8_t numSamplesPerDataPoint;
  uint8_t numBitsPerSample;
  uint16_t numBitsPerDataPoint;
  SensorSettings();
  SensorSettings(uint8_t priority, uint8_t numSamplesPerDataPoint, uint8_t numBitsPerSample);
};

class SensorMap {
public:
  SensorMap();
  ~SensorMap();
  unsigned int sensorMapSize;
  std::map<sensor_id_t, SensorSettings*> sensorMap;
  void addSensor(sensor_id_t id, uint8_t priority, uint8_t numSamplesPerDataPoint, uint8_t numBitsPerSample);
};

#endif