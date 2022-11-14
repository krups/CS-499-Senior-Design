FLAGS = -Wall -lwiringPi -pthread -std=c++17
GOAL = kaps

run: $(GOAL)
	sudo ./$(GOAL)

kaps:
	g++ -o $(GOAL) main.cpp data.cpp DataSelector.cpp Sensors.cpp $(FLAGS)

cleanData:
	rm -rf ./data/*

test: test.cpp data.cpp
	g++ -o test test.cpp data.cpp copyBits.cpp -Wall -pthread
	./test
clean:
	rm -f $(GOAL) a.out

cleanAll: cleanData clean