CXX=g++
CXXFLAGS=-std=c++11
LIBS=-lsfml-graphics -lsfml-window -lsfml-system

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

main: main.o
	$(CXX) main.o -o main $(LIBS)

run: main
	./main
