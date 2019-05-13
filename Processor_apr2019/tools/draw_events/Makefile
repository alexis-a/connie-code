CFITSIO = $(FITSIOROOT)
CPP = g++
CC = gcc
CFLAGS = -Wall $(shell root-config --cflags) -I$(CFITSIO) -O3
LIBS = -L$(CFITSIO) -lcfitsio -lm $(shell root-config --libs)
GLIBS =
GLIBS +=
#OBJECTS = draw_events.o
#HEADERS = globalConstants.h
OBJECTS = draw_events.o tinyxml2.o gConfig.o globalConstants.o
HEADERS = globalConstants.h tinyxml2.h gConfig.h

ALL : draw_events.exe
#ALL : gConfig.o
	@echo "Listo!"

draw_events.exe : $(OBJECTS)
	$(CPP) $(OBJECTS) -o draw_events.exe $(LIBS) $(GLIBS) $(CFLAGS)

draw_events.o : draw_events.cc $(HEADERS)
	$(CPP) -c draw_events.cc -o draw_events.o $(CFLAGS)

gConfig.o : gConfig.cc $(HEADERS)
	$(CPP) -c gConfig.cc -o gConfig.o $(CFLAGS)

tinyxml2.o : tinyxml2.cpp $(HEADERS)
	$(CPP) -c tinyxml2.cpp -o tinyxml2.o $(CFLAGS)

globalConstants.o : globalConstants.c $(HEADERS)
	$(CPP) -c globalConstants.c -o globalConstants.o $(CFLAGS)

clean:
	rm -f *~ *.o *.exe