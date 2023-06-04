CC = g++
CFLAGS = -Wall -Wextra -pedantic -std=c++11
LDFLAGS =

TARGET = mm1sm
SOURCES = mm1sm.cpp simstats.cpp reports.cpp
HEADERS = simstats.h reports.h
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
