/**
 * @file GLGELayers.h
 * @author DM8AT
 * @brief define event layers for GLGE
 * @version 0.1
 * @date 2025-06-21
 * 
 * @copyright Copyright (c) 2025
 */

//header guard
#ifndef _GLGE_LAYERS_
#define _GLGE_LAYERS_

//include events
#include "GLGEEvent.h"

//check for C++
#if GLGE_CPP

//layers will be defined later
class Layer;

/**
 * @brief a class to manage multiple layers that all can recieve and handle events
 */
class LayerStack
{
public:

    /**
     * @brief Construct a new Layer Stack
     */
    LayerStack() = default;

    /**
     * @brief Construct a new Layer Stack
     * @warning the layer stack will own the layers
     * 
     * @param layers the layers the layer stack starts with
     */
    LayerStack(std::vector<Layer*> layers) : m_layers(layers) {}

    /**
     * @brief Destroy the Layer Stack
     * @warning this will delete all layers owned by the stack
     */
    ~LayerStack() noexcept;

    /**
     * @brief add a new layer to the stack
     * @warning the layer will be owned by the layer stack
     * 
     * @param layer a pointer to the layer to add
     */
    inline void addLayer(Layer* layer) {m_layers.push_back(layer);}

    /**
     * @brief send an event through the layer stack untill it is handled or reaches the bottom
     * 
     * @param event a pointer to the event to send. Sadly, this must be a pointer since event is a overloadable base class
     * @warning the pointer to the event will be deleted afterwards
     * 
     * @return true : the event was handled
     * @return false : the event was not handled
     */
    bool sendEvent(Event* event) noexcept;

protected:

    /**
     * @brief store a list of all layers on the stack
     */
    std::vector<Layer*> m_layers;

};

/**
 * @brief an abstract class to create a single layer to handle events
 */
class Layer
{
public:

    /**
     * @brief Construct a new Layer
     */
    Layer() = default;

    /**
     * @brief Destroy the Layer
     */
    virtual ~Layer() {}

    /**
     * @brief the function is called when an event is passed to the layer
     * 
     * @param event a pointer to the event that is being passed through the layer stack
     * 
     * @return true : the event was handled. This means the event will not being passed down further the layer stack
     * @return false : the event was not handled by this layer
     */
    virtual bool onEvent(Event*) {return false;}

};

#endif //end of C++ section

#endif