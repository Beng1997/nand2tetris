CXX = g++
CXXFLAGS = -std=c++11 -Wall

# Source files
SRCS = Main.cpp jacktokenizer.cpp compliation_engine.cpp

# Object files (automatically generated from SRCS)
OBJS = $(SRCS:.cpp=.o)

# Executable name
TARGET = JackAnalyzer

# Default rule
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile C++ source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)
