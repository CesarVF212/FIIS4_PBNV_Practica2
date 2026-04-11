CXX = g++
CXXFLAGS = -Wall -std=c++17

TARGET = main
SRCS = mainBMP.cpp
HEADERS = BMPReader.h

all: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)