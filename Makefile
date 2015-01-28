all: main

main: 
	g++ -std=c++11 -Wall main.cpp PNGImage.cpp -o render
