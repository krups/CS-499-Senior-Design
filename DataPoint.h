#ifndef DATA_POINT_H
#define DATA_POINT_H

struct DataPoint {
  std::streampos fileIndex;
  int sensor_id;
  int numIncludes;
  unsigned int gradient;
  bool used;
};

#endif