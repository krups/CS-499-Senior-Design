#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include "Sensors.h"

SensorList sensors;

// Serial port which interfaces with Feather M4 Express
#define SERIAL_FEATHER "/dev/ttyS1"

// Max characters for line buffer
#define MAX_CHARS 256

// Sensor ID Config

const std::string sensorDataPath = "./data/"; // This directory must exist before running the program

#define THERMOCOUPLE 1
#define SPECTROMETER 2
#define ACCELEROMETER 3

// Active Sensors
sensors.addSensor(THERMOCOUPLE, 1);
sensors.addSensor(SPECTROMETER, 1);
sensors.addSensor(ACCELEROMETER, 1);

// Data Selection Config
#define BYTE_TARGET 500
#define NEW_DATA_RATIO 0.8
#define POINT_INCLUDE_LIMIT 1

// Somehow have a way to indicate which sensors are used and priorities

#define DEBUG // comment/uncomment to enable/disable debugging logs
#ifdef DEBUG
// add toggles for feature debugging

// Prints data being received
// #define PRINT_DATA
#define VALIDITY
#endif

// Validity parameters
// Check for erroneous data 

#define TC_LOW -1000
#define TC_MAX 10000

#define ACC_LOW -100
#define ACC_HIGH 100

#endif