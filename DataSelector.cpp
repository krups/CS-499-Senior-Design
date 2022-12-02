#include "DataSelector.h"

#include <iostream>

DataSelector::DataSelector()
{
  sensorPriorityLCM = 1;
}

DataSelector::DataSelector(SensorMap *sensors)
{
  // Allocate memory and set initial variable values
  this->sensors = sensors;
  sensorPriorityLCM = 1;

  for (auto [sensorId, sensorSettings] : sensors->sensorMap)
  {
    dataPoints[sensorId] = new std::vector<DataPoint>;
    nextUnusedDataPointIndex[sensorId] = 0;
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

  if (previousData != nullptr)
  {
    delete previousData;
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

    unsigned int bufferSize = (sensorSettings->numBitsPerDataPoint + 7) / 8; // This rounds up to next byte threshold
    char *buffer = new char[bufferSize + 1];

#ifdef DATA_SEL_P
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
      if (dataPoints[sensorId]->size() != 0)
      {
#ifdef DATA_SEL_P
        std::cout << "CATCHING UP " << sensorId << std::endl;
#endif
        // Open the file to the last known data point
        sensorFile.seekg((*dataPoints[sensorId])[dataPoints[sensorId]->size() - 1].fileIndex, std::ios_base::beg);
        // And discard the last data point so the read pointer is at the start of the next data point
        memset(buffer, '\0', bufferSize + 1);
        sensorFile.read(buffer, bufferSize);
      }

      // Until the end of the file is reached
      while (sensorFile)
      {
#ifdef DATA_SEL_P
        std::cout << "MORE DATA " << sensorId << std::endl;
#endif
        // Create a new DataPoint class and initialize its values
        DataPoint newDataPoint;
        newDataPoint.sensor_id = sensorId;
        newDataPoint.numIncludes = 0;
        newDataPoint.fileIndex = sensorFile.tellg();
        newDataPoint.used = false;

#ifdef DATA_SEL_P
        std::cout << "FILE INDEX: " << newDataPoint.fileIndex << std::endl;
#endif

        // Read the data so the read pointer advances
        memset(buffer, '\0', bufferSize + 1);
        sensorFile.read(buffer, bufferSize);

        newDataPoint.gradient = 1;

        // Add the new DataPoint to the vector of data points for that sensor
        dataPoints[sensorId]->push_back(newDataPoint);
      }
    }

    delete[] buffer;

    // Close the file
    sensorFile.close();

    // This is where I would put the semaphore signal if we were using per-sensor file semaphores
  }
}

unsigned int DataSelector::selectDataPointsGradient(unsigned int sensorId, unsigned int numData, std::vector<DataPoint *> *tempDataPointList, unsigned int startInclusive, unsigned int endExclusive, double offset)
{
// Make sure that it doesn't try to select more data points than exist
#ifdef DATA_SEL_P
  printf("endExclusive - startInclusive: %u\n", (endExclusive - startInclusive));
  printf("numData: %u\n", numData);
  printf("offset: %lf\n", offset);
  printf("sensorId: %d\n", sensorId);
#endif
  if ((endExclusive - startInclusive) < numData)
  {
    numData = endExclusive - startInclusive;
  }

  if (numData == 0)
  {
    return 0;
  }

  int totalGradient = 0;

  for (unsigned int dataPointIndex = startInclusive; dataPointIndex < endExclusive; dataPointIndex++)
  {
    totalGradient += (*dataPoints[sensorId])[dataPointIndex].gradient;
  }

  double gradientSpacing = totalGradient / numData;

  unsigned int numPointsSelected = 0;

  bool pointPicked = false;

  for (unsigned int index = 0; index < numData; index++)
  {
    int targetGradient = (gradientSpacing * offset) + (gradientSpacing * index);

    for (unsigned int dataPointIndex = startInclusive; dataPointIndex < endExclusive; dataPointIndex++)
    {
      targetGradient -= (*dataPoints[sensorId])[dataPointIndex].gradient;

      if (targetGradient <= 0)
      {
        if ((*dataPoints[sensorId])[dataPointIndex].used == false)
        {
          (*dataPoints[sensorId])[dataPointIndex].used = true;
          tempDataPointList->push_back(&(*dataPoints[sensorId])[dataPointIndex]);
          numPointsSelected++;
          pointPicked = true;
        }
        else
        {
          for (unsigned int retryIndex = 1; retryIndex < numData - 1; retryIndex++)
          {
            unsigned int retryDataPointIndexUp = dataPointIndex + retryIndex;
            unsigned int retryDataPointIndexDown = dataPointIndex - retryIndex;

            if (retryDataPointIndexUp < endExclusive)
            {
              if ((*dataPoints[sensorId])[retryDataPointIndexUp].used == false)
              {
                (*dataPoints[sensorId])[retryDataPointIndexUp].used = true;
                tempDataPointList->push_back(&(*dataPoints[sensorId])[retryDataPointIndexUp]);
                numPointsSelected++;
                pointPicked = true;
                break;
              }
            }

            if (retryDataPointIndexDown >= startInclusive)
            {
              if ((*dataPoints[sensorId])[retryDataPointIndexDown].used == false)
              {
                (*dataPoints[sensorId])[retryDataPointIndexDown].used = true;
                tempDataPointList->push_back(&(*dataPoints[sensorId])[retryDataPointIndexDown]);
                numPointsSelected++;
                pointPicked = true;
                break;
              }
            }
          }
        }
      }

      if (pointPicked == false)
      {
        break;
      }
    }

    if (pointPicked == false)
    {
      break;
    }
  }

  return numPointsSelected;
}

