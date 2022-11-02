#ifndef DATA_POINT_H
#define DATA_POINT_H

struct DataPoint {
  uint16_t fileIndex;
  sensor_timestamp_t timestamp;
  sensor_id_t sensor_id;
  uint8_t numIncludes;
};

#endif