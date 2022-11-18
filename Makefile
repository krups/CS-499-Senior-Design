FLAGS = -Wall -lwiringPi -pthread -std=c++17
test_f = -g -Wall -pthread -std=c++17
GOAL = kaps

run: $(GOAL)
	sudo ./$(GOAL)

kaps:
	g++ -o $(GOAL) main.cpp data.cpp DataSelector.cpp $(FLAGS)

cleanData:
	rm -rf ./data/*

test: test.cpp data.cpp Sensors.cpp copyBits.cpp
	g++ -o test $^ $(test_f)
clean:
	rm -f $(GOAL) a.out

cleanAll: cleanData clean