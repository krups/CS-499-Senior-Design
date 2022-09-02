# CS-499-Senior-Design
Project Team: Christopher Cox, Joshua Morford, Susanna Moser, Evan Wells

# Problem Statement
As the capsule enters the Earth’s atmosphere, data from the thermocouple and spectrometer are packaged and transmitted to the Iridium network. Currently, the packaging and transmission is linear/brute-force. Data is collected, packed, and transmitted with no assessment of quality of data. This approach is limited as very few data packets are received because of unstable connection between the Iridium network and the modem. Additionally, spectrometer collects light as function of both wavelength and altitude, thereby growing the dimensionality of the problem.

To solve this issue, a code framework is needed that performs adaptive sampling such that only the most pertinent data is packaged and transmitted. Within the scope of a senior design, we would like the team to construct the sampling for data from the thermocouples and the spectrometer. A bonus task would be to keep the code generic such that efficient data reduction and packaging can be performed for an arbitrary set of input variables.
