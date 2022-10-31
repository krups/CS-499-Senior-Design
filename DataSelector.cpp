#include "DataSelector.h"

void DataSelector::updateDataPoints() {
  for (unsigned int sensorIndex = 0; sensorIndex < sensors.list.size(); sensorIndex++) {
    Sensor* currentSensor = &sensors.list[sensorIndex];

    std::ifstream sensorFile;
    std::string path = sensorDataPath;
    path += std::to_string(currentSensor->id);

    // SEMAPHORE WAIT

    sensorFile.open(path, std::ios_base::binary);
    if (!sensorFile.fail()) {
      std::string row;

      if (dataPoints[currentSensor->id].size() != 0) {
        sensorFile.seekg(dataPoints[currentSensor->id][dataPoints[currentSensor->id].size() - 1].fileIndex, std::ios_base::binary);
        getline(sensorFile, row);
      }
      
      while (!sensorFile.eof()) {
        DataPoint newDataPoint;
        newDataPoint.sensor_id = currentSensor->id;
        newDataPoint.numIncludes = 0;
        newDataPoint.fileIndex = sensorFile.tellg();

        getline(sensorFile, row);

        newDataPoint.timestamp = (uint16_t) *(row.c_str() + sizeof(sensor_id_t));

        dataPoints[currentSensor->id].push_back(newDataPoint);
      }
    }

    sensorFile.close();

    // SEMAPHORE SIGNAL
  }
}

std::vector<DataPoint*> DataSelector::selectData() {
  updateDataPoints();

  if (currentData.size() != 0) {
    previousData = currentData;
  }

  unsigned int totalPriority = 0;

  for (unsigned int i = 0; i < sensors.list.size(); i++) {
    totalPriority += sensors.list[i].priority;
  }

  std::unordered_map<sensor_id_t, std::vector<DataPoint*>> tempDataPointList;
  std::vector<DataPoint*> dataPointList;

  unsigned int sensorIndex;
  for (sensorIndex = 0; sensorIndex < sensors.list.size(); sensorIndex++) {
    Sensor* currentSensor = &sensors.list[sensorIndex];

    unsigned int targetByteCount = BYTE_TARGET * (currentSensor->priority / totalPriority);

    unsigned int numDataPoints = targetByteCount / currentSensor->numBytes;

    unsigned int numNewData = numDataPoints * NEW_DATA_RATIO;
    unsigned int newDataSpacing = (dataPoints[currentSensor->id].size() - lastDataPointUsedIndex[currentSensor->id]) / numNewData;
    for (unsigned int dataPointIndex = lastDataPointUsedIndex[currentSensor->id] + newDataSpacing; dataPointIndex += newDataSpacing; dataPointIndex < dataPoints[currentSensor->id].size()) {
      tempDataPointList[currentSensor->id].push_back(&dataPoints[currentSensor->id][dataPointIndex]);
    }

    unsigned int numOldData = numDataPoints - numNewData;
    unsigned int oldDataSpacing = lastDataPointUsedIndex[currentSensor->id] / numNewData;
    for (unsigned int dataPointIndex = 0; dataPointIndex += oldDataSpacing; dataPointIndex < lastDataPointUsedIndex[currentSensor->id]) {
      tempDataPointList[currentSensor->id].push_back(&dataPoints[currentSensor->id][dataPointIndex]);
    }
  }

  std::unordered_map<sensor_id_t, unsigned int> nextPointPerSensor;
  for (sensorIndex = 0; sensorIndex < sensors.list.size(); sensorIndex++) {
    Sensor* currentSensor = &sensors.list[sensorIndex];

    nextPointPerSensor[currentSensor->id] = 0;
  }

  sensorIndex = 0;
  bool moreData = true;
  while (moreData) {
    moreData = false;

    for (sensorIndex = 0; sensorIndex < sensors.list.size(); sensorIndex++) {
      Sensor* currentSensor = &sensors.list[sensorIndex];

      unsigned int pointNum = 0;
      while (pointNum < currentSensor->priority && nextPointPerSensor[currentSensor->id] < tempDataPointList[currentSensor->id].size()) {
        dataPointList.push_back(tempDataPointList[currentSensor->id][nextPointPerSensor[currentSensor->id]]);
        nextPointPerSensor[currentSensor->id] += 1;
        pointNum++;
      }

      if (nextPointPerSensor[currentSensor->id] < tempDataPointList[currentSensor->id].size()) {
        moreData = true;
      }
    }
  }

  return dataPointList;
}

void DataSelector::markUsed() {
  // for each data point in the previousData vector, increment the numIncludes by 1 and then remove from list if used more than config value

  // update lastDataPointUsedIndex to reflect changes
}