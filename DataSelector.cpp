#include "DataSelector.h"

#include <iostream>

DataSelector::DataSelector()
{
  sensorPriorityLCM = 1;
  nextDataPointID = 0;
}

DataSelector::DataSelector(SensorMap *sensors)
{
  // Allocate memory and set initial variable values
  this->sensors = sensors;
  sensorPriorityLCM = 1;
  nextDataPointID = 0;

  for (auto [sensorId, sensorSettings] : sensors->sensorMap)
  {
    dataPoints[sensorId] = new std::vector<DataPoint>;
    nextUnusedDataPointIndex[sensorId] = 0;
    lastDataPointReadIndex[sensorId] = 0;
    sensorPriorityLCM = std::lcm(sensorPriorityLCM, sensorSettings->priority);
  }

  for (auto [sensorId, sensorSettings] : sensors->sensorMap)
  {
    sensorRelativeSpacing[sensorId] = sensorPriorityLCM / sensorSettings->priority;
  }
}

DataSelector::~DataSelector()
{
  // Deallocate allocated memory
  for (auto [sensorId, sensorSettings] : sensors->sensorMap)
  {
    delete dataPoints[sensorId];
  }

  if (currentData != nullptr)
  {
    delete currentData;
  }
}

// Used to update the values stored in dataPoints
void DataSelector::updateDataPoints()
{
  // For each sensor
  for (auto [sensorId, sensorSettings] : sensors->sensorMap)
  {
    // Get the path to the file that stores that sensor's data
    std::ifstream sensorFile;
    std::string path = SENSOR_DATA_PATH;
    path += std::to_string(sensorId);

    // Determine the minimum number of bytes needed to read one data point of this sensor from the file
    unsigned int bufferSize = (sensorSettings->numBitsPerDataPoint + 7) / 8; // This rounds up to next byte threshold
    char *buffer = new char[bufferSize + 1];

#ifdef DATA_SEL_P
    std::cout << "UPDATING DATA POINTS" << std::endl;
    std::cout << "--------------------" << std::endl;
    std::cout << "sensor: " << sensorId << std::endl;
    std::cout << "numBitsPerDataPoint: " << sensorSettings->numBitsPerDataPoint << std::endl;
    std::cout << "bufferSize: " << bufferSize << std::endl;
#endif

    // This is where I would put the semaphore wait if we were using per-sensor file semaphores

    // Open the file for that sensor's data
    sensorFile.open(path, std::ios_base::binary);
    if (!sensorFile.fail())
    {
      std::string row;

      // If there were previously tracked data points
      if (lastDataPointReadIndex[sensorId] != 0)
      {
#ifdef DATA_SEL_P
        std::cout << "CATCHING UP " << sensorId << std::endl;
#endif
        // Open the file to the last known data point
        sensorFile.seekg(lastDataPointReadIndex[sensorId], std::ios_base::beg);
        // And discard the last data point so the read pointer is at the start of the next data point
        memset(buffer, '\0', bufferSize + 1);
        sensorFile.read(buffer, bufferSize);
      }

      // While no error in the file has occurred
      while (sensorFile)
      {
        // Record the current file index
        unsigned int fileIndex = sensorFile.tellg();

        // Read the data so the read pointer advances
        memset(buffer, '\0', bufferSize + 1);
        sensorFile.read(buffer, bufferSize);

        // If the file is still good after the read, then this point was valid
        // If there was an error, then this was the end of the file and there was no point after the last known point
        if (sensorFile) {
          // Initialize a new class for the new data point and set the initial values
          DataPoint newDataPoint;
          newDataPoint.sensor_id = sensorId;
          newDataPoint.numIncludes = 0;
          newDataPoint.fileIndex = fileIndex;
          newDataPoint.used = false;
          newDataPoint.gradient = 1;
          newDataPoint.ID = nextDataPointID;

          // Update internal variables
          nextDataPointID++;
          lastDataPointReadIndex[sensorId] = fileIndex;

          // Add the new DataPoint to the vector of data points for that sensor
          dataPoints[sensorId]->push_back(newDataPoint);

#ifdef DATA_SEL_P
          std::cout << "MORE DATA SENSOR " << sensorId << " INDEX " << newDataPoint.fileIndex std::endl;
#endif
        }
      }
    }

    // Remove the temporary buffer used for reading this sensor's data
    delete[] buffer;

    // Close the file
    sensorFile.close();

    // This is where I would put the semaphore signal if we were using per-sensor file semaphores
  }
}

