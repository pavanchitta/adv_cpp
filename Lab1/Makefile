CC=g++
CXXFLAGS= -I. -std=c++17 -g

DEPS = engine.h regex.h testbase.h
OBJ = test_regex.o testbase.o engine.o regex.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CXXFLAGS)

test_regex: $(OBJ) 
	$(CC) -o $@ $^ $(CXXFLAGS)

clean:
	rm -f *.o test_regex
