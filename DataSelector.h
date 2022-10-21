#include <vector>
#include <unordered_map>
#include "config.h"
#include "DataPoint.h"

class DataSelector {
  public:
    std::vector<DataPoint*> selectData();

    void markDataPoints(std::vector<DataPoint*> points);

  private:
    void updateDataPoints();

    std::unordered_map<sensor_id_t, std::vector<DataPoint>> dataPoints;
    std::unordered_map<sensor_id_t, int> lastDataPointUsedIndex;

};