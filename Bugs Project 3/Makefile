CC = /usr/local/cs/bin/g32

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

.PHONY: default all clean

all: testbugs

%.o: %.cpp $(HEADERS)
	$(CC) -c -std=c++14 -Wno-unused-parameter -Wno-unused-variable -Wno-reorder -Wno-misleading-indentation -Wno-switch $< -o $@

testbugs: $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@

clean:
	rm -f *.o
	rm -f testbugs
