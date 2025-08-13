# Graphic Library Game Engine (GLGE)
GLGE is a cross-platform open source game engine designed for 2D and 3D game developement in C and C++. The structure is very object-oriented with an instance based structure of memory ownership. 

## Project status
[![C/C++ CI](https://github.com/DM8AT/GLGE/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/DM8AT/GLGE/actions/workflows/c-cpp.yml)

GLGE is currently in a very early state of developement. The API is not fully created and currently only one API backend is partially set up. 

In the current state, only the C++ API is functional. A lot of the C bindings are currently missing. 

As for the cross-platform support, only linux (with debian-based distributions) is tested. With a bit of setup it should be possible to compile on Windows and MacOS, but there are currently no one-click solutions for those operating systems available. 

# Installation
The library can be installed using either the pre-build binaries from the releases + the header zip or by building the library from scratch. 

## Pre-build binaries
The pre-build library binaries for a static library are dependend on the operating system the project should run on. The naming convention for releases is operating system dependend (as they expect different file endings and own naming patterns). 

For the convention patterns in the examples, `[IS]` is replaced by the identifier for the instruction set. 

| OS Name | Naming convention | Example |
|---------|-------------------|---------|
| Linux (Debian) | libGLGE_[IS].a | libGLGE_x86.a | 
| Windows | GLGE_[IS].lib | GLGE_x86.lib |
| MacOS | libGLGE_[IS].a | libGLGE_ARM.a |

In the future there may also be support for dynamic libraries. 

## Building GLGE yourself
To build GLGE yourself, first clone the branch in question to your system. Make sure to clone recursively because GLGE uses submodules that won't be cloned the missing submodules will lead to problems compiling the library. 

### Installing the libraries
First, install all neccecery libraries. For all linux distributions with the apt package manager the bash-script `installDependencies.sh` is provided to run the installation of all non-submodule dependencies. For MacOS and Windows, you need to install the libraries and set the inclusion pathes yourself. The libraries are: 

| Library name | Optional | Description of usage |
|--------------|----------|----------------------|
| OpenGL       | :x:      | Used for the OpenGL (GL) graphic API backends |
| GLEW         | :x:      | A binding librariy to load and access the OpenGL functions |
| SDL2         | :x:      | A cross platform library to interface with the windowing system as well as keyboard, mouse and controller/gampads |
| SDL2_image   | :x:      | Used when loading an image from disk |
| Vulkan       | :x:      | Neccecery for the vulkan (VK) backends |

### Compiling the library
Before the GLGE library can be compiled, all submodules need to be compiled to static libraries. 

To compile imgui and assimp, check out the building instructions. For Pugixml create a directory named `pugixml_build` in the directory that contains the build shell script. After that you can run the shell script to build it (if you are on linux) or compile the library into the new folder. 

After that, compile all cpp and c files in the folder `GLGE` into object files and link them together with the build libraries into a single static library or shared object (dynamic linked library). On Linux, simply run `./runbuild.sh s` while in the top-level directory (the directory containing this readme) to compile the library and examples or `./runbuild.sh` to build the library and build and run the examples. 
