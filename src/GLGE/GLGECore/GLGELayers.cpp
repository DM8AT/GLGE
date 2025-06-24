/**
 * @file GLGELayers.cpp
 * @author DM8AT
 * @brief implement the functionality for layers and layer stacks
 * @version 0.1
 * @date 2025-06-22
 * 
 * @copyright Copyright (c) 2025
 */

//include layer related stuff
#include "GLGELayers.h"

LayerStack::~LayerStack() noexcept
{
    //delete all layers
    for (uint64_t i = 0; i < m_layers.size(); ++i) {delete m_layers[i];}
    //clear the layer stack
    m_layers.clear();
}

bool LayerStack::sendEvent(Event* event) noexcept
{
    //store if the event was handled successfully
    bool handled = false;
    //loop over all layers the stack owns
    for (uint64_t i = 0; i < m_layers.size(); ++i)
    {
        //handle the event by the layer and check if it was handled
        if (m_layers[i]->onEvent(event))
        {
            //store that the handeling was successfull
            handled = true;
            break;
        }
    }

    //delete the event
    delete event;

    //return if the event was handled
    return handled;
}