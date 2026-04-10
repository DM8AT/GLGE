#! /bin/bash

# create the build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi;

# build message
echo "[BUILD INFO] Started the build step for GLGE"

# build the actual project and store if it was built successfully
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
#cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j24
Ret=$?
cd ..

# sanity check the result
if [ ! $Ret -eq 0 ]; then
    # error message
    echo "[BUILD ERROR] Failed to build GLGE"
    exit 1
fi;

# build message
echo "[BUILD INFO] Finished build for GLGE"

# document the project
./document.sh

# sanity check the documentation result
if [ ! $? -eq 0 ]; then
    # error message
    echo "[BUILD ERROR] Failed to generate the documentation for GLGE"
    exit 2
fi;

# build the shader
cd assets/shader
./compile.sh
res=$?
cd ../..

# sanity check if the shader were build
if [ ! $res -eq 0 ]; then
    # error message
    echo "[BUILD ERROR] Failed to compile the shader for GLGE"
    exit 3
fi;

# build message
echo "[BUILD INFO] Running the built executable"

# finally, run the program
./build/MAIN

# sanity check the program's return value
if [ ! $? -eq 0 ]; then 
    # error message
    echo "[BUILD ERROR] An error occurred while running the built executable"
    exit 4
fi;