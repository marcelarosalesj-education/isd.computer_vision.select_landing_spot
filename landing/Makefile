CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

all:sls

sls:sls.cpp
	g++ -std=c++11 -o sls sls.cpp `pkg-config opencv --cflags --libs`

clean:
	\rm sls
