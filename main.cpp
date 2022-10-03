#include <stdio.h>
#include <string>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include "config.h"

#include <iostream> // used for printing vectors in tests
#include <vector> // temporarily store data
#include <string> 

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
  std::string section = "";
  std::string line = "";
  std::vector<int> split_line;
  std::vector<std::vector<int>> data;

  for (int x = 0;;)
  {
    // Gets the first int, converts to char and adds to string
    letter = (char)serialGetchar(fd);
    line += letter;

    // End of line
    if (letter == '\n')
    {
      // Loop through every letter
      for (char letter: line)
      {
        // End of section
        if (letter == ',')
        {
          split_line.push_back(std::stoi(section));
          section = "";
        }
        // Section continues
        else
        {
          section += letter;
        }
      }

      // Add data to vectors
      split_line.push_back(std::stoi(section));
      data.push_back(split_line);

      // Resets variables
      line = "";
      section = "";
      split_line.clear();

      // Prints all data within vector that was just saved, if PRINT_DATA is set
      if (PRINT_DATA)
      {
        for (int j=0; j < (int) data.at(x).size(); j++)
        {
          std::cout << data.at(x).at(j) << ' ';
        }
        std::cout << '\n';
        x++;
      }
    }
  }
  
  return 0;
}