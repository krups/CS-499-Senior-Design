#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include "Sensors.h"

//////////////////////////////
//
//    Packet Configuration
//
//////////////////////////////
// Packet size in bytes
#define PACKET_SIZE 340
#define PACKET_SIZE_BITS (PACKET_SIZE * 8)

// Sensor metadata size in bytes
#define SENSOR_ID_BITS 4
#define SENSOR_TIMESTAMP_BITS 12
#define SENSOR_METADATA_BITS (SENSOR_ID_BITS + SENSOR_TIMESTAMP_BITS)

// Delay for creating packets in seconds 
#define PACKET_DELAY 0

// File Paths 
const std::string SENSOR_DATA_PATH = "./data/"; // This directory must exist before running the program, must end with /
const std::string PACKET_DATA_PATH = "./packets/"; // This directory must exist before running the program, must end with /

// Serial port which interfaces with Feather M4 Express
#define SERIAL_FEATHER "/dev/ttyS1"

// Max characters for line buffer
#define MAX_CHARS 256

//////////////////////////////
//
//    Sensor Configuration
//
//////////////////////////////

// GPS 
#define GPS_ID 1
#define GPS_PRIORITY 1
#define GPS_NUM_SAMPLES_PER_DATA_POINT 5
#define GPS_NUM_BITS_PER_SAMPLE 16
#define GPS_OFFSET 0
#define GPS_MULT 1

// RMC 
#define RMC_ID 2
#define RMC_PRIORITY 1
#define RMC_NUM_SAMPLES_PER_DATA_POINT 5
#define RMC_NUM_BITS_PER_SAMPLE 16
#define RMC_OFFSET 0
#define RMC_MULT 1

// ACC sensor
#define ACC_ID 3
#define ACC_PRIORITY 1
#define ACC_NUM_SAMPLES_PER_DATA_POINT 3
#define ACC_NUM_BITS_PER_SAMPLE 8
#define ACC_OFFSET 0
#define ACC_MULT 1

// IMU Sensor
#define IMU_ID 4
#define IMU_PRIORITY 1
#define IMU_NUM_SAMPLES_PER_DATA_POINT 6
#define IMU_NUM_BITS_PER_SAMPLE 16
#define IMU_OFFSET 0
#define IMU_MULT 1

// TC Sensors
#define TC_ID 5
#define TC_PRIORITY 1
#define TC_NUM_SAMPLES_PER_DATA_POINT 4
#define TC_NUM_BITS_PER_SAMPLE 11
#define TC_OFFSET (-1 * TC_LOW)
#define TC_MULT 1

// PRES sensors
#define PRES_ID 6
#define PRES_PRIORITY 1
#define PRES_NUM_SAMPLES_PER_DATA_POINT 5
#define PRES_NUM_BITS_PER_SAMPLE 11
#define PRES_OFFSET 0
#define PRES_MULT 10

// Spectrometer
#define SPEC_ID 7
#define SPEC_PRIORITY 1
#define SPEC_NUM_SAMPLES_PER_DATA_POINT 5
#define SPEC_NUM_BITS_PER_SAMPLE 4
#define SPEC_OFFSET 0
#define SPEC_MULT 1


//////////////////////////////
//
//    Validity Configuration
//
//////////////////////////////
// Make sure to check values 
// in the checkValid() 
// function in main.cpp
#define TC_LOW -1000
#define TC_MAX 10000

#define ACC_LOW 0
#define ACC_HIGH 15

////////////////////////////////////
//
//    Data Selection Configuration
//
////////////////////////////////////
#define NEW_DATA_RATIO 0.8
#define POINT_INCLUDE_LIMIT 1

#define NEW_DATA_GRADIENT_SELECT false
#define OLD_DATA_GRADIENT_SELECT false

//////////////////////////////
//
//    Debug Configuration 
//
//////////////////////////////
#define DEBUG // comment/uncomment to enable/disable debugging logs
#ifdef DEBUG
// #define PRINT_DATA
//#define PACKET_P
#define VALIDITY_P
// #define DEBUG_P
//#define DATA_SEL_P // debug statements in DataSelector.cpp
// #define DATA_P // debug statements in data.cpp
#endif

#endif
