#include <string>
#include <Adafruit_NeoPixel.h>
#include "serial_headers.h"

int i = 0;

void printTC()
{
    int n = 4;
    int values[n];
    for (int j = 0; j < n; j++)
    {
        values[j] = random() % 1700;
    }
    Serial1.print(TC_SERIAL);
    Serial1.print(',');
    Serial1.print(i);
    for (int j = 0; j < n; j++)
    {
        Serial1.print(',');
        Serial1.print(values[j]);
    }
    Serial1.print('\n');
}

void printACC()
{
    int n = 3;
    int values[n];
    for (int j = 0; j < n; j++)
    {
        values[j] = (random() % 15);
    }
    Serial1.print(ACC_SERIAL);
    Serial1.print(',');
    Serial1.print(i);
    for (int j = 0; j < n; j++)
    {
        Serial1.print(',');
        Serial1.print(values[j]);
    }
    Serial1.print('\n');
}

void printIMU()
{
    int n = 6;
    int values[n];
    for (int j = 0; j < 3; j++)
    {
        values[j] = random() % 20;
    }
    for (int j = 3; j < n; j++)
    {
        values[j] = random() % 2000;
    }
    Serial1.print(IMU_SERIAL);
    Serial1.print(',');
    Serial1.print(i);
    for (int j = 0; j < n; j++)
    {
        Serial1.print(',');
        Serial1.print(values[j]);
    }
    Serial1.print('\n');
}
void buttonWait(int buttonPin)
{
    int buttonState = 0;
    while (1)
    {
        buttonState = digitalRead(buttonPin);
        if (buttonState == LOW)
        {
            return;
        }
    }
}

Adafruit_NeoPixel led(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

void setup()
{
    Serial1.begin(115200);
    pinMode(5, INPUT_PULLUP);
    led.begin();
    led.show();
}

void loop()
{
    led.setPixelColor(0, led.Color(0, 0, 0));
    led.show();
    if (!(i % 100))
        Serial1.println(0x8);
    else
    {
        int option = random() % 3;
        switch (option)
        {
        case 0:
            printTC();
            break;
        case 1:
            printACC();
            break;
        case 2:
            printIMU();
            break;
        default:
            break;
        }
    }
    i++;
    buttonWait(5);
    led.setPixelColor(0, led.Color(0, 150, 0));
    led.show();

    delay(300);
}