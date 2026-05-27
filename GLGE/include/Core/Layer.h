/**
 * @file Layer.h
 * @author DM8AT
 * @brief define an abstract layer system
 * @version 0.1
 * @date 2026-05-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_CORE_LAYER_
#define _GLGE_CORE_LAYER_

//add common stuff
#include "Common.h"
//add reference counting
#include "Reference.h"
//add unique ids
#include "UUID.h"

//use the library namespace
namespace GLGE {

    /**
     * @brief layer managers must be defined later
     */
    class LayerManager;

    /**
     * @brief define a simple layer abstraction
     * 
     * A layer is similar to a state machine. A single layer can be active at the current point in time (application state) from which switching to different layers is possible. 
     * Transitions can be of different nature, they can either unload the layer (state is voided unless saved) or keep it loaded (state is preserved regardless of storing). 
     * The nature of the transition is defined by the layer that invoked the transition. 
     * 
     * A layer manages its own state. Layers are identified through unique ids. 
     */
    class Layer : public Referable {
    public:

        /**
         * @brief Construct a new Layer
         * 
         * @param uuid the unique ID of the layer
         * @param manager a reference to the layer manager that manages this layer
         */
        Layer(UUID uuid, GLGE::Reference<LayerManager>&& manager)
         : m_uuid(uuid), m_manager(manager)
        {}

        /**
         * @brief Destroy the Layer
         */
        virtual ~Layer() = default;

        //layers are immovable
        Layer(Layer&&) = delete;
        Layer(const Layer&) = delete;
        Layer& operator=(Layer&&) = delete;
        Layer& operator=(const Layer&) = delete;

        /**
         * @brief a function that is run if the layer is attached
         */
        virtual void onAttach() = 0;

        /**
         * @brief a function that is called each update while the layer is bound
         */
        virtual void onUpdate() = 0;

        /**
         * @brief a function that is run if the layer is detached
         */
        virtual void onDetach() = 0;
        
    private:

        /**
         * @brief store the own uuid
         */
        UUID m_uuid;
        /**
         * @brief store a reference to the layer manager
         */
        GLGE::Reference<LayerManager> m_manager;

    };

}

#endif