CC=g++
OPTS=-g

all: main.o predictor.o
	$(CC) $(OPTS) -lm -o predictor main.o predictor.o

main.o: main.cpp predictor.hpp
	$(CC) $(OPTS) -c main.cpp

predictor.o: predictor.hpp predictor.cpp
	$(CC) $(OPTS) -c predictor.cpp

clean:
	rm -f *.o predictor;
