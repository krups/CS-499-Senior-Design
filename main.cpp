#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include "config.h"

int main (void)
{
  int fd ;
  int count ;
  unsigned int nextTime ;

  if ((fd = serialOpen (SERIAL_FEATHER, 115200)) < 0)
  {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
  }

  if (wiringPiSetup () == -1)
  {
    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
    return 1 ;
  }

  nextTime = millis () + 300 ;

  for (count = 0 ; count < 256 ; )
  {
    if (millis () > nextTime)
    {
      printf ("\nOut: %3d: ", count) ;
      fflush (stdout) ;
      serialPutchar (fd, count) ; // Sends over TX
      nextTime += 300 ;
      ++count ;
    }

    delay (3) ;

    while (serialDataAvail (fd))
    {
      printf (" -> %3d", serialGetchar (fd)) ; // Receives from RX
      fflush (stdout) ;
    }
  }

  printf ("\n") ;
  return 0 ;
}