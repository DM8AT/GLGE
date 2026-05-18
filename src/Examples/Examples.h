/**
 * @file Examples.h
 * @author DM8AT
 * @brief Example launcher registry (populated from plugins and/or static registration).
 * @version 0.1
 * @date 2026-05-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef _EXAMPLES_
#define _EXAMPLES_

//add the example contract system
#include "ExamplePluginContract.h"

#include <utility>
#include <vector>

/**
 * @brief store all examples
 */
extern std::vector<std::pair<const char*, PFN_ExampleFunc>> EXAMPLES;

#endif