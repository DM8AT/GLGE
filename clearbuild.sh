#! /bin/bash

# print status info
echo "[INFO] Removing GLGE binaries"
#delete the binaries
rm -r build

# check if assimp exists
if [ -d "src/GLGE/GLGE3rdParty/assimp" ]; then

    # print an info that assimp is being cleand
    echo "[INFO] Clearing assimp binaries"

    # delete the library
    rm src/GLGE/GLGE3rdParty/assimp/lib/libassimp.a
    # delete the binaries
    rm -r src/GLGE/GLGE3rdParty/assimp/bin

    # info that assimp binaries are removed
    echo "[INFO] Finished clearing assimp binaries"

fi

# message that everything is done
echo "[INFO] Finished clearing the build data"