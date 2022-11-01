#include <vector>
#include <unordered_map>
#include <ifstream>
#include "config.h"
#include "DataPoint.h"

class DataSelector {
  public:
    DataSelector();
    ~DataSelector();
    std::vector<DataPoint*>* selectData();
    void markUsed();

  private:
    void updateDataPoints();

    unsigned int totalSensorPriority;
    std::vector<DataPoint*>* currentData;
    std::vector<DataPoint*>* previousData;

    std::unordered_map<sensor_id_t, std::vector<DataPoint>*> dataPoints;
    std::unordered_map<sensor_id_t, unsigned int> lastDataPointUsedIndex;

};