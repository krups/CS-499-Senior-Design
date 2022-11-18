#include "config.h"

void initializeSensors(SensorMap &sensors) {
  // Active Sensors
  // Entries should be formatted: sensor_id, sensor_priority, num_samples_per_data_point, num_bits_per_sample
  sensors.addSensor(THERMOCOUPLE_ID, THERMOCOUPLE_PRIORITY, THERMOCOUPLE_NUM_SAMPLES_PER_DATA_POINT, THERMOCOUPLE_NUM_BITS_PER_SAMPLE);
  sensors.addSensor(SPECTROMETER_ID, SPECTROMETER_PRIORITY, SPECTROMETER_NUM_SAMPLES_PER_DATA_POINT, SPECTROMETER_NUM_BITS_PER_SAMPLE);
  sensors.addSensor(ACCELEROMETER_ID, ACCELEROMETER_PRIORITY, ACCELEROMETER_NUM_SAMPLES_PER_DATA_POINT, ACCELEROMETER_NUM_BITS_PER_SAMPLE);
}