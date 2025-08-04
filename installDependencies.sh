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

# install the dependencies using apt
apt install -y libsdl2-dev libsdl2-image-dev libgl-dev libglew-dev libvulkan-dev vulkan-validationlayers

# print a message that the install is finished
echo "[INFO] Finished the installation of all dependencies"