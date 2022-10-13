#ifndef SERIAL_HEADERS_H
#define SERIAL_HEADERS_H

///////////////////////////
//    Sensor Headers 
///////////////////////////

// Spectrometers
#define SPECTROMETER_1_SERIAL 0x101
// #define SPECTROMETER_2_SERIAL 0x102

// TCs 
#define TC_SERIAL 0x201

// IMU
#define IMU_SERIAL 0x301

// GPS/RMC
#define GPS_SERIAL 0x401
#define RMC_SERIAL 0x402

// ACC
#define ACC_SERIAL 0x501

///////////////////////////
//    Command Headers 
///////////////////////////

// Packet Request from Feather M4
#define PACKET_REQUEST 0x801


///////////////////////////
//    Misc Headers 
///////////////////////////

// Misc debug
#define MISC_SERIAL 0x999

#endif