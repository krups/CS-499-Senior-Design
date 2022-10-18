#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Sensor {
  sensor_id_t id;
  uint8_t priority;
};

std::vector<Sensor> activeSensors;

// Serial port which interfaces with Feather M4 Express
#define SERIAL_FEATHER "/dev/ttyS1"

// Max characters for line buffer
#define MAX_CHARS 256

// Sensor ID Config
typedef uint8_t sensor_id_t;
typedef uint16_t sensor_timestamp_t;
const std::string sensorDataPath = "./data/"; // This directory must exist before running the program

#define THERMOCOUPLE 1
#define SPECTROMETER 2
#define ACCELEROMETER 3

// Active Sensors
activeSensors.push_back(Sensor(THERMOCOUPLE, 1));
activeSensors.push_back(Sensor(SPECTROMETER, 1));
activeSensors.push_back(Sensor(ACCELEROMETER, 1));

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