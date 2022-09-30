#include <stdio.h>
#include <string>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include "config.h"

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
  for (;;)
  {
    putchar (serialGetchar (fd)) ;
    fflush (stdout) ;
  }
  return 0;
}