unsigned int DataSelector::selectDataPointsIndex(unsigned int sensorId, unsigned int numData, std::vector<DataPoint *> *tempDataPointList, unsigned int startInclusive, unsigned int endExclusive, double offset)
{
#ifdef DATA_SEL_P
  printf("endExclusive - startInclusive: %u\n", (endExclusive - startInclusive));
  printf("numData: %u\n", numData);
  printf("offset: %lf\n", offset);
  printf("sensorId: %d\n", sensorId);
#endif
  // Make sure that it doesn't try to select more data points than exist
  if ((endExclusive - startInclusive) <= numData)
  {
    numData = endExclusive - startInclusive;
    offset = 0;
  }

  if (numData == 0)
  {
    return 0;
  }

  // Calculate the index increment to allow that number of data points to be evenly spaced across time (from the last used data point to the last recorded data point)
  double dataSpacing = ((endExclusive - 1) - startInclusive) / numData;
  dataSpacing = std::max(1.0, dataSpacing);

  unsigned int numPointsSelected = 0;

  // Iterate through the new data points using the calculated increment and add them to the temporary vector
  double dataPointIndex = startInclusive + (dataSpacing * offset);
  for (unsigned int i = 0; i < numData; i++)
  {
    tempDataPointList->push_back(&(*dataPoints[sensorId])[(int)dataPointIndex]);
    numPointsSelected++;

    dataPointIndex += dataSpacing;
  }

  return numPointsSelected;
}

