# Makefile

CXX := g++
CXXFLAGS := -std=c++17 -Wall -O1
DFLAGS := -std=c++17 -Wall -ggdb -O0
TARGET := test
INCLUDE_DIR := ./include

all: main.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp -I$(INCLUDE_DIR)

debug: main.cpp
	$(CXX) $(DFLAGS) -o $(TARGET) main.cpp -I$(INCLUDE_DIR)

clean:
	rm $(TARGET)