/**
 * @file GLGESettings.h
 * @author DM8AT
 * @brief this file is used to define the settings for GLGE
 * @version 0.1
 * @date 2025-04-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef GLGE_SETTINGS
#define GLGE_SETTINGS

/*

$$$$$$$\  $$$$$$$$\ $$$$$$$$\  $$$$$$\  $$\   $$\ $$\    $$$$$$$$\       $$\       $$$$$$\ $$$$$$$\  $$$$$$$\   $$$$$$\  $$$$$$$\ $$\     $$\  $$$$$$\  
$$  __$$\ $$  _____|$$  _____|$$  __$$\ $$ |  $$ |$$ |   \__$$  __|      $$ |      \_$$  _|$$  __$$\ $$  __$$\ $$  __$$\ $$  __$$\\$$\   $$  |$$  __$$\ 
$$ |  $$ |$$ |      $$ |      $$ /  $$ |$$ |  $$ |$$ |      $$ |         $$ |        $$ |  $$ |  $$ |$$ |  $$ |$$ /  $$ |$$ |  $$ |\$$\ $$  / $$ /  \__|
$$ |  $$ |$$$$$\    $$$$$\    $$$$$$$$ |$$ |  $$ |$$ |      $$ |         $$ |        $$ |  $$$$$$$\ |$$$$$$$  |$$$$$$$$ |$$$$$$$  | \$$$$  /  \$$$$$$\  
$$ |  $$ |$$  __|   $$  __|   $$  __$$ |$$ |  $$ |$$ |      $$ |         $$ |        $$ |  $$  __$$\ $$  __$$< $$  __$$ |$$  __$$<   \$$  /    \____$$\ 
$$ |  $$ |$$ |      $$ |      $$ |  $$ |$$ |  $$ |$$ |      $$ |         $$ |        $$ |  $$ |  $$ |$$ |  $$ |$$ |  $$ |$$ |  $$ |   $$ |    $$\   $$ |
$$$$$$$  |$$$$$$$$\ $$ |      $$ |  $$ |\$$$$$$  |$$$$$$$$\ $$ |         $$$$$$$$\ $$$$$$\ $$$$$$$  |$$ |  $$ |$$ |  $$ |$$ |  $$ |   $$ |    \$$$$$$  |
\_______/ \________|\__|      \__|  \__| \______/ \________|\__|         \________|\______|\_______/ \__|  \__|\__|  \__|\__|  \__|   \__|     \______/ 

*/

//include the C default librarys

//for default C functions
#include <stdlib.h>
//include integer with a specific length
#include <stdint.h>
//include the C default input output
#include <stdio.h>
//include booleans
#include <stdbool.h>
//include modules to work with strings
#include <string.h>
//include the time
#include <time.h>

//check for C++
#ifdef __cplusplus

//include iostream for default input-output
#include <iostream>
//include the string class for easy working with strings
#include <string>
//include the string view class for constant strings
#include <string_view>
//include vectors
#include <vector>
//include dictionaris alias unorderd maps
#include <unordered_map>
//include threads
#include <thread>
//include the time
#include <ctime>
//include output streams
#include <ostream>
//include files
#include <fstream>
//include the file system
#include <filesystem>
//include string streams
#include <sstream>
//include algorithms
#include <algorithm>
//include mutices
#include <mutex>

/**
 * @brief print an std::vector into an output stream
 * 
 * @param os a reference to the out stream to print into
 * @param v the vector to print
 * @return std::ostream& a reference to the output stream with the content
 */
template <typename T> std::ostream& operator<<(std::ostream& os, std::vector<T> v) noexcept
{
    //add a header
    os << "vector[" << v.size() << "]{";
    //iterate over all elements of the vector
    for (size_t i = 0; i < v.size(); ++i)
    {
        //print the element
        os << v[i];
    }
    //add a suffix and return
    return os << "}";
}

#endif //end of C++ section

