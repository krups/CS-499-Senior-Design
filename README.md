# CS-499-Senior-Design
Project Team: Christopher Cox, Joshua Morford, Susanna Moser, Evan Wells

# Problem Statement
As the capsule enters the Earth’s atmosphere, data from the thermocouple and spectrometer are packaged and transmitted to the Iridium network. Currently, the packaging and transmission is linear/brute-force. Data is collected, packed, and transmitted with no assessment of quality of data. This approach is limited as very few data packets are received because of unstable connection between the Iridium network and the modem. Additionally, spectrometer collects light as function of both wavelength and altitude, thereby growing the dimensionality of the problem.

To solve this issue, a code framework is needed that performs adaptive sampling such that only the most pertinent data is packaged and transmitted. Within the scope of a senior design, we would like the team to construct the sampling for data from the thermocouples and the spectrometer. A bonus task would be to keep the code generic such that efficient data reduction and packaging can be performed for an arbitrary set of input variables.

# How the program works
![88407785-F246-4D2D-9B8F-11695B1C94CA](https://user-images.githubusercontent.com/41971210/205755514-5db6eaef-b308-42f8-ba25-865613a06237.png)

The Feather Controller sends data and commands over serial to this program, specifically the I/O thread. This data is processed by validating it, converting its representation to a more compact bit format, and saving it to storage.

The packaging thread takes this saved data and selects data to be packaged. This data is then packaged into a packet and saved to a packet buffer.

The I/O thread waits for a packet request and sends what is in the packet buffer when one is received. When the I/O thread sends a packet to the Feather Controller, it also saves a copy of the packet to storage.

# How to compile
Using the included makefile makes compiling very easy, just run the command `make kaps` or `make` to compile and run. To manually compile, run `g++ -o <executable name> *.cpp -g -Wall -lwiringPi -pthread -std=c++17`.

# How to run
To run the program, first compile the program into an executable either manually or using a variation of the `make` command. After compilation, the executable can be run to run the program. Alternatively, the command `make` or `make run` can be used to compile and immediately run the program.

# How to test
To test the program, the compilation option for testing can be used: `make test`. This compiles the program into a testing executable which can then be run to test the program.

# How to use the configuration file
The configuration file `config.h` contains many variable definitions for the program. The variables in this file can be modified to change their values everywhere the variable is used.

Some common variables that may need to be changed:
* What file paths should be used
* What serial device should be used
* IDs, priorities, and precision multipliers for each sensor
* Validity bounds for each sensor
* Data selection:
  * Ratio of new to old data
  * The maximum times each data point should be sent
  * What type of data selection algorithm should be used
* Whether debug statements should be printed

# Adding a new sensor
To add a new sensor to the program, the following things must be done:
1) Make new #define configuration parameters for the sensor in the "Sensor Configuration" section of `config.h` that immitate the existing sensor parameters in that section. Set the parameters to values specific to the new sensor being added. (Ensure that the sensor id is unique at the very least.)
2) Go to `main.cpp` and find the `main()` function. Add another line calling `sensors.addSensor(...)` with the configuration parameters that were just created in `config.h`.
3) Copy the `sensors.addSensor(...)` line that was just added in `main()` in `main.cpp` and paste the same line to the same location in `main()` in `decode.cpp`.

Notes about sensor parameters:
- Generally speaking, the number of bits per sample should be the smallest number of bits required to uniquely represent all possible outputs from the sensor.
- If a sensor can output negative numbers, then the sensor offset needs to be equal to the negation of the lowest possible negative number that the sensor can output. (For example, if a sensor can output down to -124, then the offset needs to be 124.) Otherwise the offset should be 0.
- If a sensor outputs decimal values and you want to preserve those demical values, then set the multiplier to 10<sup>n</sup> where n is the number of decimal places you wish to preserve. Otherwise the multiplier should be 1.

To add validation parameters to the new sensor, the following things must be done:
1) Make new #define configuration parameters for the sensor in the "Valdity Configuration" section of `config.h` that immitate the existing sensor parameters in that section. Set the high and low values as desired.
2) In `main.cpp` find the `checkValid()` function. Copy one of the existing validity checks (each check is an if statement) and then change the paramenters used in the if statement to match the ones for the sensor you are adding validity checks for.
3) Update the debug print and any other comments to match the sensor you are making the validity check for.