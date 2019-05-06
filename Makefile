CXX = g++
CXXFLAGS = -std=c++11 -O1 -ggdb -pedantic -Wall -Wextra
TARGETS = memory_pool

all: $(TARGETS)

clean:
	rm -f $(TARGETS)
