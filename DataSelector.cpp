#include "DataSelector.h"

DataSelector::DataSelector() {
  sensorPriorityLCM = 1;
}

DataSelector::DataSelector(SensorMap* sensors) {
  // Allocate memory and set initial variable values
  this->sensors = sensors;
  sensorPriorityLCM = 1;

  for (auto [sensorId, sensorSettings] : sensors->sensorMap) {
    dataPoints[sensorId] = new std::vector<DataPoint>;
    nextUnusedDataPointIndex[sensorId] = 0;
    sensorPriorityLCM = std::lcm(sensorPriorityLCM, sensorSettings->priority);
  }

  for (auto [sensorId, sensorSettings] : sensors->sensorMap) {
    sensorRelativeSpacing[sensorId] = sensorPriorityLCM / sensorSettings->priority;
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

    unsigned int bufferSize = (sensorSettings->numBitsPerDataPoint + 7) / 8; // This rounds up to next byte threshold
    char* buffer = new char[bufferSize];

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
        memset(buffer, '\0', bufferSize);
        sensorFile.read(buffer, bufferSize);
      }
      
      // Until the end of the file is reached
      while (sensorFile) {
        // Create a new DataPoint class and initialize its values
        DataPoint newDataPoint;
        newDataPoint.sensor_id = sensorId;
        newDataPoint.numIncludes = 0;
        newDataPoint.fileIndex = sensorFile.tellg();
        newDataPoint.used = false;

        // Read the data so the read pointer advances
        memset(buffer, '\0', bufferSize);
        sensorFile.read(buffer, bufferSize);

        newDataPoint.gradient = 1;

        // Add the new DataPoint to the vector of data points for that sensor
        dataPoints[sensorId]->push_back(newDataPoint);
      }
    }

    // Close the file
    sensorFile.close();

    // This is where I would put the semaphore signal if we were using per-sensor file semaphores
  }
}

