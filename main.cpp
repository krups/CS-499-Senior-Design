#include <iostream>
#include <fstream>
#include <string>
#include <pthread.h>
#include <semaphore.h>
#include "config.h"

char packetBuffer[PACKET_SIZE];
sem_t packetSem;
sem_t sensor1Sem;

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

    for (int i = 0; i < 10; i++) {
        // File path of sensor data file
        std::ofstream sensorDataFile;
        std::string path = SENSOR_DATA_PATH;
        path += "favWord";

        // Write to the sensor data file
        sem_wait(&sensor1Sem);
        printf("THREAD 1: Writing data to file\n");
        sensorDataFile.open(path, std::ios_base::app);
        if (!sensorDataFile.fail()) {
            sensorDataFile << "baboon";
            sensorDataFile.close();
        }
        sem_post(&sensor1Sem);

        // Read the most recent created packet
        sem_wait(&packetSem);
        printf("THREAD 1: Sending packet to terminal\n");
        printf("Your favorite character is %c\n", packetBuffer[0]);
        sem_post(&packetSem);        
    }
    return NULL;
}

int main() {
    printf("I'm having a KRUPSy day\n");

    if ( sem_init(&packetSem, 0, 1) != 0 ) {
       printf("ERROR: Semaphore failed\n");
    }

    if ( sem_init(&sensor1Sem, 0, 1) != 0 ) {
       printf("ERROR: Semaphore failed\n");
    }

    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, PackagingThread, NULL);
    pthread_create(&thread2, NULL, IOThread, NULL);
    pthread_exit(0);
}