/*

$$\    $$\ $$$$$$$$\ $$$$$$$\   $$$$$$\  $$$$$$\  $$$$$$\  $$\   $$\       $$\   $$\ $$\   $$\ $$\      $$\ $$$$$$$\  $$$$$$$$\ $$$$$$$\  $$$$$$\ $$\   $$\  $$$$$$\  
$$ |   $$ |$$  _____|$$  __$$\ $$  __$$\ \_$$  _|$$  __$$\ $$$\  $$ |      $$$\  $$ |$$ |  $$ |$$$\    $$$ |$$  __$$\ $$  _____|$$  __$$\ \_$$  _|$$$\  $$ |$$  __$$\ 
$$ |   $$ |$$ |      $$ |  $$ |$$ /  \__|  $$ |  $$ /  $$ |$$$$\ $$ |      $$$$\ $$ |$$ |  $$ |$$$$\  $$$$ |$$ |  $$ |$$ |      $$ |  $$ |  $$ |  $$$$\ $$ |$$ /  \__|
\$$\  $$  |$$$$$\    $$$$$$$  |\$$$$$$\    $$ |  $$ |  $$ |$$ $$\$$ |      $$ $$\$$ |$$ |  $$ |$$\$$\$$ $$ |$$$$$$$\ |$$$$$\    $$$$$$$  |  $$ |  $$ $$\$$ |$$ |$$$$\ 
 \$$\$$  / $$  __|   $$  __$$<  \____$$\   $$ |  $$ |  $$ |$$ \$$$$ |      $$ \$$$$ |$$ |  $$ |$$ \$$$  $$ |$$  __$$\ $$  __|   $$  __$$<   $$ |  $$ \$$$$ |$$ |\_$$ |
  \$$$  /  $$ |      $$ |  $$ |$$\   $$ |  $$ |  $$ |  $$ |$$ |\$$$ |      $$ |\$$$ |$$ |  $$ |$$ |\$  /$$ |$$ |  $$ |$$ |      $$ |  $$ |  $$ |  $$ |\$$$ |$$ |  $$ |
   \$  /   $$$$$$$$\ $$ |  $$ |\$$$$$$  |$$$$$$\  $$$$$$  |$$ | \$$ |      $$ | \$$ |\$$$$$$  |$$ | \_/ $$ |$$$$$$$  |$$$$$$$$\ $$ |  $$ |$$$$$$\ $$ | \$$ |\$$$$$$  |
    \_/    \________|\__|  \__| \______/ \______| \______/ \__|  \__|      \__|  \__| \______/ \__|     \__|\_______/ \________|\__|  \__|\______|\__|  \__| \______/ 

*/

/**
 * @brief a macro that is used to convert 3 version elements to a version number
 * 
 * Structure of a version number:
 * M = Major version (12 bits)
 * m = Minor version (12 bits)
 * p = Patch (8 bits)
 * MMMMMMMMMMMMmmmmmmmmmmmmpppppppp
 * 
 * @param major the major version number
 * @param minor the minor version number
 * @param patch the number of the patch
 */
#define GLGE_TO_VERSION(major, minor, patch) ((uint32_t)(((((uint32_t)major) & 0xFFF) << 20) | ((((uint32_t)minor) & 0xFFF) << 8) | ((((uint32_t)patch) & 0xFF))))

/**
 * @brief get the major version from a version identifyer
 * 
 * @param version the version number as defined by the macro GLGE_TO_VERSION
 */
#define GLGE_GET_MAJOR_VERSION(version) ((((uint32_t)version) >> 20) & 0xFFF)

/**
 * @brief get the minor version from a version identifyer
 * 
 * @param version the version number as defined by the macro GLGE_TO_VERSION
 */
#define GLGE_GET_MINOR_VERSION(version) ((((uint32_t)version) >> 8) & 0xFFF)

/**
 * @brief get the patch version from a version identifyer
 * 
 * @param version the version number as defined by the macro GLGE_TO_VERSION
 */
#define GLGE_GET_PATCH_VERSION(version) ((((uint32_t)version)) & 0xFF)

//store the version number for GLGE
#define GLGE_VERSION GLGE_TO_VERSION(0,1,0)

/*

 $$$$$$\        $$\  $$$$$$\                          $$$$$$\  $$$$$$$$\ $$$$$$$$\ $$$$$$$$\ $$$$$$\ $$\   $$\  $$$$$$\   $$$$$$\  
$$  __$$\      $$  |$$  __$$\ $$\       $$\          $$  __$$\ $$  _____|\__$$  __|\__$$  __|\_$$  _|$$$\  $$ |$$  __$$\ $$  __$$\ 
$$ /  \__|    $$  / $$ /  \__|$$ |      $$ |         $$ /  \__|$$ |         $$ |      $$ |     $$ |  $$$$\ $$ |$$ /  \__|$$ /  \__|
$$ |         $$  /  $$ |   $$$$$$$$\ $$$$$$$$\       \$$$$$$\  $$$$$\       $$ |      $$ |     $$ |  $$ $$\$$ |$$ |$$$$\ \$$$$$$\  
$$ |        $$  /   $$ |   \__$$  __|\__$$  __|       \____$$\ $$  __|      $$ |      $$ |     $$ |  $$ \$$$$ |$$ |\_$$ | \____$$\ 
$$ |  $$\  $$  /    $$ |  $$\ $$ |      $$ |         $$\   $$ |$$ |         $$ |      $$ |     $$ |  $$ |\$$$ |$$ |  $$ |$$\   $$ |
\$$$$$$  |$$  /     \$$$$$$  |\__|      \__|         \$$$$$$  |$$$$$$$$\    $$ |      $$ |   $$$$$$\ $$ | \$$ |\$$$$$$  |\$$$$$$  |
 \______/ \__/       \______/                         \______/ \________|   \__|      \__|   \______|\__|  \__| \______/  \______/ 

*/