unsigned int DataSelector::selectDataPointsGradient(unsigned int sensorId, unsigned int numData, std::vector<DataPoint *> *tempDataPointList, unsigned int startInclusive, unsigned int endExclusive, double offset)
{
#ifdef DATA_SEL_P
  printf("SELECTING DATA POINTS GRADIENT\n");
  printf("------------------------------\n");
  printf("sensorId: %d\n", sensorId);
  printf("endExclusive - startInclusive: %u\n", (endExclusive - startInclusive));
  printf("numData: %u\n", numData);
  printf("startInclusive: %u\n", startInclusive);
  printf("endExclusive: %u\n", endExclusive);
  printf("offset: %lf\n", offset);
#endif

  // If the range of available data is less than the amount of data that is trying to be selected
  if ((endExclusive - startInclusive) < numData)
  {
    // Set the number of points to select equal to the number of available points
    numData = endExclusive - startInclusive;
  }

  // If there are no points to select, simply return
  if (numData == 0)
  {
    return 0;
  }

  // First, find the sum of the gradient for all points in the range of data to select from
  unsigned int totalGradient = 0;

  for (unsigned int dataPointIndex = startInclusive; dataPointIndex < endExclusive; dataPointIndex++)
  {
    totalGradient += (*dataPoints[sensorId])[dataPointIndex].gradient;
  }

  // Divide that number by numData to determine the gradient spacing for each point to be picked
  // so that there is an even spacing of gradient between each picked point
  double gradientSpacing = (double) totalGradient / (double) numData;

  unsigned int numPointsSelected = 0;

  bool allDataSelected = false;

#ifdef DATA_SEL_P
  printf("total gradient: %d\n", totalGradient);
  printf("gradient spacing: %f\n", gradientSpacing);
#endif

  // For each point that we are trying to slect
  for (unsigned int index = 0; index < numData; index++)
  {
    // If we have run out of data, stop looking for points to pick
    if (allDataSelected) {
      break;
    }

    bool pointPicked = false;

    // Determine how far into the total gradient we want to go before stopping and picking a point
    unsigned int targetGradient = (unsigned int) ((gradientSpacing * offset) + (gradientSpacing * index));

#ifdef DATA_SEL_P
    printf("target gradient: %d\n", targetGradient);
#endif

    // For every data point in the available range of points to select from, starting at the beginning and working to the end
    for (unsigned int dataPointIndex = startInclusive; dataPointIndex < endExclusive; dataPointIndex++)
    {
      // Remove the value of that point's gradient
      targetGradient -= (*dataPoints[sensorId])[dataPointIndex].gradient;

      // If the targetGradient has gone below 0 (overflowed in this case due to unsigned int) then we have gone the desired distance through the avialable number of points
      if (targetGradient > totalGradient) // This means that it went below 0 and overflowed
      {
        // If the point we stopped at hasn't been picked yet
        if ((*dataPoints[sensorId])[dataPointIndex].used == false)
        {
#ifdef DATA_SEL_P
          printf("sensor %d added point %d fileIndex %d\n", sensorId, dataPointIndex, (*dataPoints[sensorId])[dataPointIndex].fileIndex);
#endif
          // Pick this point and mark it so it can't be picked twice
          (*dataPoints[sensorId])[dataPointIndex].used = true;
          tempDataPointList->push_back(&(*dataPoints[sensorId])[dataPointIndex]);
          numPointsSelected++;
          pointPicked = true;
        }
        // If the point we stopped at was already picked, we still want to pick a point in this area
        else
        {
          // For each nearby point starting at the immediate neighbors and moving outward
          for (unsigned int retryIndex = 1; retryIndex < numData - 1; retryIndex++)
          {
            unsigned int retryDataPointIndexUp = dataPointIndex + retryIndex;
            unsigned int retryDataPointIndexDown = dataPointIndex - retryIndex;

            // If the next closest neighbor in the direction of newer data wasn't used
            if (retryDataPointIndexUp < endExclusive)
            {
              if ((*dataPoints[sensorId])[retryDataPointIndexUp].used == false)
              {
#ifdef DATA_SEL_P
                printf("sensor %d added point %d retry up fileIndex %d\n", sensorId, retryDataPointIndexUp, (*dataPoints[sensorId])[retryDataPointIndexUp].fileIndex);
#endif
                // Pick it and mark it so it can't be picked twice
                (*dataPoints[sensorId])[retryDataPointIndexUp].used = true;
                tempDataPointList->push_back(&(*dataPoints[sensorId])[retryDataPointIndexUp]);
                numPointsSelected++;
                pointPicked = true;

                // Break so we stop searching for neighbors
                break;
              }
            }

            // If the next closest neighbor in the direction of older data wasn't used
            if (retryDataPointIndexDown >= startInclusive)
            {
              // Pick it and mark it so it can't be picked twice
              if ((*dataPoints[sensorId])[retryDataPointIndexDown].used == false)
              {
#ifdef DATA_SEL_P
                printf("sensor %d added point %d retry down fileIndex %d\n", sensorId, retryDataPointIndexDown, (*dataPoints[sensorId])[retryDataPointIndexDown].fileIndex);
#endif
                (*dataPoints[sensorId])[retryDataPointIndexDown].used = true;
                tempDataPointList->push_back(&(*dataPoints[sensorId])[retryDataPointIndexDown]);
                numPointsSelected++;
                pointPicked = true;

                // Break so we stop searching for neighbors
                break;
              }
            }
          }
        }
      }

      // If we have picked a point, either the point we desired or a nearby neighbor, stop searching through the available points
      if (pointPicked) {
        break;
      }
    }

    // If we searched through every point and every neighbor and could not find a point to pick
    if (!pointPicked)
    {
      // Indicate that we have run out of data
      allDataSelected = true;
    }
  }

  return numPointsSelected;
}

