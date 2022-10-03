#ifndef CONFIG_H
#define CONFIG_H

// Serial port which interfaces with Feather M4 Express
#define SERIAL_FEATHER "/dev/ttyS1"

// Prints data being received if set to 1, disables when set to 0
#define PRINT_DATA 1

#define DEBUG // comment/uncomment to enable/disable debugging logs
#ifdef DEBUG
// add toggles for feature debugging

#endif

#endif