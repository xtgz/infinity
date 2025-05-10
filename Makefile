CC = g++

# Include paths
CDEFINES = -I/usr/include/python3.10
CDEFINES += -I/usr/local/lib/python3.10/dist-packages/pybind11/include

# Source files
SOURCES = Dispatcher.cpp Mode.cpp precomp.cpp SpeedSample.cpp bindings.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Name for Python shared library
PYMODULE = magicXorMiner.so

# Linker flags for Linux 
LDFLAGS = -shared -lOpenCL
# Compiler flags for Linux
CFLAGS = -c -std=c++11 -Wall -O2 -fPIC

# Default target
all: $(PYMODULE)

# Link the shared library
$(PYMODULE): $(OBJECTS)
	$(CC) -shared $(OBJECTS) $(LDFLAGS) -o $@

# Compile source files to object files
%.o: %.cpp
	$(CC) $(CFLAGS) $(CDEFINES) $< -o $@

# Clean up
clean:
	rm -rf *.o $(PYMODULE)

.PHONY: all clean