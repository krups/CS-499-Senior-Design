#ifndef CONFIG_H
#define CONFIG_H

#include <string>

// Serial port which interfaces with Feather M4 Express
#define SERIAL_FEATHER "/dev/ttyS1"

// Max characters for line buffer
#define MAX_CHARS 256

// Sensor ID Config
typedef char sensor_id_t;
const std::string sensorDataPath = "./data/"; // This directory must exist before running the program

#define DEBUG // comment/uncomment to enable/disable debugging logs
#ifdef DEBUG
// add toggles for feature debugging

// Prints data being received
#define PRINT_DATA
#define VALIDITY
#endif

// Validity parameters
// Check for erroneous data 

#define TC_LOW -1000
#define TC_MAX 10000

#endif