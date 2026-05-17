/**
 * @file Examples.h
 * @author DM8AT
 * @brief gather all examples
 * @version 0.1
 * @date 2026-05-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _EXAMPLES_
#define _EXAMPLES_

//add all examples
#include "DefaultExample.h"

//store a list of mappings of the example launchers to their names
inline static std::vector<std::pair<const char*, PFN_ExampleFunc>> EXAMPLES = {
    {std::pair("Default Example - This is just an all-around example that does not test a specific system in-depth.", &defaultExample)}
};

#endif