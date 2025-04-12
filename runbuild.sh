#! /bin/bash
if [ ! -d "build" ]; then
    mkdir build
fi
cd build
cmake ..
make -j
cd ..
echo [INFO] Started shader compilation
res=$(./compileShaders.sh)
if [ "$res" -eq "1" ]; then
    echo [ERROR] Shader compelation error
    exit 1
else 
    echo [INFO] Shaders compiled without errors
fi
echo -e "\n"
if [ $# == 0 ]; then

    echo [INFO] Running C Test
    ./build/TEST_C
    echo [INFO] Finished C Test

    echo -e ""

    echo [INFO] Running C++ Test
    ./build/TEST_CPP
    echo [INFO] Finished C++ Test

fi