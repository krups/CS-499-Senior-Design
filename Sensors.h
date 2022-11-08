#ifndef SENSORS_H
#define SENSORS_H

#include <vector>
#include "config.h"

typedef uint8_t sensor_id_t;
typedef uint16_t sensor_timestamp_t;

class Sensor {
public:
  sensor_id_t id;
  uint8_t priority;
  uint8_t numSamplesPerDataPoint;
  uint8_t numBitsPerSample;
  uint16_t numBitsPerDataPoint;
  Sensor() {this->id = 0; this->priority = 0; this->numSamplesPerDataPoint = 0; this-> numBitsPerSample = 0;};
  Sensor(sensor_id_t id, uint8_t priority, uint8_t numSamplesPerDataPoint, uint8_t numBitsPerSample) {this->id = id; this->priority = priority; this->numSamplesPerDataPoint = numSamplesPerDataPoint; this->numBitsPerSample = numBitsPerSample; this->numBitsPerDataPoint = SENSOR_METADATA_BITS + (numSamplesPerDataPoint + numBitsPerDataPoint);};
};

class SensorList {
public:
  unsigned int listSize;
  std::vector<Sensor> list;
  SensorList() {listSize = 0;};
  void addSensor(sensor_id_t id, uint8_t priority, uint8_t numSamplesPerDataPoint, uint8_t numBitsPerSample) {Sensor newSensor = Sensor(id, priority, numSamplesPerDataPoint, numBitsPerSample); list.push_back(newSensor); listSize++;};
};

#endif
