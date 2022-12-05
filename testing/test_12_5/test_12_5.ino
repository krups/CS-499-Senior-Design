#include <string>
#include <Adafruit_NeoPixel.h>

int i = 0;

void printInvalid()
{
    // higher acc
    Serial1.println("5,1,2,3,16");

    // lower acc
    Serial1.println("5,2,-1,9,10");

    // tc lower
    Serial1.println("1,3,-1001,0,3,4");

    // invalid id
    Serial1.println("27,4,1,2,3,4");

    // read a string
    Serial1.println("5,5,hello,krupsy,day");

    // NaN (how to do this? pls send help)
    Serial1.println("5,6,NaN,NaN,NaN");

    // too few data points
    Serial1.println("5,7,1,2");

    // too many data points
    Serial1.println("5,8,1,2,3,4");

    // no timestamp/too few data points
    Serial1.println("5,50,34");
}

void  printMain()
{
    // maybe
    // send 7 tc
    Serial1.println("1,2,50,100,1000,1500"); //
    Serial1.println("1,3,0,-50,-100,-500");
    Serial1.println("1,4,-500,500,-900,900");
    Serial1.println("1,5,0,0,0,1250");       //
    Serial1.println("1,6,9000,500,-250,-50");
    Serial1.println("1,7,420,450,600,750");  //
    Serial1.println("1,1,1,2,3,4");

    // TEMP: use this instead of negatives until that is fixed
    // Serial1.println("1,3,0,50,100,500");
    // Serial1.println("1,4,500,500,900,900");
    // Serial1.println("1,6,9000,500,250,50");

    // Send 9 acc, last one is invalid
    Serial1.println("5,8.5,0,0,1");  // noninteger
    Serial1.println("5,9,5,5,0");
    Serial1.println("5,10,1,2,3"); 
    Serial1.println("5,11,1,2,3");
    Serial1.println("5,12,0,1,1");
    Serial1.println("5,13,9,9,5");
    Serial1.println("5,14,9,7,5");
    Serial1.println("5,15,1,2,3");
    Serial1.println("5,0,1,2,10");

    // send 6 spec
    Serial1.println("10,16,1,2,3.7,4.8888886887388,5"); // noninteger
    Serial1.println("10,17,5,4,3,2,1");
    Serial1.println("10,18,1,2,0,4,5");
    Serial1.println("10,19,5,4,0,2,1");
    Serial1.println("10,20,2,0,2,2,2");
    Serial1.println("10,21,2,2,2,0,2");

    // send 16 tc
    Serial1.println("1,22,1,12,3.33,4");  // noninteger          
    Serial1.println("1,63,50,100,1000,1500");
    Serial1.println("1,12,0,-50,-100,-500");
    Serial1.println("1,25,-570,500,-910,900");
    Serial1.println("1,26,0,0,-3,1250");
    Serial1.println("1,27,9001,500,-250,-50");
    Serial1.println("1,28,426,450,600,750");   
    Serial1.println("1,29,1,20,3,7");          
    Serial1.println("1,30,50,137,1300,1500");  
    Serial1.println("1,31,-10,-51,-100,-500");
    Serial1.println("1,32,-500,10,-900,900");
    Serial1.println("1,33,1,-50,0,1250");
    Serial1.println("1,34,9000,500,-250,-50");
    Serial1.println("1,35,350,450,400,750");   
    Serial1.println("1,36,0,0,-572,631");
    Serial1.println("1,37,1,2,3,75");

    // TEMP: use this until negatives are fixed
    // Serial1.println("1,12,0,50,100,500");
    // Serial1.println("1,25,570,500,910,900");
    // Serial1.println("1,26,0,0,3,1250");
    // Serial1.println("1,27,9001,500,250,50");
    // Serial1.println("1,31,10,51,100,500");
    // Serial1.println("1,32,500,10,900,900");
    // Serial1.println("1,33,1,50,0,1250");
    // Serial1.println("1,34,9000,500,250,50");
    // Serial1.println("1,36,0,0,572,631");
    
    // send 40 acc
    Serial1.println("5,38,1,0,1");
    Serial1.println("5,39,2,5,0");
    Serial1.println("5,40,10,2,3");
    Serial1.println("5,41,12,13,0");
    Serial1.println("5,42,9,1,1");
    Serial1.println("5,43,9,9,5");
    Serial1.println("5,44,9,3,5");
    Serial1.println("5,45,1,2,3");
    Serial1.println("5,46,2,2,1");
    Serial1.println("5,47,0,0,1");
    Serial1.println("5,48,5,0,0");
    Serial1.println("5,49,1,2,3");
    Serial1.println("5,50,10,0,0");
    Serial1.println("5,51,0,1,2");
    Serial1.println("5,52,9,9,5");
    Serial1.println("5,53,9,5,0");
    Serial1.println("5,54,1,2,3");
    Serial1.println("5,55,1,2,3");
    Serial1.println("5,56,0,0,1");
    Serial1.println("5,57,5,5,0");
    Serial1.println("5,58,1,2,3");
    Serial1.println("5,59,2,2,3");
    Serial1.println("5,60,0,1,1");
    Serial1.println("5,61,9,9,5");
    Serial1.println("5,62,9,5,5");
    Serial1.println("5,63,1,2,3");
    Serial1.println("5,64,1,2,1");
    Serial1.println("5,65,0,0,1");
    Serial1.println("5,66,5,5,0");
    Serial1.println("5,67,1,2,3");
    Serial1.println("5,68,1,2,3");
    Serial1.println("5,69,0,1,1");
    Serial1.println("5,70,9,9,5");
    Serial1.println("5,71,9,5,5");
    Serial1.println("5,72,1,2,3");
    Serial1.println("5,73,1,5,3");
    Serial1.println("5,74,11,2,5");
    Serial1.println("5,75,1,2,6");
    Serial1.println("5,76,1,2,7");
    Serial1.println("5,77,1,2,11");

    // send 18 spec
    Serial1.println("10,78,1,5,3,4,5");
    Serial1.println("10,79,1,4,3,2,1");
    Serial1.println("10,80,1,3,0,4,5");
    Serial1.println("10,81,1,4,0,2,1");
    Serial1.println("10,82,1,0,2,4,2");
    Serial1.println("10,83,5,2,2,0,2");
    Serial1.println("10,84,1,5,3,4,5");
    Serial1.println("10,85,5,4,3,3,1");
    Serial1.println("10,86,1,2,1,4,5");
    Serial1.println("10,87,5,4,0,3,1");
    
    Serial1.println("10,88,2,0,4,3,2");
    Serial1.println("10,89,2,2,1,0,2");
    Serial1.println("10,90,1,2,3,1,5");
    Serial1.println("10,91,5,1,3,2,1");
    Serial1.println("10,92,1,1,0,4,5");
    Serial1.println("10,93,5,4,2,1,1");
    Serial1.println("10,94,2,0,2,5,1");
    Serial1.println("10,95,2,3,2,0,4");
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
    pinMode(5, INPUT_PULLUP);  // this is the button pin
    led.begin();
    led.show();
}