unsigned int DataSelector::selectDataPointsIndex(unsigned int sensorId, unsigned int numData, std::vector<DataPoint *> *tempDataPointList, unsigned int startInclusive, unsigned int endExclusive, double offset)
{
#ifdef DATA_SEL_P
  printf("SELECTING DATA POINTS INDEX\n");
  printf("------------------------------\n");
  printf("sensorId: %d\n", sensorId);
  printf("endExclusive - startInclusive: %u\n", (endExclusive - startInclusive));
  printf("numData: %u\n", numData);
  printf("startInclusive: %u\n", startInclusive);
  printf("endExclusive: %u\n", endExclusive);
  printf("offset: %lf\n", offset);
#endif
  // If the range of available data is less than the amount of data that is trying to be selected
  if ((endExclusive - startInclusive) <= numData)
  {
    // Set the number of points to select equal to the number of available points
    numData = endExclusive - startInclusive;

    // And remove any offset that was set so that all points are guaranteed to be picked
    offset = 0;
  }

  // If there are no points to select, simply return
  if (numData == 0)
  {
    return 0;
  }

  // Calculate the index increment to allow that number of data points to be evenly spaced across time (from the last used data point to the last recorded data point)
  double dataSpacing = ((endExclusive - 1) - startInclusive) / numData;
  dataSpacing = std::max(1.0, dataSpacing);

  unsigned int numPointsSelected = 0;

  // Iterate through the data points using the calculated increment and add them to the temporary vector
  double dataPointIndex = startInclusive + (dataSpacing * offset);
  for (unsigned int i = 0; i < numData; i++)
  {
#ifdef DATA_SEL_P
    printf("sensor %d added point %d fileIndex %d\n", sensorId, (int)dataPointIndex, (*dataPoints[sensorId])[(int)dataPointIndex].fileIndex);
#endif
    tempDataPointList->push_back(&(*dataPoints[sensorId])[(int)dataPointIndex]);
    numPointsSelected++;

    dataPointIndex += dataSpacing;
  }

  return numPointsSelected;
}

