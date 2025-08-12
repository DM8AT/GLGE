#! /bin/bash

# install all dependencies for GLGE under linux using the apt package manager

# print a message that the install started
echo "[INFO] Started to install all dependencies for GLGE"

# the program requires root to install everything
if [[ $EUID -ne 0 ]]; then
    # print an error message and stop
    echo "[ERROR] You must be root to run this program"
    exit 1
fi

# install SDL2 and SDL2 image using apt
apt install -y libsdl2-dev libsdl2-image-dev

# check if the SDL2 install was successfull
if [ $? -ne 0 ]; then
    # if not, print an error and stop
    echo "[ERROR] Failed to install SDL2 libraries"
    exit 1
fi

# install OpenGL using apt
apt install -y libgl-dev libglew-dev

# check if the SDL2 install was successfull
if [ $? -ne 0 ]; then
    # if not, print an error and stop
    echo "[ERROR] Failed to install OpenGL / GLEW libraries"
    exit 1
fi

# print a message that the install is finished
echo "[INFO] Finished the installation of all dependencies"