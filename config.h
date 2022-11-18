#ifndef CONFIG_H
#define CONFIG_H

#include <string>

// Maximum packet size in bytes
#define PACKET_SIZE 340
#define PACKET_SIZE_BITS PACKET_SIZE * 8

// Sensor metadata size in bytes
#define SENSOR_ID_BITS 4
#define SENSOR_TIMESTAMP_BITS 12
#define SENSOR_METADATA_BITS (SENSOR_ID_BITS + SENSOR_TIMESTAMP_BITS)

// Serial port which interfaces with Feather M4 Express
#define SERIAL_FEATHER "/dev/ttyS1"

// Max characters for line buffer
#define MAX_CHARS 256

// Sensor ID Config
const std::string SENSOR_DATA_PATH = "./data/"; // This directory must exist before running the program

#define THERMOCOUPLE_ID 1
#define THERMOCOUPLE_PRIORITY 1
#define THERMOCOUPLE_NUM_SAMPLES_PER_DATA_POINT 4
#define THERMOCOUPLE_NUM_BITS_PER_SAMPLE 11

#define SPECTROMETER_ID 2
#define SPECTROMETER_PRIORITY 1
#define SPECTROMETER_NUM_SAMPLES_PER_DATA_POINT 2
#define SPECTROMETER_NUM_BITS_PER_SAMPLE 1

#define ACCELEROMETER_ID 3
#define ACCELEROMETER_PRIORITY 1
#define ACCELEROMETER_NUM_SAMPLES_PER_DATA_POINT 2
#define ACCELEROMETER_NUM_BITS_PER_SAMPLE 16

// Validity parameters
// Check for erroneous data
#define TC_LOW -1000
#define TC_MAX 10000

#define ACC_LOW -100
#define ACC_HIGH 100

// Data Selection Config
#define NEW_DATA_RATIO 0.8
#define POINT_INCLUDE_LIMIT 1

#define DEBUG // comment/uncomment to enable/disable debugging logs
#ifdef DEBUG
// #define PRINT_DATA
#define PACKET_P
#define VALIDITY_P
#endif

#endif