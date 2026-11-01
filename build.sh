#! /bin/bash

# start the timer
START_TIME=$SECONDS

# configure build files
if [ ! -d "build" ]; then
    mkdir build
    cmake -DCMAKE_BUILD_TYPE=Debug -B build examples -G "Ninja"
fi;

# get available RAM in MB
RAM_MB=$(free -m | awk '/^Mem:/ {print $2}')
# calculate build threads (3 threads per 8 GB)
THREADS=$((RAM_MB * 3 / 8192))
# cap thread count to core count
CPU_CORES=$(nproc)
BUILD_THREADS=$((BUILD_THREADS < CPU_CORES ? BUILD_THREADS : CPU_CORES))
# ensure at least one thread
if [ "$THREADS" -lt 1 ]; then
    THREADS=1
fi
# debug messages
echo "Available RAM: ${RAM_MB} MB"
echo "Using ${THREADS} build threads"

# actual building
cmake --build build --parallel $THREADS -- -d stats

# calculate elapsed time
ELAPSED_TIME=$((SECONDS - START_TIME))

# print elapsed time
echo "-----------------------------------"
echo "Build completed in ${ELAPSED_TIME} seconds."
echo "-----------------------------------"