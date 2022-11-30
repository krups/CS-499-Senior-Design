#include <stdio.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

#include "config.h"
#include "data.h"
#include "serial_headers.h"
#include "DataSelector.h"
#include "copyBits.h"

char packetBuffer[PACKET_SIZE];
bool dataUsed = false;

sem_t dataUsedSem;
sem_t packetSem;
sem_t sensor1Sem;

SensorMap sensors;

void saveData(Data data, char *buf)
{
#ifdef PRINT_DATA
    data.printData();
#endif
    std::ofstream sensorDataFile;
    std::string path = SENSOR_DATA_PATH;
    path += std::to_string(data.getType());
    sensorDataFile.open(path, std::ios::app | std::ios::binary);
    if (!sensorDataFile.fail())
    {
        sensorDataFile.write(buf, data.getNumBytes());
        std::cout << "Saving line: " << data << " to file" << std::endl;
#ifdef PRINT_DATA
        printf("Saved %d, %u to file!\n", data.getType(), data.getTimeStamp());
#endif
    }
    else
#ifdef PRINT_DATA
        printf("Open: %s failed!\n", path.c_str());
#endif
    sensorDataFile.close();
}

bool checkValid(Data data)
{
    std::vector<u_int16_t> points = data.getData();
    for (int i = 0; i < data.getNumVals(); i++)
    { // Add checks to correspond with validity parameters in config.h

        // Check for valid numbers
        if (points[i] == NAN)
        {
#ifdef VALIDITY_P
            printf("Error %u: Validity check failed! with '%d is NAN'!\n", data.getTimeStamp(), i);
#endif
            return false;
        }

        // TC Validity Check
        if (data.getType() == TC_ID && (points[i] < TC_LOW || points[i] > TC_MAX))
        {
#ifdef VALIDITY_P
            printf("Error %u: TC validity check failed with '%d = %u'!\n", data.getTimeStamp(), i, points[i]);
#endif
            return false;
        }

        // ACC Validity Check
        if (data.getType() == ACC_ID && (points[i] < ACC_LOW || points[i] > ACC_HIGH))
        {
#ifdef VALIDITY_P
            printf("Error %u: ACC validity check failed with '%d = %u'!\n", data.getTimeStamp(), i, points[i]);
#endif
            return false;
        }
    }
    return true;
}

void *PackagingThread(void *arguments)
{
    DataSelector dataSelector(&sensors);
    std::vector<DataPoint *> dataList;
    std::ifstream sensorFile;
    uint8_t *buffer;
    uint8_t newPacket[PACKET_SIZE];

    // Constantly create new packets
    while (true)
    {
        delay(6000);

        // Initialize newPacket to zeros
        memset(newPacket, '\0', PACKET_SIZE);

        sem_wait(&sensor1Sem);
        // Select data
        dataList = *dataSelector.selectData();
        sem_post(&sensor1Sem);

        // If there was data, create a new packet
        if (!dataList.empty())
        {
            // Read each data point from sensor file
            unsigned int startingPos = 0;
            for (DataPoint *dataInfo : dataList)
            {
                #ifdef DEBUG_P
                std::cout << "list size: " << dataList.size() << std::endl;

                std::cout << "debug: " << dataInfo->sensor_id << " " << dataInfo->fileIndex << std::endl;
#endif
                // Open the sensor file
                sem_wait(&sensor1Sem);
                std::string path = SENSOR_DATA_PATH;
                path += std::to_string(dataInfo->sensor_id);
                sensorFile.open(path, std::ios_base::binary);

                if (sensorFile.is_open())
                {
                    // Go to the line
                    std::cout << "index: " << dataInfo->fileIndex << std::endl;
                    sensorFile.seekg(dataInfo->fileIndex);

                    // Find the number of bytes for that type
                    unsigned int numBits = sensors.sensorMap[dataInfo->sensor_id]->numBitsPerDataPoint;
                    unsigned int numBytes = (numBits + 7) / 8;

                    // Read the bytes
                    buffer = (uint8_t *)malloc(numBytes);
                    sensorFile.read((char *)buffer, numBytes);
                    if (!sensorFile)
                        std::cout << "ERROR: only " << sensorFile.gcount() << " bytes could be read, " << numBytes << " expected\n";
                    else
                    {
                        // Add to the packet
                        copyBitsB(buffer, 0, newPacket, startingPos, numBits);
                        startingPos += numBits;
                    }

                    // Clean up
                    sensorFile.close();
                    free(buffer);
                }
                else
                {
                    std::cout << "ERROR: could not open " << path << std::endl;
                }

                sem_post(&sensor1Sem);
            }
        }

        // Access packet buffer
        sem_wait(&packetSem);

        // If the previous packet was used, mark data as sent
        sem_wait(&dataUsedSem);
        if (dataUsed)
        {
            dataSelector.markUsed();
            dataUsed = false;
        }
        sem_post(&dataUsedSem);
        // If there was new data, write new packet
        if (!dataList.empty())
        {
            memcpy(packetBuffer, newPacket, PACKET_SIZE);
        }

#ifdef PACKET_P // Print the packet for debugging
        printf("Generated packet: %s\n", packetBuffer);
#endif
        // Release packet buffer
        sem_post(&packetSem);
    } // end while(true)

    free(newPacket);
    return NULL;
} // end PackagingThread

