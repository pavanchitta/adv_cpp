CC=g++
CXXFLAGS=-std=c++17 -Wall -Werror -g -I.

DEPS=treeset.h testbase.h
OBJ = treeset.o testbase.o test-treeset.o 

%.o : %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CXXFLAGS)

test : $(OBJ)
	$(CC) -o $@ $^ $(CXXFLAGS)


clean: 
	rm -f *.o test