//check if C++ is defined
#ifdef __cplusplus
//say that C++ is used (1 means true, 0 means false)
#define GLGE_CPP 1
#else
//say that C++ is not used (1 means true, 0 means false)
#define GLGE_CPP 0
#endif

//define if GLGE should create a namespace if C++ is used. If C is used, a namespace can not be created (1: a namespace will be created, 0: no namespace will be created)
#define GLGE_CREATE_NAMESPACE 0
//the name of the namespace GLGE will use
#define GLGE_NAMESPACE GLGE

//define if GLGE should prefix C functions (1: C functions will be prefixed, 0: C functions will not be prefixed)
#define GLGE_PREFIX_C_FUNCTIONS 0
//define the name of the prefix to use
#define GLGE_PREFIX glge_

/*

$$$$$$$$\ $$$$$$$\  $$$$$$$\   $$$$$$\  $$$$$$$\         $$$$$$\  $$$$$$$$\ $$$$$$$$\ $$$$$$$$\ $$$$$$\ $$\   $$\  $$$$$$\   $$$$$$\  
$$  _____|$$  __$$\ $$  __$$\ $$  __$$\ $$  __$$\       $$  __$$\ $$  _____|\__$$  __|\__$$  __|\_$$  _|$$$\  $$ |$$  __$$\ $$  __$$\ 
$$ |      $$ |  $$ |$$ |  $$ |$$ /  $$ |$$ |  $$ |      $$ /  \__|$$ |         $$ |      $$ |     $$ |  $$$$\ $$ |$$ /  \__|$$ /  \__|
$$$$$\    $$$$$$$  |$$$$$$$  |$$ |  $$ |$$$$$$$  |      \$$$$$$\  $$$$$\       $$ |      $$ |     $$ |  $$ $$\$$ |$$ |$$$$\ \$$$$$$\  
$$  __|   $$  __$$< $$  __$$< $$ |  $$ |$$  __$$<        \____$$\ $$  __|      $$ |      $$ |     $$ |  $$ \$$$$ |$$ |\_$$ | \____$$\ 
$$ |      $$ |  $$ |$$ |  $$ |$$ |  $$ |$$ |  $$ |      $$\   $$ |$$ |         $$ |      $$ |     $$ |  $$ |\$$$ |$$ |  $$ |$$\   $$ |
$$$$$$$$\ $$ |  $$ |$$ |  $$ | $$$$$$  |$$ |  $$ |      \$$$$$$  |$$$$$$$$\    $$ |      $$ |   $$$$$$\ $$ | \$$ |\$$$$$$  |\$$$$$$  |
\________|\__|  \__|\__|  \__| \______/ \__|  \__|       \______/ \________|   \__|      \__|   \______|\__|  \__| \______/  \______/ 

*/

//enable compile time errors (1 means compiler errors will appear, 0 means no compiler errors will appear)
#define GLGE_COMPILER_ERRORS 1

