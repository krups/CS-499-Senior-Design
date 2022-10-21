FLAGS = -Wall -lwiringPi -pthread
GOAL = kaps

run: $(GOAL)
	sudo ./$(GOAL)

kaps:
	g++ -o $(GOAL) main.cpp data.cpp $(FLAGS)

cleanData:
	rm -rf ./data/*

test: main.cpp
	g++ -o test main.cpp -Wall -pthread

clean: cleanData
	rm -f $(GOAL) a.out
