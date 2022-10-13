FLAGS = -Wall -lwiringPi -pthread
GOAL = kaps

run: $(GOAL)
	sudo ./$(GOAL)

kaps:
	g++ -o $(GOAL) main.cpp data.cpp $(FLAGS)

clean:
	rm -f $(GOAL) a.out
	rm -rf ./data/*
