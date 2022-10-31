#ifndef SENSORS_H
#define SENSORS_H

#include <vector>

typedef uint8_t sensor_id_t;
typedef uint16_t sensor_timestamp_t;

class Sensor {
public:
  sensor_id_t id;
  uint8_t priority;
  uint8_t numBytes;
  Sensor() {this->id = 0; this->priority = 0;};
  Sensor(sensor_id_t id, uint8_t priority, uint8_t numBytes) {this->id = id; this->priority = priority; this->numBytes = numBytes;};
};

class SensorList {
public:
  std::vector<Sensor> list;
  void addSensor(sensor_id_t id, uint8_t priority, uint8_t numBytes) {Sensor newSensor = Sensor(id, priority, numBytes); list.push_back(newSensor);};
};

#endif
