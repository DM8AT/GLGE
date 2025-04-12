#! /bin/bash

# check if the build directory exists
if [ ! -d "build" ]; then
    # if not, create it
    mkdir build
fi
# go into the build directory
cd build
# create the makefile using cmake
cmake ..
# compile the code 
make -j
# go back into the main directory
cd ..

# enable / disable shader compillation (1: enabled | 0: disabled)
compileShader=0
# check if the shader should be compiled
if [ "$compileShader" -eq "1" ]; then
    # print that shaders will be compiled
    echo [INFO] Started shader compilation
    # compile the shader
    res=$(./compileShaders.sh)
    # check if the shader compiled without errors
    if [ "$res" -eq "1" ]; then
        # if errors occourd, print that errors where encounterd
        echo [ERROR] Shader compelation error
        # close the program with errors
        exit 1
    # if no errors where encounterd
    else 
        # print that all shaders compiled fine
        echo [INFO] Shaders compiled without errors
    fi
    # leave some space to the output of the rest
    echo -e "\n"
fi

# check if there are command line arguments
# command line arguments can be used to disable the auto-run of the program
if [ $# == 0 ]; then

    # run the C test program
    echo [INFO] Running C Test
    ./build/TEST_C
    echo [INFO] Finished C Test

    # leave on new line
    echo -e ""

    # run the C++ test program
    echo [INFO] Running C++ Test
    ./build/TEST_CPP
    echo [INFO] Finished C++ Test

fi