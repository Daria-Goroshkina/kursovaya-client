CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDLIBS = -lssl -lcrypto

TARGET = client
OBJS = main.o interface.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

main.o: main.cpp interface.h
	$(CXX) $(CXXFLAGS) -c main.cpp

interface.o: interface.cpp interface.h
	$(CXX) $(CXXFLAGS) -c interface.cpp

clean:
	rm -f $(OBJS) $(TARGET)
