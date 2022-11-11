#ifndef SERIAL_HEADERS_H
#define SERIAL_HEADERS_H

///////////////////////////
//    Sensor Headers 
///////////////////////////

// Spectrometers
#define SPECTROMETER_1_SERIAL 0xA
// #define SPECTROMETER_2_SERIAL 0x102

// TCs 
#define TC_SERIAL 0x1

// IMU
#define IMU_SERIAL 0x2

// GPS/RMC
#define GPS_SERIAL 0x3
#define RMC_SERIAL 0x4

// ACC
#define ACC_SERIAL 0x5

///////////////////////////
//    Command Headers 
///////////////////////////

// Packet Request from Feather M4
#define PACKET_REQUEST 0x8


///////////////////////////
//    Misc Headers 
///////////////////////////

// Misc debug
#define MISC_SERIAL 0x9

#endif