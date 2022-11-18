#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include "Sensors.h"

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
const std::string PACKET_DATA_PATH = "./data/packets/"; // This directory must exist before running the program

#define TC_ID 1
#define TC_PRIORITY 1
#define TC_NUM_SAMPLES_PER_DATA_POINT 4
#define TC_NUM_BITS_PER_SAMPLE 11

#define IMU_ID 2
#define IMU_PRIORITY 1
#define IMU_NUM_SAMPLES_PER_DATA_POINT 6
#define IMU_NUM_BITS_PER_SAMPLE 16

#define GPS_ID 3
#define GPS_PRIORITY 1
#define GPS_NUM_SAMPLES_PER_DATA_POINT 5
#define GPS_NUM_BITS_PER_SAMPLE 16

#define RMC_ID 4
#define RMC_PRIORITY 1
#define RMC_NUM_SAMPLES_PER_DATA_POINT 5
#define RMC_NUM_BITS_PER_SAMPLE 16

#define ACC_ID 5
#define ACC_PRIORITY 1
#define ACC_NUM_SAMPLES_PER_DATA_POINT 3
#define ACC_NUM_BITS_PER_SAMPLE 4

#define PRES_ID 6
#define PRES_PRIORITY 1
#define PRES_NUM_SAMPLES_PER_DATA_POINT 5
#define PRES_NUM_BITS_PER_SAMPLE 11
#define PRES_MULT 10

#define SPEC_ID 7
#define SPEC_PRIORITY 1
#define SPEC_NUM_SAMPLES_PER_DATA_POINT 5
#define SPEC_NUM_BITS_PER_SAMPLE 8

// Validity parameters
// Check for erroneous data
#define TC_LOW -1000
#define TC_MAX 10000

#define ACC_LOW -100
#define ACC_HIGH 100

// Data Selection Config
#define NEW_DATA_RATIO 0.8
#define POINT_INCLUDE_LIMIT 1

#define NEW_DATA_GRADIENT_SELECT true
#define OLD_DATA_GRADIENT_SELECT true

#define DEBUG // comment/uncomment to enable/disable debugging logs
#ifdef DEBUG
// #define PRINT_DATA
#define PACKET_P
#define VALIDITY_P
// #define DEBUG_P
#endif

#endif
