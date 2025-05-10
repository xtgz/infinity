~#!/usr/bin/env bash

# build_mac.sh
# Auto-generates Makefile.mac with system-specific CDEFINES/LDFLAGS and builds the project.

PYBIN="${1:-python3}"
echo "Using Python executable: $PYBIN"
echo

# Get pybind11 include
PYBIND11_INCLUDE=$($PYBIN -c "import pybind11; print(pybind11.get_include())" 2>/dev/null)
if [ -z "$PYBIND11_INCLUDE" ]; then
    echo "[ERROR] pybind11 not found. Please install with 'pip install pybind11'."
    exit 1
fi

# Get Python include
PYTHON_INCLUDE=$($PYBIN -c "import sysconfig; print(sysconfig.get_paths().get('include',''))" 2>/dev/null)
PYTHON_LIBDIR=$($PYBIN -c "import sysconfig; print(sysconfig.get_config_var('LIBDIR') or '')" 2>/dev/null)
PYTHON_VERSION=$($PYBIN -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')" 2>/dev/null)

if [ -z "$PYTHON_INCLUDE" ] || [ -z "$PYTHON_LIBDIR" ]; then
    echo "[ERROR] Failed to detect Python include or lib paths."
    exit 1
fi

# Generate Makefile.mac
cat <<EOF > Makefile.mac
CC = g++

CDEFINES = -I$PYBIND11_INCLUDE -I$PYTHON_INCLUDE

SOURCES = Dispatcher.cpp Mode.cpp precomp.cpp SpeedSample.cpp bindings.cpp
OBJECTS = \$(SOURCES:.cpp=.o)

PYMODULE = magicXorMiner.so

LDFLAGS = -framework OpenCL \\
          -L$PYTHON_LIBDIR \\
          -lpython$PYTHON_VERSION

CFLAGS = -c -std=c++11 -Wall -O2 -fPIC

all: \$(PYMODULE)

\$(PYMODULE): \$(OBJECTS)
	\$(CC) -shared \$(OBJECTS) \$(LDFLAGS) -o \$@

%.o: %.cpp
	\$(CC) \$(CFLAGS) \$(CDEFINES) \$< -o \$@

clean:
	rm -f *.o

.PHONY: all clean
EOF

# Build
echo "==> Running build using Makefile.mac..."
make -f Makefile.mac clean && make -f Makefile.mac && make -f Makefile.mac clean

# Optionally remove Makefile.mac
# rm Makefile.mac~
