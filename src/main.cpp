/**
 * @file main.cpp
 * @author DM8AT
 * @brief define the main selector for the examples
 * @version 0.1
 * @date 2026-04-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add all examples
#include "OpenGL/gl_example1.h"
#include "Vulkan/vk_example1.h"

//add default I/O
#include <iostream>

//store a name map
const char* nameMap[] = {
    "OpenGL example 1",
    "Vulkan example 1"
};

//store function pointers to the entry points
void (*functionMap[])() = {
    &gl_example1,
    &vk_example1
};

int main() {
    //print the information entry table
    std::cout << "Loaded examples:\n";
    for (size_t i = 0; i < (sizeof(nameMap)/sizeof(*nameMap)); ++i) {
        std::cout << "    " << i << " : " << nameMap[i] << "\n";
    }
    //ask for an example to select
    std::cout << "\nPlease enter the number of the example to run:\n";
    uint32_t example = 1;
    std::cin >> example;
    //sanity check
    if (example > (sizeof(functionMap)/sizeof(*functionMap))) {
        std::cout << "[ERROR] The input " << example << " is too high. The amount of table entries is " << 
                     (sizeof(functionMap)/sizeof(*functionMap)) << ", please input a number less than that.\n";
        return 1;
    }

    //run the code
    (*functionMap[example])();

    //sane exit
    return 0;
}