CXX := g++
CXXFLAGS := -std=c++17  -Wextra -O2 -Wno-missing-field-initializers \
            -I/usr/local/include -I/usr/X11R6/include
LDFLAGS := -L/usr/local/lib -L/usr/X11R6/lib
LDLIBS := -lGLEW -lglfw -lGLU -lGL -lm

SRC := main.cpp
OBJ := $(SRC:.cpp=.o)
TARGET := main

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

