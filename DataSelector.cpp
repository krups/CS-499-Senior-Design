#include "DataSelector.h"

DataSelector::DataSelector(SensorMap* sensors) {
  // Allocate memory and set initial variable values
  totalSensorPriority = 0;

  sensors = sensors;

  for (auto [sensorId, sensorSettings] : sensors->sensorMap) {
    dataPoints[sensorId] = new std::vector<DataPoint>;
    lastDataPointUsedIndex[sensorId] = 0;
    totalSensorPriority += sensorSettings->priority;
  }
}

DataSelector::~DataSelector() {
  // Deallocate allocated memory
  for (auto [sensorId, sensorSettings] : sensors->sensorMap) {
    free(dataPoints[sensorId]);
  }

  if (currentData != nullptr) {
    free(currentData);
  }

  if (previousData != nullptr) {
    free(previousData);
  }
}

// Used to update the values stored in dataPoints
void DataSelector::updateDataPoints() {
  // For each sensor
  for (auto [sensorId, sensorSettings] : sensors->sensorMap) {
    // Get the path to the file that stores that sensor's data
    std::ifstream sensorFile;
    std::string path = SENSOR_DATA_PATH;
    path += std::to_string(sensorId);

    // This is where I would put the semaphore wait if we were using per-sensor file semaphores

    // Open the file for that sensor's data
    sensorFile.open(path, std::ios_base::binary);
    if (!sensorFile.fail()) {
      std::string row;

      // If there were previously tracked data points
      if (dataPoints[sensorId]->size() != 0) {
        // Open the file to the last known data point
        sensorFile.seekg((*dataPoints[sensorId])[dataPoints[sensorId]->size() - 1].fileIndex, std::ios_base::beg);
        // And discard the last data point so the read pointer is at the start of the next data point
        getline(sensorFile, row);
      }
      
      // Until the end of the file is reached
      while (!sensorFile.eof()) {
        // Create a new DataPoint class and initialize its values
        DataPoint newDataPoint;
        newDataPoint.sensor_id = sensorId;
        newDataPoint.numIncludes = 0;
        newDataPoint.fileIndex = sensorFile.tellg();

        // Read the information for this data point
        getline(sensorFile, row);

        // Use the file data to extract the timestamp
        newDataPoint.timestamp = (uint16_t) *(row.c_str() + sizeof(sensor_id_t));

        // Add the new DataPoint to the vector of data points for that sensor
        dataPoints[sensorId]->push_back(newDataPoint);
      }
    }

    // Close the file
    sensorFile.close();

    // This is where I would put the semaphore signal if we were using per-sensor file semaphores
  }
}

