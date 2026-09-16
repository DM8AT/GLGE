/**
 * @file LayerManager.cpp
 * @author DM8AT
 * @brief implement the layer manager
 * @version 0.1
 * @date 2026-05-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add layer managers
#include "Core/LayerManager.h"

void GLGE::LayerManager::activateLayer(UUID layer, bool unloadOld) {
    //if the layer does not exist, stop
    if (!isKnown(layer)) 
    {throw GLGE::Exception("Failed to find the requested layer using the provided UUID", "GLGE::LayerManager::activateLayer");}

    //check if a layer is currently active
    if (m_hasActiveLayer) {
        //if yes, detach the old layer
        m_layers[m_activeLayer]->onDetach();

        //if the layer should be unloaded, erase it
        if (unloadOld)
        {m_layers.erase(m_activeLayer);}
    }

    //store the new active layer
    m_activeLayer = layer;
    m_hasActiveLayer = true;

    //call the layer that it is now active
    m_layers[m_activeLayer]->onAttach();
}

void GLGE::LayerManager::update() {
    //only tick if an active layer exists
    if (m_hasActiveLayer)
    {m_layers[m_activeLayer]->onUpdate();}
}