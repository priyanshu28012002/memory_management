# Compiler
CXX = g++
# Compiler flags
CXXFLAGS = -Wall -Wextra -O2 -std=c++17
# Output executable
TARGET = memory
# Source files
SRC = myalloc.cpp

# Default target: Compile the program
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean up compiled files
clean:
	rm -f $(TARGET)