// Called by the packet building thread to select data points to include in the nexxt packet
std::vector<DataPoint*>* DataSelector::selectData() {
  // First, update the vectors of data points that are tracked in memory and used for data selection
  updateDataPoints();

  // Track the previously generated vector of data points
  if (previousData != nullptr) {
    free(previousData);
  }
  previousData = currentData;

  // Make an unordered map of ints to track how many data points go to each sensor
  std::unordered_map<sensor_id_t, unsigned int> pointsPerSensor;
  for (unsigned int sensorIndex = 0; sensorIndex < sensorListSize; sensorIndex++) {
    pointsPerSensor[sensorsList->list[sensorIndex].id] = 0;
  }

  // Loop while allocating points to each sensor until every possible bit has been used
  unsigned int usedSpace = 0;
  bool moreSpace = true; // Make this actually become false or change looping condition when fully made
  while (moreSpace) {
    
  }

  // Create a temporary unordered map of vectors of all data points that are chosen for the next packet
  std::unordered_map<sensor_id_t, std::vector<DataPoint*>*> tempDataPointList;
  
  // Initialize the unordered map's vectors
  for (auto [sensorId, sensorSettings] : sensors->sensorMap) {
    tempDataPointList[sensorId] = new std::vector<DataPoint*>;
  }

  // For every sensor
  for (auto [sensorId, sensorSettings] : sensors->sensorMap) {

    // NUM NEW DATA CALCULATED HERE
    // NUM OLD DATA CALCULATED HERE
    
    // Calculate the index increment to allow that number of data points to be evenly spaced across time (from the first recorded data point to the last used data point)
    unsigned int oldDataSpacing = lastDataPointUsedIndex[sensorId] / numOldData;
    oldDataSpacing = std::max((unsigned int) 1, oldDataSpacing);
    // Iterate through the old data points using the calculated increment and add them to the temporary vector
    for (unsigned int dataPointIndex = 0; dataPointIndex < lastDataPointUsedIndex[sensorId]; dataPointIndex += oldDataSpacing) {
      tempDataPointList[sensorId]->push_back(&(*dataPoints[sensorId])[dataPointIndex]);
    }
    
    // Calculate the index increment to allow that number of data points to be evenly spaced across time (from the last used data point to the last recorded data point)
    unsigned int newDataSpacing = (dataPoints[sensorId]->size() - lastDataPointUsedIndex[sensorId]) / numNewData;
    newDataSpacing = std::max((unsigned int) 1, newDataSpacing);
    // Iterate through the new data points using the calculated increment and add them to the temporary vector
    unsigned int sensorDataSize = dataPoints[sensorId]->size();
    for (unsigned int dataPointIndex = lastDataPointUsedIndex[sensorId] + newDataSpacing; dataPointIndex < sensorDataSize; dataPointIndex += newDataSpacing) {
      tempDataPointList[sensorId]->push_back(&(*dataPoints[sensorId])[dataPointIndex]);
    }
  }

  // Make a temporary unordered map of integers to track the last used data point from each sensor's temporary vector
  std::unordered_map<sensor_id_t, unsigned int> nextPointPerSensor;
  for (auto [sensorId, sensorSettings] : sensors->sensorMap) {
    // Initialize the value to 0
    nextPointPerSensor[sensorId] = 0;
  }

  // Create the vector of sensor data that is returned
  std::vector<DataPoint*>* dataPointList = new std::vector<DataPoint*>;

  // Until all data in the temporary vectors of sensor data have been added to the returned vector
  bool moreData = true;
  while (moreData) {
    moreData = false;

    // The following code is basically a round-robin method of adding different sensors' data points
    // to the returned vector, except that the number of points from each sensor added to the final vector
    // in each iteration is weighted by the sensor's priority

    // For each sensor
    for (auto [sensorId, sensorSettings] : sensors->sensorMap) {
      // Until a number of data points equal to the sensor's priority have been added to the returned vector
      // or until the sensor has no more data points left to add
      unsigned int pointNum = 0;
      unsigned int tempDataPointListSize = tempDataPointList[sensorId]->size();
      while (pointNum < sensorSettings->priority && nextPointPerSensor[sensorId] < tempDataPointListSize) {
        // Add the next data point to the returned vector
        dataPointList->push_back((*tempDataPointList[sensorId])[nextPointPerSensor[sensorId]]);
        // Increment the counts
        nextPointPerSensor[sensorId] += 1;
        pointNum++;
      }

      // If there are more data points to add from this sensor
      if (nextPointPerSensor[sensorId] < tempDataPointList[sensorId]->size()) {
        // Indicate that more data must be added (continuing the loop)
        moreData = true;
      }
    }
  }

  // Free allocated memory for temporary data point vectors for each sensor
  for (auto [sensorId, sensorSettings] : sensors->sensorMap) {
    free(tempDataPointList[sensorId]);
  }

  // Track the currently generated list of data points
  currentData = dataPointList;

  // Return the list
  return dataPointList;
}

// Called by the packet building thread to indicate that the previous list of data points was used
void DataSelector::markUsed() {
  // For each data point in the previous list of data points
  unsigned int previousDataSize = previousData->size();
  for (unsigned int previousDataIndex = 0; previousDataIndex < previousDataSize; previousDataIndex++) {
    DataPoint* targetDataPoint = (*previousData)[previousDataIndex];

    // Increment the number of times that this data point has been used
    targetDataPoint->numIncludes++;

    // If the number of times that this data point has been used exceeds the configured limit
    if (targetDataPoint->numIncludes > POINT_INCLUDE_LIMIT) {
      // Loop through the vector of data points for the sensor that this data point is associated with to find the index of that data point
      unsigned int sensorDataSize = dataPoints[targetDataPoint->sensor_id]->size();
      for (unsigned int sensorDataIndex; sensorDataIndex < sensorDataSize; sensorDataIndex++) {
        // If the pointers match, then the object has been found
        if ((void*) &((*dataPoints[targetDataPoint->sensor_id])[sensorDataIndex]) == (void*) targetDataPoint) {
          // Use the index that was counted while searching to remove the item from the vector of data points for that sensor
          std::vector<DataPoint>::iterator sensorDataIterator = dataPoints[targetDataPoint->sensor_id]->begin();
          std::advance(sensorDataIterator, sensorDataIndex);
          dataPoints[targetDataPoint->sensor_id]->erase(sensorDataIterator);

          // If the index of the removed point was greater than the last used data point from this sensor
          if (sensorDataIndex > lastDataPointUsedIndex[targetDataPoint->sensor_id]) {
            // Update the last used data point index for this sensor, accounting for the fact that a point was removed
            lastDataPointUsedIndex[targetDataPoint->sensor_id] = sensorDataIndex - 1;
          } else {
            // Otherwise, the index of the last used data point to reflect that an item was removed
            lastDataPointUsedIndex[targetDataPoint->sensor_id] -= 1;
          }
          
          // Exit the inner loop
          break;
        }
      }
    }
  }
}