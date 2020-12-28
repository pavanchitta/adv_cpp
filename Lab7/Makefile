CC=g++
CXXFLAGS=-std=c++17 -Wall  -g -I.

DEPS=treeset.h testbase.h
OBJ = testbase.o test-treeset.o 

%.o : %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CXXFLAGS)

test : $(OBJ)
	$(CC) -o $@ $^ $(CXXFLAGS)


clean: 
	rm -f *.o test