// Called by the packet building thread to select data points to include in the nexxt packet
std::vector<DataPoint *> *DataSelector::selectData()
{
#ifdef DATA_SEL_P
  printf("SELECTING DATA\n");
  printf("--------------\n");
#endif

  // First, update the vectors of data points that are tracked in memory and used for data selection
  updateDataPoints();

  // Make an unordered map of ints to track how many data points go to each sensor
  std::unordered_map<int, unsigned int> pointsPerSensor;
  for (auto [sensorId, sensorSettings] : sensors->sensorMap)
  {
    pointsPerSensor[sensorId] = 0;
#ifdef DATA_SEL_P
    printf("data points sensor %d: %u\n", sensorId, dataPoints[sensorId]->size());
#endif
  }

  // Make an unordered map of ints to track which iteration the next data point should be added for each sensor
  std::unordered_map<int, unsigned int> nextIterationPerSensor;
  for (auto [sensorId, sensorSettings] : sensors->sensorMap)
  {
    nextIterationPerSensor[sensorId] = 0;
  }

#ifdef DATA_SEL_P
  printf("ALLOCATING POINTS\n");
  printf("-----------------\n");
  printf("LCM: %d\n", sensorPriorityLCM);
#endif
  // Loop while allocating points to each sensor until every possible bit has been used
  bool moreData = false;
  unsigned int usedSpace = 0;
  unsigned int iteration = 0;

  // Start with a maximum number of iterations equal to the LCM (Least Common Multiple) of all of the sensor's priorities
  // Doing this many iterations will allow each data point to have been allowed the chance to be allocated a number of data
  // points equal to each sensor's priority if each sensor tries to add a point every LCM / priority iterations
  unsigned int maxIterations = sensorPriorityLCM;
  while (iteration <= maxIterations)
  {
    // For each sensor
    for (auto [sensorId, sensorSettings] : sensors->sensorMap)
    {
      // If the number of iterations have passed to try to allocate another point to this sensor (LCM / priority iterations)
      if (iteration >= nextIterationPerSensor[sensorId])
      {
#ifdef DATA_SEL_P
          printf("attempt for sensor: %d iteration: \n", sensorId, iteration);
          printf("num picked for this sensor: %d\n", pointsPerSensor[sensorId]);
          printf("usedSpace: %d\n", usedSpace);
          printf("total space: %d\n", PACKET_SIZE_BITS);
#endif

        // If there is enough space to add a point to this sensor
        if ((usedSpace + sensorSettings->numBitsPerDataPoint) <= (PACKET_SIZE_BITS))
        {
          // If there are more data points for this sensor that have not yet been allocated to this packet
          if (dataPoints[sensorId]->size() > pointsPerSensor[sensorId]) {
            // Allocate another point to this sensor and mark the next iteration during which it should attempt to allocate another point to this sensor
            usedSpace += sensorSettings->numBitsPerDataPoint;
            pointsPerSensor[sensorId] += 1;
            nextIterationPerSensor[sensorId] += sensorRelativeSpacing[sensorId];
#ifdef DATA_SEL_P
            printf("added point for sensor %d on iteration %d\n", sensorId, iteration);
#endif

            // Indicate that a point was added
            moreData = true;
          }
        }
      }
    }

    // If we have reached the current limit on the number of iterations but during the last LCM iterations we added at least one point
    if (iteration == maxIterations && moreData)
    {
      // It is possible that there are more points we could add to the sensor
      // Reset the indicator for more data
      moreData = false;
      // Add another LCM iterations to allow one more full cycle of trying to allocate points to sensors according to their priority
      maxIterations += sensorPriorityLCM;
#ifdef DATA_SEL_P
      printf("more data, new end %d\n", maxIterations);
#endif
    }

    iteration++;
  }
#ifdef DATA_SEL_P
  printf("ENDED ALLOCATION iteration %d\n", iteration);
#endif

  // Create a temporary unordered map of vectors of all data points that are chosen for the next packet
  std::unordered_map<int, std::vector<DataPoint *> *> tempDataPointList;

  // Initialize the unordered map's vectors
  for (auto [sensorId, sensorSettings] : sensors->sensorMap)
  {
    tempDataPointList[sensorId] = new std::vector<DataPoint *>;
  }

  // For every sensor
  for (auto [sensorId, sensorSettings] : sensors->sensorMap)
  {
#ifdef DATA_SEL_P
    printf("SELECTING %d POINTS FOR SENSOR %d\n", pointsPerSensor[sensorId], sensorId);
#endif

    unsigned int sensorDataSize = dataPoints[sensorId]->size();

    // Determine how much data for this sensor will be old or new data points
    unsigned int numOldData = 0;
    // Only if old data exists let any points be allocated to old data
    if (nextUnusedDataPointIndex[sensorId] > 0)
    {
      numOldData = pointsPerSensor[sensorId] * (1 - NEW_DATA_RATIO); // Calculate old data count before new data count to have the integer multiplication round down on old data, giving a round up on new data
    }
    unsigned int numNewData = pointsPerSensor[sensorId] - numOldData;

#ifdef DATA_SEL_P
    printf("old points: %d\n", numOldData);
    printf("new points: %d\n", numNewData);
#endif

    // Track the number of data points selected
    unsigned int numSelected = 0;

    // Pick new data points based on configuration parameter
    if (NEW_DATA_GRADIENT_SELECT)
    {
      numSelected = selectDataPointsGradient(sensorId, numNewData, tempDataPointList[sensorId], nextUnusedDataPointIndex[sensorId], sensorDataSize, 1.0);
    }
    else
    {
      numSelected = selectDataPointsIndex(sensorId, numNewData, tempDataPointList[sensorId], nextUnusedDataPointIndex[sensorId], sensorDataSize, 1.0);
    }

#ifdef DATA_SEL_P
    printf("actual new points: %d\n", numSelected);
#endif

    // If the number of new points picked doesn't match the expected number
    if (numSelected != numNewData)
    {
      // Allocate any remaining points to old data to fill as much space as possible
      numOldData = pointsPerSensor[sensorId] - numSelected;
    }

#ifdef DATA_SEL_P
    printf("old points: %d\n", numOldData);
#endif

    // Pick old data points based on configuration parameter
    if (OLD_DATA_GRADIENT_SELECT)
    {
      numSelected = selectDataPointsGradient(sensorId, numOldData, tempDataPointList[sensorId], 0, nextUnusedDataPointIndex[sensorId], 0.5);
    }
    else
    {
      numSelected = selectDataPointsIndex(sensorId, numOldData, tempDataPointList[sensorId], 0, nextUnusedDataPointIndex[sensorId], 0.5);
    }

#ifdef DATA_SEL_P
    printf("actual old points: %d\n", numSelected);
#endif

  } // end for

  // Compile the temporary vectors into a single vector

  // Make a temporary unordered map of integers to track the next unused data point from each sensor's temporary vector
  std::unordered_map<int, unsigned int> nextPointPerSensor;
  for (auto [sensorId, sensorSettings] : sensors->sensorMap)
  {
    // Initialize the value to 0
    nextPointPerSensor[sensorId] = 0;
  }

  // Create the vector of sensor data that is returned
  std::vector<DataPoint *> *dataPointList = new std::vector<DataPoint *>;

  // Until all data in the temporary vectors of sensor data have been added to the returned vector
  moreData = true;
  while (moreData)
  {
    moreData = false;

    // The following code is basically a round-robin method of adding different sensors' data points
    // to the returned vector, except that the number of points from each sensor added to the final vector
    // in each iteration is weighted by the sensor's priority

    // For each sensor
    for (auto [sensorId, sensorSettings] : sensors->sensorMap)
    {
      // Until a number of data points equal to the sensor's priority have been added to the returned vector
      // or until the sensor has no more data points left to add
      unsigned int pointNum = 0;
      unsigned int tempDataPointListSize = tempDataPointList[sensorId]->size();
      while (pointNum < sensorSettings->priority && nextPointPerSensor[sensorId] < tempDataPointListSize)
      {
        // Add the next data point to the returned vector
        dataPointList->push_back((*tempDataPointList[sensorId])[nextPointPerSensor[sensorId]]);
        // Increment the counts
        nextPointPerSensor[sensorId] += 1;
        pointNum++;
      }

      // If there are more data points to add from this sensor
      if (nextPointPerSensor[sensorId] < tempDataPointListSize)
      {
        // Indicate that more data must be added (continuing the loop)
        moreData = true;
      }
    }
  }

  // Free allocated memory for temporary data point vectors for each sensor
  for (auto [sensorId, sensorSettings] : sensors->sensorMap)
  {
    delete tempDataPointList[sensorId];
  }
  
  // Release memory for old list of data points
  if (currentData != nullptr) {
    delete currentData;
  }

  // Track the currently generated list of data points
  currentData = dataPointList;

  // Unmark all data points that were used to build this packet
  unsigned int dataPointListSize = dataPointList->size();
  for (unsigned int dataPointListIndex = 0; dataPointListIndex < dataPointListSize; dataPointListIndex++)
  {
    (*dataPointList)[dataPointListIndex]->used = false;
  }

#ifdef DATA_SEL_P
  std::cout << "generated list size: " << dataPointList->size() << std::endl;
#endif

  // Return the list
  return dataPointList;
}

