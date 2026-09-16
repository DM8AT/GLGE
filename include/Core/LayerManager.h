/**
 * @file LayerManager.h
 * @author DM8AT
 * @brief define a class that is responsible for managing layers
 * @version 0.1
 * @date 2026-05-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_CORE_LAYER_MANAGER_
#define _GLGE_CORE_LAYER_MANAGER_

//add instances
#include "Instance.h"
#include "BaseClass.h"
//add layers
#include "Layer.h"

//use the library namespace
namespace GLGE {

    /**
     * @brief layers are defined somewhere else
     */
    class Layer;

    /**
     * @brief define a class that is responsible for managing layers
     */
    class LayerManager final : public Referable, public BaseClass {
    public:

        /**
         * @brief Construct a new Layer Manager
         */
        LayerManager() = default;

        /**
         * @brief Destroy the Layer Manager
         */
        ~LayerManager() = default;

        //layer managers are immovable
        LayerManager(LayerManager&&) = delete;
        LayerManager(const LayerManager&) = delete;
        LayerManager& operator=(LayerManager&&) = delete;
        LayerManager& operator=(const LayerManager&) = delete;

        /**
         * @brief create a new layer
         * 
         * @tparam T the type of layer to create
         * @tparam Args the argument types to forward to the layer
         * @param args the arguments to forward to the layer
         * 
         * @return `UUID` the universal unique identifier of the layer
         */
        template <typename T, typename... Args> 
        requires (std::is_base_of_v<Layer, T>)
        UUID createLayer(Args&&... args) {
            //generate a UUID
            UUID id = m_uuidReg.generate();
            //create the layer
            m_layers[id] = Reference<Layer>(new T(std::forward<Args>(args)..., id, Reference<LayerManager>(this)));
            //return the UUID
            return id;
        }

        /**
         * @brief remove a layer
         * 
         * @param uuid the UUID of the layer to remove
         */
        void removeLayer(UUID uuid)
        {m_layers.erase(uuid);}

        /**
         * @brief check if a uuid is known
         * 
         * @param uuid the uuid to check
         * @return `true` if the uuid is known, `false` if unknown
         */
        inline bool isKnown(UUID uuid) const
        {return m_layers.contains(uuid);}

        /**
         * @brief Get a specific layer
         * 
         * If the UUID is invalid this function will throw
         * 
         * @param uuid the universal unique identifier
         * @return `Layer&` a reference to the layer
         */
        inline Layer& getLayer(UUID uuid)
        {return *(m_layers.at(uuid));}

        /**
         * @brief Get a specific layer
         * 
         * If the UUID is invalid this function will throw
         * 
         * @param uuid the universal unique identifier
         * @return `const Layer&` a constant reference to the layer
         */
        inline const Layer& getLayer(UUID uuid) const
        {return *(m_layers.at(uuid));}

        /**
         * @brief access a specific layer
         * 
         * If the UUID is invalid this function will throw
         * 
         * @param uuid the UUID of the layer to access
         * @return `Layer&` a reference to the layer
         */
        inline Layer& operator[](UUID uuid) 
        {return getLayer(uuid);}

        /**
         * @brief access a specific layer
         * 
         * If the UUID is invalid this function will throw
         * 
         * @param uuid the UUID of the layer to access
         * @return `const Layer&` a constant reference to the layer
         */
        inline const Layer& operator[](UUID uuid) const
        {return getLayer(uuid);}

        /**
         * @brief check if an active layer exists
         * 
         * @return `true` if a layer is currently active, `false` if not
         */
        inline bool hasActiveLayer() const noexcept
        {return m_hasActiveLayer;}

        /**
         * @brief get the uuid of the currently active layer
         * 
         * @return `UUID` the UUID of the currently active layer
         */
        inline UUID activeLayer() const noexcept
        {return m_activeLayer;}

        /**
         * @brief activate a specific layer
         * 
         * @param layer the UUID of the layer
         * @param unloadOld `true` if the old layer should be unloaded, `false` to keep the old layer loaded
         */
        void activateLayer(UUID layer, bool unloadOld = true);

        /**
         * @brief tick the manager
         */
        void update();

    private:

        /**
         * @brief store the UUID registry for layers
         */
        UUIDRegistry m_uuidReg;

        /**
         * @brief store the UUID of the current layer
         */
        UUID m_activeLayer = 0;
        /**
         * @brief store if a layer exists
         */
        bool m_hasActiveLayer = false;

        /**
         * @brief store all the layers
         */
        std::unordered_map<UUID, Reference<Layer>> m_layers;

    };

}

#endif