#include "config.h"

void initializeSensors(SensorMap &sensors)
{
  // Active Sensors
  // Entries should be formatted: sensor_id, sensor_priority, num_samples_per_data_point, num_bits_per_sample
  sensors.addSensor(TC_ID, TC_PRIORITY, TC_NUM_SAMPLES_PER_DATA_POINT, TC_NUM_BITS_PER_SAMPLE);
  sensors.addSensor(IMU_ID, IMU_PRIORITY, IMU_NUM_SAMPLES_PER_DATA_POINT, IMU_NUM_BITS_PER_SAMPLE);
  sensors.addSensor(GPS_ID, GPS_PRIORITY, GPS_NUM_SAMPLES_PER_DATA_POINT, GPS_NUM_BITS_PER_SAMPLE);
  sensors.addSensor(RMC_ID, RMC_PRIORITY, RMC_NUM_SAMPLES_PER_DATA_POINT, RMC_NUM_BITS_PER_SAMPLE);
  sensors.addSensor(ACC_ID, ACC_PRIORITY, ACC_NUM_SAMPLES_PER_DATA_POINT, ACC_NUM_BITS_PER_SAMPLE);
  sensors.addSensor(PRES_ID, PRES_PRIORITY, PRES_NUM_SAMPLES_PER_DATA_POINT, PRES_NUM_BITS_PER_SAMPLE, PRES_MULT);
  sensors.addSensor(SPEC_ID, SPEC_PRIORITY, SPEC_NUM_SAMPLES_PER_DATA_POINT, SPEC_NUM_BITS_PER_SAMPLE);
}