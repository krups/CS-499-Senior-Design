#include <stdio.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <fstream>
#include <iostream> // used for printing vectors in tests
#include <cstdint>
#include <math.h>

#include "serial_headers.h"
#include "data.h"
#include "config.h"

void saveData(Data data)
{
#ifdef PRINT_DATA
  data.printData();
#endif
  std::ofstream sensorDataFile;
  std::string path = sensorDataPath;
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

int main()
{
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

  for (;;)
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
        code += line[x];
        x++;
      }

      // If command
      if (stoi(code) == PACKET_REQUEST) // or something like this
      {
        printf("Sending packet!\n");
        serialPuts(fd, "hi");
      }
      // Save data
      else
      {
#ifdef PRINT_DATA
        printf("line = %s\n", line);
#endif
        Data datum(line);
        // saveData(line) or whatever
        if (checkValid(datum))
          saveData(datum);
      }

      // // Prints all data within vector that was just saved, if PRINT_DATA is set
      // #ifdef PRINT_DATA
      //       pos = 0;
      //       letter = line[pos];
      //       while (letter != '\n')
      //       {
      //         printf("%c", letter);
      //         letter = line[++pos];
      //       }
      //       printf("\n");
      // #endif

      // Reset variables
      pos = 0;
      code = "";
      memset(line, '\0', MAX_CHARS * sizeof(char));

    } // end if
  }   // end for

  return 0;
} // end main()