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

char packetBuffer[PACKET_SIZE];
sem_t packetSem;
sem_t sensor1Sem;


void saveData(Data data)
{
#ifdef PRINT_DATA
  data.printData();
#endif
  std::ofstream sensorDataFile;
  std::string path = SENSOR_DATA_PATH;
  path += std::to_string(data.getType());

  // SEMAPHORE WAIT

  sensorDataFile.open(path, std::ios_base::app);
  if (!sensorDataFile.fail())
  {
    sensorDataFile << data;
#ifdef PRINT_DATA
    printf("Saved %d, %u to file!\n", data.getType(), data.getTimeStamp());
#endif
  }
  else
#ifdef PRINT_DATA
    printf("Open: %s failed!\n", path);
#endif
  sensorDataFile.close();

  // SEMAPHORE SIGNAL
}

bool checkValid(Data data)
{
  std::vector<u_int16_t> points = data.getData();
  for (int i = 0; i < data.getNumVals(); i++)
  { // Add checks to correspond with validity parameters in config.h

    // Check for valid numbers
    if (points[i] == NAN)
    {
#ifdef VALIDITY
      printf("Error %u: Validity check failed! with '%d is NAN'!\n", data.getTimeStamp(), i);
#endif
      return false;
    }

    // TC Validity Check
    if (data.getType() == TC_SERIAL && (points[i] < TC_LOW || points[i] > TC_MAX))
    {
#ifdef VALIDITY
      printf("Error %u: TC validity check failed with '%d = %u'!\n", data.getTimeStamp(), i, points[i]);
#endif
      return false;
    }

    // ACC Validity Check
    if (data.getType() == ACC_SERIAL && (points[i] < ACC_LOW || points[i] > ACC_HIGH))
    {
#ifdef VALIDITY
      printf("Error %u: ACC validity check failed with '%d = %u'!\n", data.getTimeStamp(), i, points[i]);
#endif
      return false;
    }
  }
  return true;
}


void * PackagingThread (void * arguments) {
    printf("THREAD 2: Packaging your data here\n");

    for (int i = 0; i < 10; i++) {
        // File path of sensor data file
        std::string path = SENSOR_DATA_PATH;
        path += "favWord";

        // Read the sensor data file
        std::string data;
        sem_wait(&sensor1Sem);
        printf("THREAD 2: Reading data from file\n");
        std::ifstream sensorDataFile(path);
        if (sensorDataFile.is_open()) {
            getline(sensorDataFile, data);
            sensorDataFile.close();
        }
        sem_post(&sensor1Sem);

        // Write first character to packet buffer
        sem_wait(&packetSem);
        printf("THREAD 2: Putting data in packet\n");
        packetBuffer[0] = data[0];
        sem_post(&packetSem);
    }

    return NULL;
}

void * IOThread (void * arguments) {
    printf("Hello feather controller\n");

    // Initialization
    int fd;
    if ((fd = serialOpen(SERIAL_FEATHER, 115200)) < 0)
    {
        fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
        return 1;
    }
    printf("Connected to %s!\n", SERIAL_FEATHER);
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "Unable to start wiringPi: %s\n", strerror(errno));
        return 1;
    }
    printf("wiringPi set up!\n");
    printf("Listening for serial!\n");

    // Variables for reading in data
    char letter;
    char line[MAX_CHARS];
    int pos = 0;
    std::string code = "";

    while(true) {
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
                code += line[x];
                x++;
            }
                
            // Return most recent packet if command
            if (stoi(code) == PACKET_REQUEST) // or something like this
            {
                sem_wait(&packetSem);
                printf("THREAD 1: Sending packet to terminal\n");
                printf("Your favorite character is %c\n", packetBuffer[0]);
                sem_post(&packetSem);   
            }
            // Otherwise, save data
            else {
                // Get the data
                getline(serialInput, data);

                // File path of sensor data file
                std::ofstream sensorDataFile;
                std::string path = SENSOR_DATA_PATH;
                path += command;

                // Write to the sensor data file
                sem_wait(&sensor1Sem);
                printf("THREAD 1: Writing data to file\n");
                sensorDataFile.open(path, std::ios_base::app);
                if (!sensorDataFile.fail()) {
                    std::string line = command + "," + data + "\n";
                    #ifdef PRINT_DATA
                        printf("line = %s\n", line);
                    #endif
                    Data datum(const_cast<char*>(line.c_str()));
                    // saveData(line) or whatever
                    if (checkValid(datum))
                        saveData(datum);
                    sensorDataFile.close();
                }
                sem_post(&sensor1Sem);   
            }
            // Reset variables
            pos = 0;
            code = "";
            memset(line, '\0', MAX_CHARS * sizeof(char));
        }
    }



    for (int i = 0; i < 10; i++) {
        // Read first part of input
        getline(serialInput, command, ',');
        std::cout << command << std::endl;

         
        }
    }
    return NULL;
}

int main() {
    printf("I'm having a KRUPSy day\n");

    // Start semaphores
    if ( sem_init(&packetSem, 0, 1) != 0 ) {
       printf("ERROR: Semaphore failed\n");
    }

    if ( sem_init(&sensor1Sem, 0, 1) != 0 ) {
       printf("ERROR: Semaphore failed\n");
    }

    // Start threads
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, PackagingThread, NULL);
    pthread_create(&thread2, NULL, IOThread, NULL);
    pthread_exit(0);
}
