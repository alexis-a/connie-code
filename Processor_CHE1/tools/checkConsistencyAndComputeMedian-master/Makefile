CFITSIO = $(FITSIOROOT)
CPP = g++
CC = gcc
CFLAGS = -Wall -I$(CFITSIO) -O3  -fopenmp
LIBS = -L$(CFITSIO) -lcfitsio -lm 
GLIBS = 
GLIBS += 
OBJECTS = checkConsistencyAndComputeMedian.o 
HEADERS = globalConstants.h

ALL : checkConsistencyAndComputeMedian.exe
	@echo "Listo!"

checkConsistencyAndComputeMedian.exe : $(OBJECTS)
	$(CPP) $(OBJECTS) -o checkConsistencyAndComputeMedian.exe $(LIBS) $(GLIBS) $(CFLAGS)

checkConsistencyAndComputeMedian.o : checkConsistencyAndComputeMedian.cc $(HEADERS)
	$(CPP) -c checkConsistencyAndComputeMedian.cc -o checkConsistencyAndComputeMedian.o $(CFLAGS)

clean:
	rm -f *~ *.o *.exe