/*

$$\      $$\  $$$$$$\  $$$$$$$\  $$\   $$\ $$$$$$\ $$\   $$\  $$$$$$\         $$$$$$\  $$$$$$$$\ $$$$$$$$\ $$$$$$$$\ $$$$$$\ $$\   $$\  $$$$$$\   $$$$$$\  
$$ | $\  $$ |$$  __$$\ $$  __$$\ $$$\  $$ |\_$$  _|$$$\  $$ |$$  __$$\       $$  __$$\ $$  _____|\__$$  __|\__$$  __|\_$$  _|$$$\  $$ |$$  __$$\ $$  __$$\ 
$$ |$$$\ $$ |$$ /  $$ |$$ |  $$ |$$$$\ $$ |  $$ |  $$$$\ $$ |$$ /  \__|      $$ /  \__|$$ |         $$ |      $$ |     $$ |  $$$$\ $$ |$$ /  \__|$$ /  \__|
$$ $$ $$\$$ |$$$$$$$$ |$$$$$$$  |$$ $$\$$ |  $$ |  $$ $$\$$ |$$ |$$$$\       \$$$$$$\  $$$$$\       $$ |      $$ |     $$ |  $$ $$\$$ |$$ |$$$$\ \$$$$$$\  
$$$$  _$$$$ |$$  __$$ |$$  __$$< $$ \$$$$ |  $$ |  $$ \$$$$ |$$ |\_$$ |       \____$$\ $$  __|      $$ |      $$ |     $$ |  $$ \$$$$ |$$ |\_$$ | \____$$\ 
$$$  / \$$$ |$$ |  $$ |$$ |  $$ |$$ |\$$$ |  $$ |  $$ |\$$$ |$$ |  $$ |      $$\   $$ |$$ |         $$ |      $$ |     $$ |  $$ |\$$$ |$$ |  $$ |$$\   $$ |
$$  /   \$$ |$$ |  $$ |$$ |  $$ |$$ | \$$ |$$$$$$\ $$ | \$$ |\$$$$$$  |      \$$$$$$  |$$$$$$$$\    $$ |      $$ |   $$$$$$\ $$ | \$$ |\$$$$$$  |\$$$$$$  |
\__/     \__|\__|  \__|\__|  \__|\__|  \__|\______|\__|  \__| \______/        \______/ \________|   \__|      \__|   \______|\__|  \__| \______/  \______/ 

*/

//enable compile time warnings (1 means compiler warnings will appear, 0 means no compiler warnings will appear)
#define GLGE_COMPILER_WARNINGS 1

/*

$$$$$$$\  $$$$$$$$\ $$$$$$$\  $$\   $$\  $$$$$$\         $$$$$$\  $$$$$$$$\ $$$$$$$$\ $$$$$$$$\ $$$$$$\ $$\   $$\  $$$$$$\   $$$$$$\  
$$  __$$\ $$  _____|$$  __$$\ $$ |  $$ |$$  __$$\       $$  __$$\ $$  _____|\__$$  __|\__$$  __|\_$$  _|$$$\  $$ |$$  __$$\ $$  __$$\ 
$$ |  $$ |$$ |      $$ |  $$ |$$ |  $$ |$$ /  \__|      $$ /  \__|$$ |         $$ |      $$ |     $$ |  $$$$\ $$ |$$ /  \__|$$ /  \__|
$$ |  $$ |$$$$$\    $$$$$$$\ |$$ |  $$ |$$ |$$$$\       \$$$$$$\  $$$$$\       $$ |      $$ |     $$ |  $$ $$\$$ |$$ |$$$$\ \$$$$$$\  
$$ |  $$ |$$  __|   $$  __$$\ $$ |  $$ |$$ |\_$$ |       \____$$\ $$  __|      $$ |      $$ |     $$ |  $$ \$$$$ |$$ |\_$$ | \____$$\ 
$$ |  $$ |$$ |      $$ |  $$ |$$ |  $$ |$$ |  $$ |      $$\   $$ |$$ |         $$ |      $$ |     $$ |  $$ |\$$$ |$$ |  $$ |$$\   $$ |
$$$$$$$  |$$$$$$$$\ $$$$$$$  |\$$$$$$  |\$$$$$$  |      \$$$$$$  |$$$$$$$$\    $$ |      $$ |   $$$$$$\ $$ | \$$ |\$$$$$$  |\$$$$$$  |
\_______/ \________|\_______/  \______/  \______/        \______/ \________|   \__|      \__|   \______|\__|  \__| \______/  \______/ 

*/

//enable or disable all debug prints (1 means that debug stuff will be included, 0 means that no debug stuff will be included)
#define GLGE_ENABLE_DEBUGGING 1

