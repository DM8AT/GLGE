/**
 * @file GLGEInstance.cpp
 * @author DM8AT
 * @brief implement the GLGE Instance and related classes and functions
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the instance
#include "GLGEInstance.h"

/**
 * @brief this is a function for the update thread of an instance
 * 
 * @param instance a reference to the instance
 */
void instance_update_thread(Instance* instance)
{
    //loop while the instance is active
    while (instance->isActive())
    {
        //start the limiter
        instance->updateLimiter().startTick();
        //iterate over all elements attatched to the instance
        for (size_t i = 0; i < instance->getAttatchedElements().size();)
        {
            //update the element
            if (instance->getAttatchedElements()[i]->onUpdate())
            {
                //if the element still exists, increment the count
                ++i;
            }
        }
        //end the limiter
        instance->updateLimiter().endTick();
    }
}

Instance::Instance(std::string_view name, APIs api) : m_name(name), m_api(api) 
{
    //create the instance update thread
    m_updateThread = std::thread(instance_update_thread, this);
}

Instance::~Instance() noexcept
{
    //say that the instance is no longer active
    m_active = false;
    //join the update thread
    m_updateThread.join();
    //iterate over all attatched elements
    while (m_elements.size() > 0)
    {
        //destroy the element
        delete m_elements[0];
        //erase the element
        m_elements.erase(m_elements.begin());
    }
    //check if a logger exists
    if (m_logger)
    {
        //print the contents of the logger
        m_logger->printAll();
        //delete the logger
        delete m_logger;
        //set the logger to 0
        m_logger = 0;
    }
}


void Instance::addElement(InstAttachableClass* element) noexcept
{
    //print a debug info
    GLGE_DEBUG_WRAPPER(
        std::stringstream stream;
        stream << "Adding instance of a class that inherits from InstAttachableClass to instance named \"" << m_name << "\": " << *element;
        logDebug(stream, MESSAGE_TYPE_DEBUG);
    )
    //add the instance
    m_elements.emplace_back(element);
}

void Instance::removeElement(InstAttachableClass* element) noexcept
{
    //print a debug info
    GLGE_DEBUG_WRAPPER(
        std::stringstream stream;
        stream << "Removing instance of class that inherits from InstAttachableClass from instance named \"" << m_name << "\": " << *element;
        logDebug(stream, MESSAGE_TYPE_DEBUG);
    )
    //search the pointer
    auto pos = std::find(m_elements.begin(), m_elements.end(), element);
    //check if the element does not exist
    if (pos == m_elements.end())
    {
        //print an error and return
        log("Failed to find the requested element", MESSAGE_TYPE_ERROR);
        return;
    }
    //erase the element
    m_elements.erase(pos);
}