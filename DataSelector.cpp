#include "DataSelector.h"

void DataSelector::updateDataPoints() {
  for (unsigned int sensorId = 0; sensorId < sensors.list.size(); sensorId++) {
    Sensor* currentSensor = &sensors.list[sensorId];

    // go to last data point used, open file, go to that line, see if it continues, if it does add data point info until end
  }
}

std::vector<DataPoint*> DataSelector::selectData() {
  updateDataPoints();

  unsigned int totalPriority = 0;

  for (unsigned int i = 0; i < sensors.list.size(); i++) {
    totalPriority += sensors.list[i].priority;
  }

  std::unordered_map<sensor_id_t, std::vector<DataPoint*>> tempDataPointList;
  std::vector<DataPoint*> dataPointList;

  for (unsigned int sensorId = 0; sensorId < sensors.list.size(); sensorId++) {
    Sensor* currentSensor = &sensors.list[sensorId];

    unsigned int targetByteCount = BYTE_TARGET * (currentSensor->priority / totalPriority);

    unsigned int numDataPoints = targetByteCount / currentSensor->numBytes;

    unsigned int numNewDataPoints = numDataPoints * NEW_DATA_RATIO;
    unsigned int newDataPointSpacing = (dataPoints[currentSensor->id].size() - lastDataPointUsedIndex[currentSensor->id]) / numNewDataPoints;
    for (unsigned int dataPointIndex = lastDataPointUsedIndex[currentSensor->id] + newDataPointSpacing; dataPointIndex += newDataPointSpacing; dataPointIndex < dataPoints[currentSensor->id].size()) {
      tempDataPointList[currentSensor->id].push_back(&dataPoints[currentSensor->id][dataPointIndex]);
    }

    unsigned int numOldDataPoints = numDataPoints - numNewDataPoints;
    unsigned int oldDataPointSpacing = lastDataPointUsedIndex[currentSensor->id] / numNewDataPoints;
    for (unsigned int dataPointIndex = 0; dataPointIndex += oldDataPointSpacing; dataPointIndex < lastDataPointUsedIndex[currentSensor->id]) {
      tempDataPointList[currentSensor->id].push_back(&dataPoints[currentSensor->id][dataPointIndex]);
    }
  }

  // INTERLEAVE ALL OF THE TEMP VECTORS INTO MAIN VECTOR

  return dataPointList;
}

void DataSelector::markDataPoints(std::vector<DataPoint*> points) {
  // for each data point in the given vector, increment the numIncludes by 1 and then remove from list if used more than config value

  // update lastDataPointUsedIndex to reflect changes
}