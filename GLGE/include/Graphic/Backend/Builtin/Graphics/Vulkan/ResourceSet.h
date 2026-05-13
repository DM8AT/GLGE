/**
 * @file ResourceSet.h
 * @author DM8AT
 * @brief define the vulkan backend implementation of a resource set
 * @version 0.1
 * @date 2026-05-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_RESOURCE_SET_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_RESOURCE_SET_

//add the resource set API
#include "Graphic/Backend/Graphics/ResourceSet.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::Vulkan {

    /**
     * @brief define the graphic backend API for a set of resources
     */
    class ResourceSet : public GLGE::Graphic::Backend::Graphic::ResourceSet {
    public:

        /**
         * @brief Construct a new resource set
         * 
         * @param set a pointer to the frontend set to initialize from
         */
        ResourceSet(GLGE::Graphic::ResourceSet* set);

        /**
         * @brief Destroy the Resource Set
         */
        ~ResourceSet();

        /**
         * @brief Get the Descriptor Set
         * 
         * @return `void*` the internal vulkan descriptor set
         */
        inline void* getDescriptorSet() const noexcept
        {return m_set;}

        /**
         * @brief Get the Descriptor Pool
         * 
         * @return `void*` the internal vulkan descriptor pool
         */
        inline void* getDescriptorPool() const noexcept
        {return m_pool;}

        /**
         * @brief Get the Descriptor Layout
         * 
         * @return `void*` the internal vulkan descriptor layout
         */
        inline void* getDescriptorLayout() const noexcept
        {return m_layout;}

        /**
         * @brief bind the resource set to a command buffer
         * 
         * @param buffer a pointer to the command buffer to bind the set to
         * 
         * @warning Unused for vulkan. All commands shall bind specifically for their needs. 
         */
        virtual void bind(GLGE::Graphic::Backend::Graphic::CommandBuffer* buffer) override {}

    protected:

        /**
         * @brief store the descriptor set of this resource set
         */
        void* m_set = nullptr;
        /**
         * @brief store the descriptor pool of this resource set
         */
        void* m_pool = nullptr;
        /**
         * @brief store the descriptor layout of this resource set
         */
        void* m_layout = nullptr;

    };

}

#endif