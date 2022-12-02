#include "Sensors.h"

SensorSettings::SensorSettings() {
  this->priority = 0;
  this->numSamplesPerDataPoint = 0;
  this->numBitsPerSample = 0;
  this->numBitsPerDataPoint = 0;
  this->offset = 0;
  this->multiplier = 1;
}

SensorSettings::SensorSettings(unsigned int priority, unsigned int numSamplesPerDataPoint, unsigned int numBitsPerSample) {
  this->priority = priority;
  this->numSamplesPerDataPoint = numSamplesPerDataPoint;
  this->numBitsPerSample = numBitsPerSample;
  this->numBitsPerDataPoint = SENSOR_METADATA_BITS + (numSamplesPerDataPoint * numBitsPerSample);
  this->offset = 0;
  this->multiplier = 1;
}

SensorSettings::SensorSettings(unsigned int priority, unsigned int numSamplesPerDataPoint, unsigned int numBitsPerSample, unsigned int offset) {
  this->priority = priority;
  this->numSamplesPerDataPoint = numSamplesPerDataPoint;
  this->numBitsPerSample = numBitsPerSample;
  this->numBitsPerDataPoint = SENSOR_METADATA_BITS + (numSamplesPerDataPoint * numBitsPerSample);
  this->offset = offset;
  this->multiplier = 1;
}

SensorSettings::SensorSettings(unsigned int priority, unsigned int numSamplesPerDataPoint, unsigned int numBitsPerSample, unsigned int offset, unsigned int multiplier) {
  this->priority = priority;
  this->numSamplesPerDataPoint = numSamplesPerDataPoint;
  this->numBitsPerSample = numBitsPerSample;
  this->numBitsPerDataPoint = SENSOR_METADATA_BITS + (numSamplesPerDataPoint * numBitsPerSample);
  this->offset = offset;
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

void SensorMap::addSensor(unsigned int id, unsigned int priority, unsigned int numSamplesPerDataPoint, unsigned int numBitsPerSample, unsigned int offset) {
  SensorSettings* newSensorSettings = new SensorSettings(priority, numSamplesPerDataPoint, numBitsPerSample, offset);
  sensorMap[id] = newSensorSettings;
  sensorMapSize = sensorMap.size();
}

void SensorMap::addSensor(unsigned int id, unsigned int priority, unsigned int numSamplesPerDataPoint, unsigned int numBitsPerSample, unsigned int offset, unsigned int multiplier) {
  SensorSettings* newSensorSettings = new SensorSettings(priority, numSamplesPerDataPoint, numBitsPerSample, offset, multiplier);
  sensorMap[id] = newSensorSettings;
  sensorMapSize = sensorMap.size();
}