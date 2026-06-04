CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

SRC_DIR = collector
SRCS = $(SRC_DIR)/main.cpp $(SRC_DIR)/container.cpp $(SRC_DIR)/metrics.cpp $(SRC_DIR)/display.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = cgroup-top

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
