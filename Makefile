MAIN_DIR = .
LOG_DIR = ./log

INC_DIR = -I$(LOG_DIR)

SOURCES = $(wildcard $(LOG_DIR)/*.cpp) \
		  $(wildcard $(MAIN_DIR)/*.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SOURCES))

TARGET = main
CXX = g++
CXXFLAGS = -std=c++11 ${INC_DIR}
LFLAGS = -lpthread

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LFLAGS)

$(OBJS): %.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(TARGET)