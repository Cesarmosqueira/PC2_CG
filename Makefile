CC  := gcc
CXX := g++
CP  := cp
RM  := rm -rf
MD  := mkdir -p

CXXFLAGS += -std=c++17 -pedantic -Wall -O3
LDFLAGS += -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl

CPPFLAGS += -I include/ -I dependencies/
#all: dirs bin/program

default: 
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o bin/main.o main.cpp -g
	$(CXX) -o bin/program bin/*.o $(LDFLAGS) -g

stbi:
	$(CC) -c src/stb_image.c -I dependencies/ -o bin/stbi.o

glad:
	$(CC) -c src/glad.c -I dependencies/ -o bin/glad.o
clean: 
	rm -rf bin/*