void *IOThread(void *arguments)
{
    // Initialization
    int fd;
    if ((fd = serialOpen(SERIAL_FEATHER, 115200)) < 0)
    {
        fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
        exit(1);
    }
    printf("Connected to %s!\n", SERIAL_FEATHER);
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "Unable to start wiringPi: %s\n", strerror(errno));
        exit(1);
    }
    printf("wiringPi set up!\n");
    printf("Listening for sensor data!\n");

    // Variables for reading in data
    char letter;
    char line[MAX_CHARS];
    int pos = 0;
    std::string code = "";

    int packetFileName = 0;

    while (true)
    {
        // Gets the first int, converts to char
        letter = (char)serialGetchar(fd);
        line[pos] = letter;
        pos++;

        // End of line
        if (letter == '\n')
        {
            int x = 0;
            // Get command/sensor id
            while (line[x] != ',' && line[x] != '\n')
            {
                if (line[x] != '\n' || line[x] != '\r')
                {
                    code += line[x];
                    x++;
                }
            }
#ifdef DEBUG_P
            printf("CODE: %s\n", code.c_str());
            printf("CODE to INT: %d\n", stoi(code));
#endif
            // Return most recent packet if command
            try
            {
                if (stoi(code) == PACKET_REQUEST)
                {
                    sem_wait(&packetSem);
#ifdef DEBUG_P
                    printf("PACKET REQUEST RECEIVED:\n");
#endif
#ifdef PACKET_P
                    printf("Sent packet: %s\n", packetBuffer);
#endif
                    serialPuts(fd, packetBuffer);
                    sem_post(&packetSem);
                    sem_wait(&dataUsedSem);
                    dataUsed = true;
                    sem_post(&dataUsedSem);

                    // Save sent packet
                    // std::ofstream packetDataFile;
                    std::ofstream packetDataFile;
                    std::string path = PACKET_DATA_PATH;
                    path += std::to_string(packetFileName);

                    packetDataFile.open(path, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
                    if (packetDataFile.fail())
                    {
                        printf("ERROR OPENING PACKET DATA FILE: %s\n", path.c_str());
                    }
                    else
                    {
                        sem_wait(&packetSem);
                        // Writes 1 packet of size PACKET_SIZE from packetBuffer to packetDataFile
                        packetDataFile.write(packetBuffer, sizeof(packetBuffer));
#ifdef PACKET_P
                        printf("Saved packet %s to file!\n", packetBuffer);
#endif
                        sem_post(&packetSem);
                        packetFileName++;
                        packetDataFile.close();
                    }
                }
                // Otherwise, save data
                else
                {

#ifdef PRINT_DATA
                    printf("%s\n", line);
#endif

                    try
                    {
                        // Write to the sensor data file
                        Data datum(line, &sensors);
                        int size = datum.getNumBytes();
                        char temp_buf[size];
                        memset(&temp_buf, 0, size);
                        datum.createBitBuffer(temp_buf);

                        sem_wait(&sensor1Sem);

                        // Save data after checking validity
                        if (checkValid(datum))
                            saveData(datum, temp_buf);
                        sem_post(&sensor1Sem);
                    }
                    catch (std::string e)
                    {
                        printf("%s\n", e.c_str());
                    }
                } // end else
            }     // end try
            catch (const std::exception &e)
            {
#ifdef DEBUG_P
                printf("Invalid code received\n");
#endif
            }

            // Reset variables
            pos = 0;
            code = "";
            memset(line, '\0', MAX_CHARS * sizeof(char));
        } // end if
    }     // end while(true)

    return NULL;
} // end IOThread

int main()
{
    // Initialize packet buffer to zeros
    memset(packetBuffer, '\0', PACKET_SIZE);

    sensors.addSensor(TC_ID, TC_PRIORITY, TC_NUM_SAMPLES_PER_DATA_POINT, TC_NUM_BITS_PER_SAMPLE);
    sensors.addSensor(IMU_ID, IMU_PRIORITY, IMU_NUM_SAMPLES_PER_DATA_POINT, IMU_NUM_BITS_PER_SAMPLE);
    sensors.addSensor(GPS_ID, GPS_PRIORITY, GPS_NUM_SAMPLES_PER_DATA_POINT, GPS_NUM_BITS_PER_SAMPLE);
    sensors.addSensor(RMC_ID, RMC_PRIORITY, RMC_NUM_SAMPLES_PER_DATA_POINT, RMC_NUM_BITS_PER_SAMPLE);
    sensors.addSensor(ACC_ID, ACC_PRIORITY, ACC_NUM_SAMPLES_PER_DATA_POINT, ACC_NUM_BITS_PER_SAMPLE);
    sensors.addSensor(PRES_ID, PRES_PRIORITY, PRES_NUM_SAMPLES_PER_DATA_POINT, PRES_NUM_BITS_PER_SAMPLE, PRES_MULT);
    sensors.addSensor(SPEC_ID, SPEC_PRIORITY, SPEC_NUM_SAMPLES_PER_DATA_POINT, SPEC_NUM_BITS_PER_SAMPLE);

    // Start semaphores
    if (sem_init(&dataUsedSem, 0, 1) != 0)
    {
        printf("ERROR: Semaphore failed\n");
    }
    if (sem_init(&packetSem, 0, 1) != 0)
    {
        printf("ERROR: Semaphore failed\n");
    }

    if (sem_init(&sensor1Sem, 0, 1) != 0)
    {
        printf("ERROR: Semaphore failed\n");
    }

    // Start threads
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, IOThread, NULL);
    pthread_create(&thread2, NULL, PackagingThread, NULL);
    pthread_exit(0);
} // end main
