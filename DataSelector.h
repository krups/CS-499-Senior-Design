#include "config.h"
#include "DataPoint.h"
#include <vector>
#include <unordered_map>

class DataSelector {
  public:
    std::vector<DataPoint> SelectData();

  private:
    void updateDataPoints();
    
    std::unordered_map<sensor_id_t, std::vector<DataPoint>> dataPoints;
    std::unordered_map<sensor_id_t, int> lastDataPointIndex;
  
};