void loop()
{
    /*
    * Part 1: Invalid data
    */
    Serial.println("Waiting to start part 1");
    buttonWait(5);
    Serial.println("Starting part 1");
    // Send invalid data
    printInvalid();
    delay(1000);
    // Request packet
    Serial1.println(0x8);
    delay(1000);
    led.setPixelColor(0, led.Color(150, 0, 0));  // red
    led.show();

    /*
    * Part 2: Main packet
    */
    Serial.println("Waiting to start part 2");
    buttonWait(5);
    Serial.println("Starting part 2");
    // Send main data
    printMain();
    delay(1000);
    // Request packet
    Serial1.println(0x8);
    delay(1000);
    led.setPixelColor(0, led.Color(0, 150, 0)); // green
    led.show();

    /*
    * Part 3: Do it again
    */
    Serial.println("Waiting to start part 3");
    buttonWait(5);
    Serial.println("Starting part 3");

    // Request packet
    Serial1.println(0x8);
    delay(1000);
    led.setPixelColor(0, led.Color(0, 0, 150));  // blue
    led.show();

    /*
    * Part 4: Ran out of data
    */
    Serial.println("Waiting to start part 4");
    buttonWait(5);
    Serial.println("Starting part 4");

    // Request packet
    Serial1.println(0x8);
    delay(1000);
    Serial.println("Testing complete");
    delay(1000);
    led.setPixelColor(0, led.Color(50, 0, 150)); // magenta
    led.show();
}