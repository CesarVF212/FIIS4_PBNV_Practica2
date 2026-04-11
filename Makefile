CXX = g++
CXXFLAGS = -Wall -std=c++17 -Wno-sign-compare -Wno-unused-parameter

TARGET = main
SRCS = mainBMP.cpp
HEADERS = Pixels.h BMPHeader.h DIBHeaders.h BMPReader.h

all: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)