#include <cstring>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <numeric>
#include "config.h"
#include "Sensors.h"
#include "DataPoint.h"

class DataSelector {
  public:
    DataSelector();
    DataSelector(SensorMap* sensors);
    ~DataSelector();
    std::vector<DataPoint*>* selectData();
    void markUsed();

  private:
    void updateDataPoints();
    unsigned int selectDataPointsGradient(sensor_id_t sensorId, unsigned int numData, std::vector<DataPoint*>* tempDataPointList, unsigned int start, unsigned int end, float offset);
    unsigned int selectDataPointsIndex(sensor_id_t sensorId, unsigned int numData, std::vector<DataPoint*>* tempDataPointList, unsigned int start, unsigned int end, float offset);

    SensorMap* sensors;
    
    unsigned int sensorPriorityLCM;
    std::unordered_map<sensor_id_t, unsigned int> sensorRelativeSpacing;

    std::vector<DataPoint*>* currentData;
    std::vector<DataPoint*>* previousData;

    std::unordered_map<sensor_id_t, std::vector<DataPoint>*> dataPoints;
    std::unordered_map<sensor_id_t, unsigned int> nextUnusedDataPointIndex;

};