# Desert Engine makefile
# Copyright 2017 Bogdan Kozyrev t800@kvkozyrev.org
# All rights reserved.

#CC = clang++ -I../
CC = clang++
CFLAGS = -Wall -std=gnu++11 -O2 -DLINUX
ONLYCOMPILE = $(CC) $(CFLAGS) -c -g
OBJFILES :=  $(patsubst %.cpp,%.o,$(wildcard *.cpp))
PROGNAME = engine

all: $(PROGNAME)

$(PROGNAME) : $(OBJFILES)
	$(CC) -o $(PROGNAME) $(OBJFILES)

%.o: %.cpp
	${ONLYCOMPILE} -o $@ $<

clean:
	rm *.o
