#include <stdio.h>
#include <string>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <fstream>

#ifndef CONFIG_H
#define CONFIG_H

// Serial port which interfaces with Feather M4 Express
#define SERIAL_FEATHER "/dev/ttyS1"

// Which hex should be seen as a command
#define COMMAND 0x301

// Max characters for line buffer
#define MAX_CHARS 256

// Sensor ID Config
typedef sensor_id char;
const string sensorDataPath = "./data/"; // This directory must exist before running the program

#define DEBUG // comment/uncomment to enable/disable debugging logs
#ifdef DEBUG
// add toggles for feature debugging

// Prints data being received
#define PRINT_DATA

#endif

#endif