#! /bin/bash

# build message
echo "[BUILD INFO] Running automatic documentation step..."

# if the build directory doesn't exist, then create it
if [ ! -d "build" ]; then
    mkdir build
    cd build 
    mkdir doc
    cd ..
else
    # check if build/doc exists. If it doesn't exist, create it
    if [ ! -d "build/doc" ]; then
        cd build
        mkdir doc
        cd ..
    fi;
fi;

# run the actual documentation and store the result
cd GLGE
doxygen
Res=$?
cd ..

# build message
echo "[BUILD INFO] Finished automatic documentation step"

# return the result of the documentation
exit $Res