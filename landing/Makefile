CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

all:sls

sls:sls.cpp
	g++ -o sls sls.cpp `pkg-config opencv --cflags --libs` -lpthread -std=c++11 -g

clean:
	\rm sls