// Called by the packet building thread to indicate that the previous list of data points was used
void DataSelector::markUsed()
{
  // Only mark data if there is any data to mark
  if (currentData == nullptr) {
    return;
  }

  // For each data point in the previous list of data points
  unsigned int currentDataSize = currentData->size();
  for (unsigned int currentDataIndex = 0; currentDataIndex < currentDataSize; currentDataIndex++)
  {
    DataPoint *targetDataPoint = (*currentData)[currentDataIndex];

    // Loop through the vector of data points for the sensor that this data point is associated with to find the index of that data point
    unsigned int sensorDataSize = dataPoints[targetDataPoint->sensor_id]->size();
    for (unsigned int sensorDataIndex = 0; sensorDataIndex < sensorDataSize; sensorDataIndex++)
    {
      // If the IDs match, then the object has been found
      if ((*dataPoints[targetDataPoint->sensor_id])[sensorDataIndex].ID == targetDataPoint->ID)
      {
        // Increment the number of times that this data point has been used
        targetDataPoint->numIncludes++;

#ifdef DATA_SEL_P
        std::cout << "marked used sensor " << targetDataPoint->sensor_id << " index " << sensorDataIndex << " used " << targetDataPoint->numIncludes << " times" << std::endl;
#endif

        // If the index of the removed point was greater than the next unused data point from this sensor
        if (sensorDataIndex >= nextUnusedDataPointIndex[targetDataPoint->sensor_id])
        {
          // If the number of times that this data point has been used exceeds the configured limit
          if (targetDataPoint->numIncludes > POINT_INCLUDE_LIMIT)
          {
            // Use the index that was counted while searching to remove the item from the vector of data points for that sensor
            std::vector<DataPoint>::iterator sensorDataIterator = dataPoints[targetDataPoint->sensor_id]->begin();
            std::advance(sensorDataIterator, sensorDataIndex);
            dataPoints[targetDataPoint->sensor_id]->erase(sensorDataIterator);

            // Update the next unused data point index for this sensor to be where this point used to be
            nextUnusedDataPointIndex[targetDataPoint->sensor_id] = sensorDataIndex;

#ifdef DATA_SEL_P
            std::cout << "removed sensor " << targetDataPoint->sensor_id << " index " << sensorDataIndex << std::endl;
#endif
          } else {
            // Update the next unused data point index for this sensor to be immediately after this point
            nextUnusedDataPointIndex[targetDataPoint->sensor_id] = sensorDataIndex + 1;
          }
        // If the index of the removed point was less than the next unused data point from this sensor
        } else {
          // If the number of times that this data point has been used exceeds the configured limit
          if (targetDataPoint->numIncludes > POINT_INCLUDE_LIMIT)
          {
            // Use the index that was counted while searching to remove the item from the vector of data points for that sensor
            std::vector<DataPoint>::iterator sensorDataIterator = dataPoints[targetDataPoint->sensor_id]->begin();
            std::advance(sensorDataIterator, sensorDataIndex);
            dataPoints[targetDataPoint->sensor_id]->erase(sensorDataIterator);

            // Subtract the next unused data point index to account for this point being removed
            nextUnusedDataPointIndex[targetDataPoint->sensor_id] -= 1;

#ifdef DATA_SEL_P
            std::cout << "removed sensor " << targetDataPoint->sensor_id << " index " << sensorDataIndex << std::endl;
#endif
          }
        }

        // Exit the inner loop
        break;
      }
    }
  }
}