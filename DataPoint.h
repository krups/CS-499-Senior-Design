#ifndef DATA_POINT_H
#define DATA_POINT_H

struct DataPoint {
  std::streampos fileIndex;
  unsigned int ID;
  unsigned int sensor_id;
  unsigned int numIncludes;
  unsigned int gradient;
  bool used;
};

#endif