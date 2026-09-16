/**
 * @file TestLauncher.h
 * @author DM8AT
 * @brief define the test launcher function
 * @version 0.1
 * @date 2026-05-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#pragma once

//add i/o
#include <iostream>
//add exceptions
#include <exception>
//add stringstreams
#include <sstream>
//add vectors
#include <vector>

/**
 * @brief define the function layout
 * 
 * @return `int` the return value
 */
int launchUnitTests();

/**
 * @brief a helper function to read a size_t from the console
 * 
 * @return `size_t` the read value
 */
static size_t readSizeT() {
    //print user input prefix
    std::cout << "Input : ";
    //get the whole line
    std::string line;
    //loop until something is inputted
    while (line.empty()) {
        if (!std::getline(std::cin, line))
        {throw std::runtime_error("The input stream closed");}
    }

    //convert the line to an input string stream
    std::istringstream iss(line);

    //check if the initial value is a number
    long long value;
    if (!(iss >> value))  {
        //throw an error
        throw std::invalid_argument("Please input only numbers");
    }

    //check if the value is negative (invalid size_t)
    if (value < 0) {
        //input state is still valid
        throw std::invalid_argument("Please input only positive numbers");
    }

    //check for too many input items
    std::string extra;
    if (iss >> extra) {
        //throw an error
        throw std::invalid_argument("Please input only a single number");
    }

    //return the valid number
    return size_t(value);
}

/**
 * @brief a helper function to stringify a list of elements in a structured way
 * 
 * @tparam T ignored
 * @param list the list to print
 * @param align the size of the begin block, padded with spaces
 * @return `std::string` the string containing the map
 */
template <typename T> 
static std::string structureMapToString(const std::vector<std::pair<const char*, T>>& list, size_t align = 6) {
    //create the initial stream
    std::stringstream stream;
    //keep track of the current element ID
    size_t id = 0;
    //iterate over all elements
    for (const auto& [name, _] : list) {
        //create the beginning section of the element
        std::string begin("[");
        begin += std::to_string(id);
        begin += "]";
        begin.resize(align, ' ');
        stream << begin << " " << name << "\n";
        ++id;
    }
    //return the final stream
    return stream.str();
}

/**
 * @brief a helper function to stringify a list of elements in a structured way
 * 
 * @param list the list to print
 * @param align the size of the begin block, padded with spaces
 * @return `std::string` the string containing the map
 */
static std::string structureMapToString(const std::vector<std::string>& list, size_t align = 6) {
    //create the initial stream
    std::stringstream stream;
    //keep track of the current element ID
    size_t id = 0;
    //iterate over all elements
    for (const auto& name : list) {
        //create the beginning section of the element
        std::string begin("[");
        begin += std::to_string(id);
        begin += "]";
        begin.resize(align, ' ');
        stream << begin << " " << name << "\n";
        ++id;
    }
    //return the final stream
    return stream.str();
}