#include "config.h"

#include <iostream> // used for printing vectors in tests

void saveData(char* data) {
  int sensorId = (sensor_id) data;

  ofstream sensorDataFile;

  // SEMAPHORE WAIT

  sensorDataFile = open(sensorDataPath + sensorId, "a");

  if (!sensorDataFile.fail()) {
    sensorDataFile << data;
  }

  sensorDataFile.close();

  // SEMAPHORE SIGNAL
}

int main()
{
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
      if (stoi(code) == COMMAND) // or something like this
      {
        printf("Sending packet!\n");
        serialPuts(fd, "hi");
      }
      // Save data
      else
      {
        // saveData(line) or whatever
        saveData(line);
      }
      
      // Prints all data within vector that was just saved, if PRINT_DATA is set
      #ifdef PRINT_DATA
      pos = 0;
      letter = line[pos];
      while (letter != '\n')
      {
        printf("%c", letter);
        letter = line[++pos];
      }
      printf("\n");
      #endif
      
      // Reset variables
      pos = 0;
      code = "";
      memset(line, '\0', MAX_CHARS * sizeof(char));

    } // end if
  } // end for
  
  return 0;
} // end main()