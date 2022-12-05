# CS-499-Senior-Design
Project Team: Christopher Cox, Joshua Morford, Susanna Moser, Evan Wells

# Problem Statement
As the capsule enters the Earth’s atmosphere, data from the thermocouple and spectrometer are packaged and transmitted to the Iridium network. Currently, the packaging and transmission is linear/brute-force. Data is collected, packed, and transmitted with no assessment of quality of data. This approach is limited as very few data packets are received because of unstable connection between the Iridium network and the modem. Additionally, spectrometer collects light as function of both wavelength and altitude, thereby growing the dimensionality of the problem.

To solve this issue, a code framework is needed that performs adaptive sampling such that only the most pertinent data is packaged and transmitted. Within the scope of a senior design, we would like the team to construct the sampling for data from the thermocouples and the spectrometer. A bonus task would be to keep the code generic such that efficient data reduction and packaging can be performed for an arbitrary set of input variables.

# How the program works


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
