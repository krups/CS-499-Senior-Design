#include "Sensors.h"

SensorSettings::SensorSettings() {
  this->priority = 0;
  this->numSamplesPerDataPoint = 0;
  this->numBitsPerSample = 0;
  this->numBitsPerDataPoint = 0;
}

SensorSettings::SensorSettings(uint8_t priority, uint8_t numSamplesPerDataPoint, uint8_t numBitsPerSample) {
  this->priority = priority;
  this->numSamplesPerDataPoint = numSamplesPerDataPoint;
  this->numBitsPerSample = numBitsPerSample;
  this->numBitsPerDataPoint = SENSOR_METADATA_BITS + (numSamplesPerDataPoint * numBitsPerSample);
}

SensorMap::SensorMap() {
  this->sensorMapSize = 0;
}

SensorMap::~SensorMap() {
  for (auto [sensorId, sensorSettings] : sensorMap) {
    free(sensorMap[sensorId]);
  }
}

void SensorMap::addSensor(sensor_id_t id, uint8_t priority, uint8_t numSamplesPerDataPoint, uint8_t numBitsPerSample) {
  SensorSettings* newSensorSettings = new SensorSettings(priority, numSamplesPerDataPoint, numBitsPerSample);
  sensorMap[id] = newSensorSettings;
  sensorMapSize = sensorMap.size();
}