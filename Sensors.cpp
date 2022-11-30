#include "Sensors.h"

SensorSettings::SensorSettings() {
  this->priority = 0;
  this->numSamplesPerDataPoint = 0;
  this->numBitsPerSample = 0;
  this->numBitsPerDataPoint = 0;
  this->multiplier = 1;
}

SensorSettings::SensorSettings(unsigned int priority, unsigned int numSamplesPerDataPoint, unsigned int numBitsPerSample) {
  this->priority = priority;
  this->numSamplesPerDataPoint = numSamplesPerDataPoint;
  this->numBitsPerSample = numBitsPerSample;
  this->numBitsPerDataPoint = SENSOR_METADATA_BITS + (numSamplesPerDataPoint * numBitsPerSample);
  this->multiplier = 1;
}

SensorSettings::SensorSettings(unsigned int priority, unsigned int numSamplesPerDataPoint, unsigned int numBitsPerSample, int multiplier) {
  this->priority = priority;
  this->numSamplesPerDataPoint = numSamplesPerDataPoint;
  this->numBitsPerSample = numBitsPerSample;
  this->numBitsPerDataPoint = SENSOR_METADATA_BITS + (numSamplesPerDataPoint * numBitsPerSample);
  this->multiplier = multiplier;
}

SensorMap::SensorMap() {
  this->sensorMapSize = 0;
}

SensorMap::~SensorMap() {
  for (auto [sensorId, sensorSettings] : sensorMap) {
    free(sensorMap[sensorId]);
  }
}

void SensorMap::addSensor(unsigned int id, unsigned int priority, unsigned int numSamplesPerDataPoint, unsigned int numBitsPerSample) {
  SensorSettings* newSensorSettings = new SensorSettings(priority, numSamplesPerDataPoint, numBitsPerSample);
  sensorMap[id] = newSensorSettings;
  sensorMapSize = sensorMap.size();
}

void SensorMap::addSensor(unsigned int id, unsigned int priority, unsigned int numSamplesPerDataPoint, unsigned int numBitsPerSample, int multiplier) {
  SensorSettings* newSensorSettings = new SensorSettings(priority, numSamplesPerDataPoint, numBitsPerSample, multiplier);
  sensorMap[id] = newSensorSettings;
  sensorMapSize = sensorMap.size();
}