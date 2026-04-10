/**
 * @file BaseClass.cpp
 * @author DM8AT
 * @brief implement the base class
 * @version 0.1
 * @date 2025-12-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//add base classes
#include "Core/BaseClass.h"
//add exceptions
#include "Core/Exception.h"

//use the library namespace
using namespace GLGE;

BaseClass::BaseClass() {
    //in debug add a sanity check
    #if GLGE_DEBUG

    //if no instance exists, throw an error
    if (Instance::getCurrentInstance() == nullptr)
    {throw Exception("Error: No instance bound to the current thread", "GLGE::Core::BaseClass");}

    #endif

    //store the currently bound instance
    m_instance = Instance::getCurrentInstance();
}