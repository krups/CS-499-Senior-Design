#ifndef SENSORS_H
#define SENSORS_H

#include <vector>

typedef uint8_t sensor_id_t;
typedef uint16_t sensor_timestamp_t;

class Sensor {
public:
  sensor_id_t id;
  uint8_t priority;
  uint8_t numDataPoints;
  uint8_t numBitsPerDataPoint;
  Sensor() {this->id = 0; this->priority = 0; this->numDataPoints = 0; this-> numBitsPerDataPoint = 0;};
  Sensor(sensor_id_t id, uint8_t priority, uint8_t numDataPoints, uint8_t numBitsPerDataPoint) {this->id = id; this->priority = priority; this->numDataPoints = numDataPoints; this->numBitsPerDataPoint = numBitsPerDataPoint;};
};

class SensorList {
public:
  SensorList() {this->prioritySum = 0;};
  std::vector<Sensor> list;
  unsigned int prioritySum;
  void addSensor(sensor_id_t id, uint8_t priority, uint8_t numDataPoints, uint8_t numBitsPerDataPoint) {Sensor newSensor = Sensor(id, priority, numDataPoints, numBitsPerDataPoint; list.push_back(newSensor); prioritySum += priority;};
};

#endif
