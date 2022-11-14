#ifndef DATA_POINT_H
#define DATA_POINT_H

struct DataPoint {
  std::streampos fileIndex;
  sensor_id_t sensor_id;
  uint8_t numIncludes;
  double gradient;
  bool used;
};

#endif