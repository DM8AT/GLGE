# Graphic Library Game Engine (GLGE)
GLGE is a cross-platform open source game engine designed for 2D and 3D game development in C and C++. The structure is very object-oriented with an instance based structure of memory ownership. 

## Project status
[![C/C++ CI](https://github.com/DM8AT/GLGE/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/DM8AT/GLGE/actions/workflows/c-cpp.yml)

GLGE is currently in a very early state of development. The API is not fully created and currently only one API backend is partially set up. 

In the current state, only the C++ API is functional. A lot of the C bindings are currently missing. 

As for the cross-platform support, only linux (with debian-based distributions) is tested. With a bit of setup it should be possible to compile on Windows and MacOS, but there are currently no one-click solutions for those operating systems available. 

# Installation
The library can be installed using either the pre-build binaries from the releases + the header zip or by building the library from scratch. 

## Pre-build binaries
The pre-build library binaries for a static library are dependent on the operating system the project should run on. The naming convention for releases is operating system dependent (as they expect different file endings and own naming patterns). 

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
First, install all necessary libraries. For all linux distributions with the apt package manager the bash-script `installDependencies.sh` is provided to run the installation of all non-submodule dependencies. For MacOS and Windows, you need to install the libraries and set the inclusion path yourself. The libraries are: 

| Library name | Optional | Description of usage |
|--------------|----------|----------------------|
| OpenGL       | :x:      | Used for the OpenGL (GL) graphic API backends |
| GLEW         | :x:      | A binding library to load and access the OpenGL functions |
| SDL2         | :x:      | A cross platform library to interface with the windowing system as well as keyboard, mouse and controller/game-pads |
| SDL2_image   | :x:      | Used when loading an image from disk |
| Vulkan       | :x:      | Necessary for the vulkan (VK) backends |

### Compiling the library
Before the GLGE library can be compiled, all submodules need to be compiled to static libraries. 

To compile imgui and assimp, check out the building instructions. For Pugixml create a directory named `pugixml_build` in the directory that contains the build shell script. After that you can run the shell script to build it (if you are on linux) or compile the library into the new folder. 

After that, compile all cpp and c files in the folder `GLGE` into object files and link them together with the build libraries into a single static library or shared object (dynamic linked library). On Linux, simply run `./runbuild.sh s` while in the top-level directory (the directory containing this readme) to compile the library and examples or `./runbuild.sh` to build the library and build and run the examples. 

# Using GLGE
To use GLGE, first include the header file `GLGE/GLGE.h`. This will automatically include all headers for GLGE and make the feature set selected by the settings available. 

## Customizing GLGE compile-time and header settings
Some settings in GLGE can be changed in a settings file that is located at `GLGE/GLGECore/GLGESettings.h`. This is mainly used to customize the usage of GLGE, but it can also be used to select a subset of features. This can be useful for server build because a lot of libraries are not needed when some features are not selected. 

| Setting name | Define | Requires Re-compile | Default value | Safe to change |
|--------------|--------|---------------------|---------------|----------------|
| GLGE Version | GLGE_VERSION | :white_check_mark: | 0.1.0 (Currently) | Only for development |
| Is GLGE in a namespace | GLGE_CREATE_NAMESPACE | :x: | 0 / false | :white_check_mark: |
| The name of the GLGE namespace | GLGE_NAMESPACE | :x: | GLGE | :white_check_mark: |
| Is the usage of SIM-D allowed | GLGE_USE_SIMD | :white_check_mark: | 1 / true | :white_check_mark: |
| Are compile-time errors allowed | GLGE_COMPILER_ERRORS | :white_check_mark: | 1 / true | :warning: |
| Are compile-time warnings allowed | GLGE_COMPILER_WARNINGS | :white_check_mark: | 1 / true | :warning: |
| Defines if the build is a debug build | GLGE_ENABLE_DEBUGGING | :white_check_mark: | 1 / true | :white_check_mark: |
| Specifies if the math library is included | GLGE_INCLUDE_MATH | :white_check_mark: | 1 / true | :x: |
| Specifies if the graphic section if GLGE is included | GLGE_INCLUDE_GRAPHICS | :white_check_mark: | 1 / true | :white_check_mark: |
| Stores the amount of samples for the limiter to take and use for smoothing | GLGE_LIMITER_IPS_SAMPLES | :white_check_mark: | 32 | :white_check_mark: |
| Define if bindings for the 3rd party library `ImGUI` should be included | GLGE_3RD_PARTY_INCLUDE_DEAR_IMGUI | :white_check_mark: | 1 / true | :white_check_mark: |

## Documentation
GLGE is documented using the github wiki. Currently, the documentation is being build up and expended, so it is expected to be not complete. If you find that something is missing in the documentation, please open an issue and mention it. 