unsigned int DataSelector::selectDataPointsGradient(sensor_id_t sensorId, unsigned int numData, std::vector<DataPoint*>* tempDataPointList, unsigned int startInclusive, unsigned int endExclusive) {
  double totalNewGradient = 0;
    
  for (unsigned int dataPointIndex = startInclusive; dataPointIndex < endExclusive; dataPointIndex++) {
    totalNewGradient += (*dataPoints[sensorId])[dataPointIndex].gradient;
  }

  double newGradientSpacing = totalNewGradient / numData;

  unsigned int numPointsSelected = 0;

  bool allDataUsed = false;

  for (unsigned int index = 1; index <= numData; index++) {
    if (allDataUsed == true) {
      break;
    }

    bool pointPicked = false;

    double targetGradient = newGradientSpacing * index;

    for (unsigned int dataPointIndex = startInclusive; dataPointIndex < endExclusive; dataPointIndex++) {
      if (pointPicked == true || allDataUsed == true) {
        break;
      }

      targetGradient -= (*dataPoints[sensorId])[dataPointIndex].gradient;

      if (targetGradient <= 0) {
        if ((*dataPoints[sensorId])[dataPointIndex].used == false) {
          (*dataPoints[sensorId])[dataPointIndex].used = true;
          tempDataPointList->push_back(&(*dataPoints[sensorId])[dataPointIndex]);
          numPointsSelected++;
        } else {
          for (unsigned int retryIndex = 1; retryIndex < numData - 1; retryIndex++) {
            unsigned int retryDataPointIndexUp = dataPointIndex + retryIndex;
            unsigned int retryDataPointIndexDown = dataPointIndex - retryIndex;

            if (retryDataPointIndexUp < endExclusive) {
              if ((*dataPoints[sensorId])[retryDataPointIndexUp].used == false) {
                (*dataPoints[sensorId])[retryDataPointIndexUp].used = true;
                tempDataPointList->push_back(&(*dataPoints[sensorId])[retryDataPointIndexUp]);
                numPointsSelected++;
                pointPicked = true;
                break;
              }
            }
            
            if (retryDataPointIndexDown >= startInclusive) {
              if ((*dataPoints[sensorId])[retryDataPointIndexDown].used == false) {
                (*dataPoints[sensorId])[retryDataPointIndexDown].used = true;
                tempDataPointList->push_back(&(*dataPoints[sensorId])[retryDataPointIndexDown]);
                numPointsSelected++;
                pointPicked = true;
                break;
              }
            }
          }

          if (!pointPicked) {
            allDataUsed = true;
          }
        }
      }
    }
  }

  return numPointsSelected;
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
  for (auto [sensorId, sensorSettings] : sensors->sensorMap) {
    pointsPerSensor[sensorId] = 0;
  }

  // Make an unordered map of ints to track which iteration the next data point should be added for each sensor
  std::unordered_map<sensor_id_t, unsigned int> nextIterationPerSensor;
  for (auto [sensorId, sensorSettings] : sensors->sensorMap) {
    nextIterationPerSensor[sensorId] = 0;
  }

  // Loop while allocating points to each sensor until every possible bit has been used
  unsigned int usedSpace = 0;
  unsigned int iteration = 0;
  unsigned int maxIterations = -1; // This is an intentional overflow to the largest possible unsigned int value
  while (iteration < maxIterations) {
    for (auto [sensorId, sensorSettings] : sensors->sensorMap) {
      if (iteration >= nextIterationPerSensor[sensorId]) {
        if ((usedSpace + sensorSettings->numBitsPerDataPoint) < PACKET_SIZE_BITS) {
          usedSpace += sensorSettings->numBitsPerDataPoint;
          pointsPerSensor[sensorId] += 1;
          nextIterationPerSensor[sensorId] += sensorRelativeSpacing[sensorId];
        } else {
          maxIterations = iteration + sensorPriorityLCM;
        }
      }
    }

    iteration++;
  }

  // Create a temporary unordered map of vectors of all data points that are chosen for the next packet
  std::unordered_map<sensor_id_t, std::vector<DataPoint*>*> tempDataPointList;
  
  // Initialize the unordered map's vectors
  for (auto [sensorId, sensorSettings] : sensors->sensorMap) {
    tempDataPointList[sensorId] = new std::vector<DataPoint*>;
  }

  // For every sensor
  for (auto [sensorId, sensorSettings] : sensors->sensorMap) {
    unsigned int sensorDataSize = dataPoints[sensorId]->size();

    // Determine how much data for this sensor will be old or new data points
    unsigned int numOldData = 0;
    // Only if old data exists let any points be allocated to old data
    if (nextUnusedDataPointIndex[sensorId] >= 0) {
      numOldData = pointsPerSensor[sensorId] * (1 - NEW_DATA_RATIO); // Calculate old data count before new data count to have the integer multiplication round down on old data, giving a round up on new data
    }
    unsigned int numNewData = pointsPerSensor[sensorId] - numOldData;

    // Pick new data points

    unsigned int numSelected = selectDataPointsGradient(sensorId, numNewData, tempDataPointList[sensorId], nextUnusedDataPointIndex[sensorId], sensorDataSize - 1);

    if (numSelected != numNewData) {
      numOldData = pointsPerSensor[sensorId] - numSelected;
    }

    // Pick old data points

    selectDataPointsGradient(sensorId, numOldData, tempDataPointList[sensorId], 0, nextUnusedDataPointIndex[sensorId]);
  }

  // Compile the temporary vectors into a single vector

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

  // Unmark all data points that were used to build this packet
  unsigned int dataPointListSize = dataPointList->size();
  for (unsigned int dataPointListIndex = 0; dataPointListIndex < dataPointListSize; dataPointListIndex++) {
    (*dataPointList)[dataPointListIndex]->used = false;
  }

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
    
    // Loop through the vector of data points for the sensor that this data point is associated with to find the index of that data point
    unsigned int sensorDataSize = dataPoints[targetDataPoint->sensor_id]->size();
    for (unsigned int sensorDataIndex; sensorDataIndex < sensorDataSize; sensorDataIndex++) {
      // If the pointers match, then the object has been found
      if ((void*) &((*dataPoints[targetDataPoint->sensor_id])[sensorDataIndex]) == (void*) targetDataPoint) {
        // If the index of the removed point was greater than the last used data point from this sensor
        if (sensorDataIndex > nextUnusedDataPointIndex[targetDataPoint->sensor_id]) {
          // Update the last used data point index for this sensor
          nextUnusedDataPointIndex[targetDataPoint->sensor_id] = sensorDataIndex;
        }

        // If the number of times that this data point has been used exceeds the configured limit
        if (targetDataPoint->numIncludes > POINT_INCLUDE_LIMIT) {
          // Use the index that was counted while searching to remove the item from the vector of data points for that sensor
          std::vector<DataPoint>::iterator sensorDataIterator = dataPoints[targetDataPoint->sensor_id]->begin();
          std::advance(sensorDataIterator, sensorDataIndex);
          dataPoints[targetDataPoint->sensor_id]->erase(sensorDataIterator);

          // Decrement the last used data point index for this sensor to account for the deleted item
          nextUnusedDataPointIndex[targetDataPoint->sensor_id] -= 1;
        }
        
        // Exit the inner loop
        break;
      }
    }
  }
}