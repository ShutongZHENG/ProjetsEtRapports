CC = g++
CXXFLAGS = -std=c++11 -pthread -Wall
DEPS = analyseLexicale.hpp structData.hpp Graph.hpp
OBJS = Extraction.o analyseLexicale.o Graph.o

all: Extraction

Extraction: $(OBJS)
	$(CC) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(DEPS)
	$(CC) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean exe
clean:
	rm -f $(OBJS) Extraction

exe: Extraction
	./Extraction brenda.ssa
