CC=g++
CXXFLAGS=-g -std=c++17 -I.

DEPS=mbrot.h bbrot.h image.h cbqueue.h
OBJ = bbrot.o mbrot.o


%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CXXFLAGS)

bbrot: $(OBJ)
	$(CC) -o $@ $^ $(CXXFLAGS)

clean: 
	rm -f *.o bbrot