/*

$$$$$$\ $$\   $$\  $$$$$$\  $$\      $$\   $$\ $$$$$$$\  $$$$$$\ $$\   $$\  $$$$$$\         $$$$$$\  $$$$$$$$\ $$$$$$$$\ $$$$$$$$\ $$$$$$\ $$\   $$\  $$$$$$\   $$$$$$\  
\_$$  _|$$$\  $$ |$$  __$$\ $$ |     $$ |  $$ |$$  __$$\ \_$$  _|$$$\  $$ |$$  __$$\       $$  __$$\ $$  _____|\__$$  __|\__$$  __|\_$$  _|$$$\  $$ |$$  __$$\ $$  __$$\ 
  $$ |  $$$$\ $$ |$$ /  \__|$$ |     $$ |  $$ |$$ |  $$ |  $$ |  $$$$\ $$ |$$ /  \__|      $$ /  \__|$$ |         $$ |      $$ |     $$ |  $$$$\ $$ |$$ /  \__|$$ /  \__|
  $$ |  $$ $$\$$ |$$ |      $$ |     $$ |  $$ |$$ |  $$ |  $$ |  $$ $$\$$ |$$ |$$$$\       \$$$$$$\  $$$$$\       $$ |      $$ |     $$ |  $$ $$\$$ |$$ |$$$$\ \$$$$$$\  
  $$ |  $$ \$$$$ |$$ |      $$ |     $$ |  $$ |$$ |  $$ |  $$ |  $$ \$$$$ |$$ |\_$$ |       \____$$\ $$  __|      $$ |      $$ |     $$ |  $$ \$$$$ |$$ |\_$$ | \____$$\ 
  $$ |  $$ |\$$$ |$$ |  $$\ $$ |     $$ |  $$ |$$ |  $$ |  $$ |  $$ |\$$$ |$$ |  $$ |      $$\   $$ |$$ |         $$ |      $$ |     $$ |  $$ |\$$$ |$$ |  $$ |$$\   $$ |
$$$$$$\ $$ | \$$ |\$$$$$$  |$$$$$$$$\\$$$$$$  |$$$$$$$  |$$$$$$\ $$ | \$$ |\$$$$$$  |      \$$$$$$  |$$$$$$$$\    $$ |      $$ |   $$$$$$\ $$ | \$$ |\$$$$$$  |\$$$$$$  |
\______|\__|  \__| \______/ \________|\______/ \_______/ \______|\__|  \__| \______/        \______/ \________|   \__|      \__|   \______|\__|  \__| \______/  \______/ 

*/

//specify if the math library should be included (1: the library will be included, 0: the library will not be included)
#define GLGE_INCLUDE_MATH 1

//specify if the graphic library should be included (1: the library will be included, 0: the library will not be included)
#define GLGE_INCLUDE_GRAPHICS 1

/*

$$\       $$$$$$\ $$\      $$\ $$$$$$\ $$$$$$$$\ $$$$$$$$\ $$$$$$$\         $$$$$$\  $$$$$$$$\ $$$$$$$$\ $$$$$$$$\ $$$$$$\ $$\   $$\  $$$$$$\   $$$$$$\  
$$ |      \_$$  _|$$$\    $$$ |\_$$  _|\__$$  __|$$  _____|$$  __$$\       $$  __$$\ $$  _____|\__$$  __|\__$$  __|\_$$  _|$$$\  $$ |$$  __$$\ $$  __$$\ 
$$ |        $$ |  $$$$\  $$$$ |  $$ |     $$ |   $$ |      $$ |  $$ |      $$ /  \__|$$ |         $$ |      $$ |     $$ |  $$$$\ $$ |$$ /  \__|$$ /  \__|
$$ |        $$ |  $$\$$\$$ $$ |  $$ |     $$ |   $$$$$\    $$$$$$$  |      \$$$$$$\  $$$$$\       $$ |      $$ |     $$ |  $$ $$\$$ |$$ |$$$$\ \$$$$$$\  
$$ |        $$ |  $$ \$$$  $$ |  $$ |     $$ |   $$  __|   $$  __$$<        \____$$\ $$  __|      $$ |      $$ |     $$ |  $$ \$$$$ |$$ |\_$$ | \____$$\ 
$$ |        $$ |  $$ |\$  /$$ |  $$ |     $$ |   $$ |      $$ |  $$ |      $$\   $$ |$$ |         $$ |      $$ |     $$ |  $$ |\$$$ |$$ |  $$ |$$\   $$ |
$$$$$$$$\ $$$$$$\ $$ | \_/ $$ |$$$$$$\    $$ |   $$$$$$$$\ $$ |  $$ |      \$$$$$$  |$$$$$$$$\    $$ |      $$ |   $$$$$$\ $$ | \$$ |\$$$$$$  |\$$$$$$  |
\________|\______|\__|     \__|\______|   \__|   \________|\__|  \__|       \______/ \________|   \__|      \__|   \______|\__|  \__| \______/  \______/ 

*/

//specify the amount of samples used by a limiter for smoothing
#define GLGE_LIMITER_IPS_SAMPLES 32

#endif