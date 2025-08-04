#! /bin/bash

# print info that pugixml is being compiled
echo "[INFO] Compiling pugixml"

# check if the build directory exists
if [ ! -d "pugixml_build" ]; then
    # create the build directory
    mkdir pugixml_build
fi 

# go into the pugixml building directory
cd pugixml_build
echo $PATH
# there, run cmake from the pugixml directory
cmake ../pugixml
# then, build the library
make -j

# print the info that the pugixml compillation is finished
echo "[INFO] Finished pugixml compillation"