#! /bin/bash

# print info about the start of the compilation
echo "[INFO] Started to compile GLGE"

# unused
# check if assimp exists
#if [ -d "src/GLGE/GLGE3rdParty/assimp" ]; then
#    # Print some info
#    echo "[INFO] Building Assimp"
#
#    # run cmake for assimp
#    cd src/GLGE/GLGE3rdParty/assimp
#    cmake . -DBUILD_SHARED_LIBS=OFF -DASSIMP_INSTALL=OFF
#    
#    # check if the compile was successfull
#    if [ $? -ne 0 ]; then
#        # if not, print an error and stop
#        echo "[ERROR] Failed to compile GLGE because assimp could not be compiled"
#        exit 1
#    fi
#
#    cd ../../../../
#
#    # run the makefile for the library
#    cd "src/GLGE/GLGE3rdParty/assimp"
#    make -j
#    # change back to the main directory
#    cd ../../../../
#
#    # Print that the building is done
#    echo "[INFO] Finished building assimp"
#fi

# compile pugixml
cd src/GLGE/GLGE3rdParty/pugixml
bash build.sh

# check if the compile was successfull
if [ $? -ne 0 ]; then
    # if not, print an error and stop
    echo "[ERROR] Failed to compile GLGE because pugixml could not be compiled"
    exit 1
fi

cd ../../../../

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

    # leave on new line
    echo -e ""

    # run the C++ test program
    echo [INFO] Running example hub
    ./build/EXAMPLE_HUB_CPP
    echo [INFO] Finished example hub

fi