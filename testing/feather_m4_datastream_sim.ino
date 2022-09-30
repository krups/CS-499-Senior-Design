#include <string>

void setup()
{
    Serial1.begin(115200);
}

void loop()
{
    int i = random() % 50;
    while (1)
    {
        int n = 6;
        int values[n];
        for (int j = 0; j < n; j++)
        {
            values[j] = random() % 1000;
        }
        Serial1.print(0x101);
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
        i++;
        delay(random() % 300);
    }
}