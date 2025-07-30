/**
 * @file GLGEGraphicAPI.cpp
 * @author DM8AT
 * @brief implement the base functionality for the graphic API instance classes
 * @version 0.1
 * @date 2025-04-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the base graphic API
#include "GLGEGraphicAPI_Instance.h"
//include the shared graphic data
#include "../GLGEGraphicShared.h"

void GraphicInstance::addElement(GraphicInstAttatchable* element) noexcept
{
    //print a debug info
    GLGE_DEBUG_WRAPPER(
        std::stringstream stream;
        stream << "Adding instance of a class that inherits from InstAttachableClass to graphic instance instance named: " << *element;
        m_instance->logDebug(stream, MESSAGE_TYPE_DEBUG);
    )
    //add the instance
    m_elements.emplace_back(element);
}

void GraphicInstance::removeElement(GraphicInstAttatchable* element) noexcept
{
    //print a debug info
    GLGE_DEBUG_WRAPPER(
        std::stringstream stream;
        stream << "Removing instance of class that inherits from InstAttachableClass from graphic instance: " << *element;
        m_instance->logDebug(stream, MESSAGE_TYPE_DEBUG);
    )
    //search the pointer
    auto pos = std::find(m_elements.begin(), m_elements.end(), element);
    //check if the element does not exist
    if (pos == m_elements.end())
    {
        //print an error and return
        m_instance->log("Failed to find the requested element in graphic instance", MESSAGE_TYPE_ERROR);
        return;
    }
    //erase the element
    m_elements.erase(pos);
}

bool GraphicInstance::onUpdate() noexcept
{
    //iterate over all elements
    for (size_t i = 0; i < m_elements.size();)
    {
        //check if the object still exists after the update
        if (m_elements[i]->onUpdate())
        {
            //go to the next element
            ++i;
        }
    }

    //always return true
    return true;
}

GraphicInstance::GraphicInstance(Instance* instance) : InstAttachableClass(instance, "GraphicInstance") 
{
    //add the instance
    __glge_all_instances->push_back(m_instance);
    //call the create hook
    onCreate();
}

void GraphicInstance::destroy()
{
    //call the destruction hook
    onDestroy();
    //delete all attached elements
    while (m_elements.size() > 0)
    {
        delete m_elements[0];
    }
    //remove the instance
    auto pos = std::find(__glge_all_instances->begin(), __glge_all_instances->end(), m_instance);
    //erase the element if it exists
    if (pos != __glge_all_instances->end()) {__glge_all_instances->erase(pos);}
}