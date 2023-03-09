FLAGS = -g -Wall -lwiringPi -pthread -std=c++17
test_f = -g -Wall -pthread -std=c++17
GOAL = kaps

run: $(GOAL)
	sudo ./$(GOAL)

kaps: main.cpp data.cpp DataSelector.cpp Sensors.cpp copyBits.cpp config.h
	g++ -o $(GOAL) $^ $(FLAGS)

cleanData:
	rm -rf ./data/*

cleanPackets:
	rm -rf ./packets/*

test: test.cpp data.cpp Sensors.cpp copyBits.cpp
	g++ -o test $^ $(test_f)

decode: decode.cpp Sensors.cpp copyBits.cpp config.h
	g++ -o decode $^ $(test_f)

clean:
	rm -f $(GOAL) a.out

cleanAll: cleanData cleanPackets clean