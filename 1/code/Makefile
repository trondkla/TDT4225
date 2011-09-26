SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
EXECUTABLE=program

CC = g++
CFLAGS = -Wall
LIBS = 
INCLUDES = -I.

.PHONY: all
all: $(SRC) $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	$(CC) $(OBJ) $(LIBS) -o $@

.cpp.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJ) *~ \#*