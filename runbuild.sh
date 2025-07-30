#! /bin/bash

# print info about the start of the compilation
echo "[INFO] Started to compile GLGE"
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
# check if the compillation was successfull
if [ $? -ne 0 ]; then
    # print an error message
    echo "[ERROR] An error occoured during the compillation of GLGE or a test program. Please check the error log created by G++."
    # stop the program
    exit -1
fi
# go back into the main directory
cd ..
# inform about the ending of the compillation
echo "[INFO] Finished GLGE compillation"
echo ""

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
    echo [INFO] Running example hub
    ./build/EXAMPLE_HUB_CPP
    echo [INFO] Finished example hub

fi