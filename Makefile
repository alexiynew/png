all: main

main: 
	g++ -std=c++11 -Wall -g main.cpp PNGImage.cpp -o render
