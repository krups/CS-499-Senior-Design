#include "DataSelector.h"

// DataSelector::DataSelector() {

// }

// DataSelector::~DataSelector() {

// }

void DataSelector::updateDataPoints() {
  for (unsigned int sensorIndex = 0; sensorIndex < sensors.list.size(); sensorIndex++) {
    Sensor* currentSensor = &sensors.list[sensorIndex];

    std::ifstream sensorFile;
    std::string path = SENSOR_DATA_PATH;
    path += std::to_string(currentSensor->id);

    // This is where I would put the semaphore wait if we were using per-sensor file semaphores

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

    // This is where I would put the semaphore signal if we were using per-sensor file semaphores
  }
}

std::vector<DataPoint*> DataSelector::selectData() {
  updateDataPoints();

  if (currentData.size() != 0) {
    previousData = currentData;
  }

  unsigned int totalPriority = 0;

  unsigned int sensorListSize = sensors.list.size();
  for (unsigned int i = 0; i < sensorListSize; i++) {
    totalPriority += sensors.list[i].priority;
  }

  std::unordered_map<sensor_id_t, std::vector<DataPoint*>> tempDataPointList;
  std::vector<DataPoint*> dataPointList;

  unsigned int sensorIndex;
  for (sensorIndex = 0; sensorIndex < sensorListSize; sensorIndex++) {
    Sensor* currentSensor = &sensors.list[sensorIndex];

    unsigned int targetByteCount = BYTE_TARGET * (currentSensor->priority / totalPriority);

    unsigned int numDataPoints = targetByteCount / currentSensor->numBytes;

    unsigned int numNewData = numDataPoints * NEW_DATA_RATIO;
    unsigned int newDataSpacing = (dataPoints[currentSensor->id].size() - lastDataPointUsedIndex[currentSensor->id]) / numNewData;
    newDataSpacing = std::max(1, newDataSpacing);
    unsigned int sensorDataSize = dataPoints[currentSensor->id].size();
    for (unsigned int dataPointIndex = lastDataPointUsedIndex[currentSensor->id] + newDataSpacing; dataPointIndex < sensorDataSize; dataPointIndex += newDataSpacing) {
      tempDataPointList[currentSensor->id].push_back(&dataPoints[currentSensor->id][dataPointIndex]);
    }

    unsigned int numOldData = numDataPoints - numNewData;
    unsigned int oldDataSpacing = lastDataPointUsedIndex[currentSensor->id] / numNewData;
    oldDataSpacing = std::max(1, oldDataSpacing);
    for (unsigned int dataPointIndex = 0; dataPointIndex < lastDataPointUsedIndex[currentSensor->id]; dataPointIndex += oldDataSpacing) {
      tempDataPointList[currentSensor->id].push_back(&dataPoints[currentSensor->id][dataPointIndex]);
    }
  }

  std::unordered_map<sensor_id_t, unsigned int> nextPointPerSensor;
  for (sensorIndex = 0; sensorIndex < sensorListSize; sensorIndex++) {
    Sensor* currentSensor = &sensors.list[sensorIndex];

    nextPointPerSensor[currentSensor->id] = 0;
  }

  sensorIndex = 0;
  bool moreData = true;
  while (moreData) {
    moreData = false;

    for (sensorIndex = 0; sensorIndex < sensorListSize; sensorIndex++) {
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
  unsigned int previousDataSize = previousData.size();
  for (unsigned int previousDataIndex = 0; previousDataIndex < previousDataSize; previousDataIndex++) {
    DataPoint* targetDataPoint = previousData[previousDataIndex];

    targetDataPoint->numIncludes++;

    if (targetDataPoint->numIncludes > POINT_INCLUDE_LIMIT) {
      unsigned int sensorDataSize = dataPoints[targetDataPoint->sensor_id].size();
      for (unsigned int sensorDataIndex; sensorDataIndex < sensorDataSize; sensorDataIndex++) {
        if (&dataPoints[targetDataPoint->sensor_id][sensorDataIndex] == targetDataPoint) {
          std::vector<DataPoint>::iterator sensorDataIterator = vec.begin();
          std::advance(sensorDataIterator, sensorDataIndex);
          dataPoints[targetDataPoint->sensor_id].erase(sensorDataIterator);
          break;
        }
      }
    }
  }
}