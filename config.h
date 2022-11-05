#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include "Sensors.h"

// Maximum packet size in bytes
#define PACKET_SIZE 340
#define PACKET_SIZE_BITS PACKET_SIZE * 8

// Sensor metadata size in bytes
#define SENSOR_METADATA_BITS 16
#define SENSOR_ID_BITS 3
#define SENSOR_TIMESTAMP_BITS SENSOR_METADATA_BITS - SENSOR_ID_BITS

// Serial port which interfaces with Feather M4 Express
#define SERIAL_FEATHER "/dev/ttyS1"

// Max characters for line buffer
#define MAX_CHARS 256

// Sensor ID Config
const std::string SENSOR_DATA_PATH = "./data/"; // This directory must exist before running the program

#define THERMOCOUPLE 1
#define SPECTROMETER 2
#define ACCELEROMETER 3

// Validity parameters
// Check for erroneous data 
#define TC_LOW -1000
#define TC_MAX 10000

#define ACC_LOW -100
#define ACC_HIGH 100

// Data Selection Config
#define BYTE_TARGET PACKET_SIZE
#define NEW_DATA_RATIO 0.8
#define POINT_INCLUDE_LIMIT 1

#define DEBUG // comment/uncomment to enable/disable debugging logs
#ifdef DEBUG
// #define PRINT_DATA 
#define PACKET_P
#define VALIDITY_P
#endif

#endif