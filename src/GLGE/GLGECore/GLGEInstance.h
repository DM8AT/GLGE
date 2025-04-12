/**
 * @file GLGEInstance.h
 * @author DM8AT
 * @brief define the layout of an GLGE instance
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_INSTANCE_
#define _GLGE_INSTANCE_

//include the settings
#include "GLGESettings.h"

//check if this is C++
#if GLGE_CPP

/**
 * @brief instances will be defined later
 */
class Instance;

/**
 * @brief a simple virtual class to add objects to a GLGE instance
 */
class InstAttachableClass
{
public:

    /**
     * @brief Construct a new instance
     */
    InstAttachableClass() = default;

    /**
     * @brief Construct a new instance
     * 
     * @param instance a pointer to the GLGE instance the new instance will belong to
     */
    InstAttachableClass(Instance* instance) : m_instance(instance) {}

    /**
     * @brief Destroy the instance
     */
    virtual ~InstAttachableClass() {}

protected:

    /**
     * @brief destroy the object
     */
    virtual void onDestroy() {}

    /**
     * @brief attach the object to an instance
     */
    virtual void onAttach() {}

    /**
     * @brief store the instance the class belongs to
     */
    Instance* m_instance = 0;

};

/**
 * @brief a instance is a class that owns all other GLGE classes that need an instance in construction
 */
class Instance
{
public:

    /**
     * @brief Construct a new Instance
     */
    Instance() = default;



private:

    

};

#endif

#endif