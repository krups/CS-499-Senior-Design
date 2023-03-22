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
    unsigned int selectDataPointsGradient(unsigned int sensorId, unsigned int numData, std::vector<DataPoint*>* tempDataPointList, unsigned int startInclusive, unsigned int endExclusive, double offset);
    unsigned int selectDataPointsIndex(unsigned int sensorId, unsigned int numData, std::vector<DataPoint*>* tempDataPointList, unsigned int startInclusive, unsigned int endExclusive, double offset);
    std::vector<int> getDataPointValues(char *buffer, SensorSettings *sensorSettings);
    unsigned int calculateGradientValue(std::vector<int> dataValuesBefore, std::vector<int> dataValues, std::vector<int> dataValuesAfter, bool firstPoint, bool lastPoint);

    SensorMap* sensors;
    
    unsigned int sensorPriorityLCM;
    std::unordered_map<unsigned int, unsigned int> sensorRelativeSpacing;

    std::vector<DataPoint*>* currentData;

    unsigned int nextDataPointID;

    std::unordered_map<unsigned int, std::vector<DataPoint>*> dataPoints;
    std::unordered_map<unsigned int, unsigned int> nextUnusedDataPointIndex;
    std::unordered_map<unsigned int, std::streampos> lastDataPointReadIndex;

};