// Called by the packet building thread to select data points to include in the nexxt packet
std::vector<DataPoint *> *DataSelector::selectData()
{
  // First, update the vectors of data points that are tracked in memory and used for data selection
  updateDataPoints();

  // Track the previously generated vector of data points
  if (previousData != nullptr)
  {
    delete previousData;
  }
  previousData = currentData;

  // Make an unordered map of ints to track how many data points go to each sensor
  std::unordered_map<int, unsigned int> pointsPerSensor;
  for (auto [sensorId, sensorSettings] : sensors->sensorMap)
  {
    pointsPerSensor[sensorId] = 0;
  }

  // Make an unordered map of ints to track which iteration the next data point should be added for each sensor
  std::unordered_map<int, unsigned int> nextIterationPerSensor;
  for (auto [sensorId, sensorSettings] : sensors->sensorMap)
  {
    nextIterationPerSensor[sensorId] = 0;
#ifdef DATA_SEL_P
    printf("dataPointSize: %u\n", dataPoints[sensorId]->size());
#endif
  }

#ifdef DATA_SEL_P
  printf("LCM: %d\n", sensorPriorityLCM);
#endif
  // Loop while allocating points to each sensor until every possible bit has been used
  bool moreData = false;
  unsigned int usedSpace = 0;
  unsigned int iteration = 0;
  unsigned int maxIterations = sensorPriorityLCM;
  while (iteration <= maxIterations)
  {
    for (auto [sensorId, sensorSettings] : sensors->sensorMap)
    {
      if (iteration >= nextIterationPerSensor[sensorId])
      {
#ifdef DATA_SEL_P
          printf("1 num points for sensor %d: %d\n", sensorId, dataPoints[sensorId]->size());
          printf("2 num picked for this sensor: %d\n", pointsPerSensor[sensorId]);
          printf("3 usedSpace: %d\n", usedSpace);
          printf("4 total space: %d\n", PACKET_SIZE_BITS);
          printf("----\n");
#endif
        if ((usedSpace + sensorSettings->numBitsPerDataPoint) <= (PACKET_SIZE_BITS))
        {
          if (dataPoints[sensorId]->size() > pointsPerSensor[sensorId]) {
            usedSpace += sensorSettings->numBitsPerDataPoint;
            pointsPerSensor[sensorId] += 1;
            nextIterationPerSensor[sensorId] += sensorRelativeSpacing[sensorId];
#ifdef DATA_SEL_P
            printf("added point for sensor %d on iteration %d\n", sensorId, iteration);
#endif
            moreData = true;
          }
        }
      }
    }

    if (iteration == maxIterations && moreData)
    {
      moreData = false;
      maxIterations += sensorPriorityLCM;
#ifdef DATA_SEL_P
      printf("more data, new end %d\n", maxIterations);
#endif
    }

    iteration++;
  }
#ifdef DATA_SEL_P
  printf("ended iteration %d\n", iteration);
#endif

  // Create a temporary unordered map of vectors of all data points that are chosen for the next packet
  std::unordered_map<int, std::vector<DataPoint *> *> tempDataPointList;

  // Initialize the unordered map's vectors
  for (auto [sensorId, sensorSettings] : sensors->sensorMap)
  {
    tempDataPointList[sensorId] = new std::vector<DataPoint *>;
  }
#ifdef DATA_SEL_P
  printf("-------------------------------------------------\n");
#endif
  // For every sensor
  for (auto [sensorId, sensorSettings] : sensors->sensorMap)
  {
    unsigned int sensorDataSize = dataPoints[sensorId]->size();

    // Determine how much data for this sensor will be old or new data points
    unsigned int numOldData = 0;
    // Only if old data exists let any points be allocated to old data
    if (nextUnusedDataPointIndex[sensorId] > 0)
    {
      numOldData = pointsPerSensor[sensorId] * (1 - NEW_DATA_RATIO); // Calculate old data count before new data count to have the integer multiplication round down on old data, giving a round up on new data
    }
    unsigned int numNewData = pointsPerSensor[sensorId] - numOldData;

    // Track the number of data points selected
    unsigned int numSelected = 0;
#ifdef DATA_SEL_P
    printf("--------------------------------\n");
    printf("points: %d\n", pointsPerSensor[sensorId]);
#endif
    // Pick new data points
    if (NEW_DATA_GRADIENT_SELECT)
    {
      numSelected = selectDataPointsGradient(sensorId, numNewData, tempDataPointList[sensorId], nextUnusedDataPointIndex[sensorId], sensorDataSize, 1.0);
    }
    else
    {
      numSelected = selectDataPointsIndex(sensorId, numNewData, tempDataPointList[sensorId], nextUnusedDataPointIndex[sensorId], sensorDataSize, 1.0);
    }

    // If the number of new points picked doesn't match the expected number
    if (numSelected != numNewData)
    {
      // Allocate any remaining points to old data to fill as much space as possible
      numOldData = pointsPerSensor[sensorId] - numSelected;
    }

    // Pick old data points
    if (OLD_DATA_GRADIENT_SELECT)
    {
      numSelected = selectDataPointsGradient(sensorId, numOldData, tempDataPointList[sensorId], 0, nextUnusedDataPointIndex[sensorId], 0.5);
    }
    else
    {
      numSelected = selectDataPointsIndex(sensorId, numOldData, tempDataPointList[sensorId], 0, nextUnusedDataPointIndex[sensorId], 0.5);
    }
#ifdef DATA_SEL_P
    printf("--------------------------------\n");
#endif
  } // end for
#ifdef DATA_SEL_P
  printf("-------------------------------------------------\n");
#endif

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
      if (nextPointPerSensor[sensorId] < tempDataPointList[sensorId]->size())
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
  // For each data point in the previous list of data points
  unsigned int previousDataSize = previousData->size();
  for (unsigned int previousDataIndex = 0; previousDataIndex < previousDataSize; previousDataIndex++)
  {
    DataPoint *targetDataPoint = (*previousData)[previousDataIndex];

    // Increment the number of times that this data point has been used
    targetDataPoint->numIncludes++;

    // Loop through the vector of data points for the sensor that this data point is associated with to find the index of that data point
    unsigned int sensorDataSize = dataPoints[targetDataPoint->sensor_id]->size();
    for (unsigned int sensorDataIndex = 0; sensorDataIndex < sensorDataSize; sensorDataIndex++)
    {
      // If the pointers match, then the object has been found
      if ((void *)&((*dataPoints[targetDataPoint->sensor_id])[sensorDataIndex]) == (void *)targetDataPoint)
      {
        // If the index of the removed point was greater than the next unused data point from this sensor
        if (sensorDataIndex >= nextUnusedDataPointIndex[targetDataPoint->sensor_id])
        {
          // Update the next unused data point index for this sensor
          nextUnusedDataPointIndex[targetDataPoint->sensor_id] = sensorDataIndex + 1;
        }

        // If the number of times that this data point has been used exceeds the configured limit
        if (targetDataPoint->numIncludes > POINT_INCLUDE_LIMIT)
        {
          // Use the index that was counted while searching to remove the item from the vector of data points for that sensor
          std::vector<DataPoint>::iterator sensorDataIterator = dataPoints[targetDataPoint->sensor_id]->begin() + sensorDataIndex;
          // std::advance(sensorDataIterator, sensorDataIndex);
          dataPoints[targetDataPoint->sensor_id]->erase(sensorDataIterator);

          // Decrement the next unused data point index for this sensor to account for the deleted item
          nextUnusedDataPointIndex[targetDataPoint->sensor_id] -= 1;
        }

        // Exit the inner loop
        break;
      }
    }
  }
}