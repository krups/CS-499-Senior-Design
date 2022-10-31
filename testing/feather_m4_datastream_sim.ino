#include <string>
#include "serial_headers.h"

int i = 0;

void printTC()
{
    int n = 6;
    int values[n];
    for (int j = 0; j < n; j++)
    {
        values[j] = random() % 1000;
    }
    Serial1.print(TC_SERIAL);
    Serial1.print(',');
    Serial1.print(i);
    Serial1.print(',');
    Serial1.print(n);
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
        values[j] = (random() % 150);
    }
    Serial1.print(ACC_SERIAL);
    Serial1.print(',');
    Serial1.print(i);
    Serial1.print(',');
    Serial1.print(n);
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
        values[j] = random() % 360;
    }
    Serial1.print(IMU_SERIAL);
    Serial1.print(',');
    Serial1.print(i);
    Serial1.print(',');
    Serial1.print(n);
    for (int j = 0; j < n; j++)
    {
        Serial1.print(',');
        Serial1.print(values[j]);
    }
    Serial1.print('\n');
}

void setup()
{
    Serial1.begin(115200);
}

void loop()
{
    while (1)
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
        i++;
        delay(random() % 150);
    }
}