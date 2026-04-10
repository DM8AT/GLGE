/**
 * @file ResourceSet.h
 * @author DM8AT
 * @brief define an API for the resource set
 * @version 0.1
 * @date 2026-03-15
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHICS_RESOURCE_SET_
#define _GLGE_GRAPHIC_BACKEND_GRAPHICS_RESOURCE_SET_

//add command buffers
#include "CommandBuffer.h"
//add references
#include "Core/Reference.h"

//resource sets are defined somewhere else
namespace GLGE::Graphic 
{class ResourceSet;}

//use the backend namespace
namespace GLGE::Graphic::Backend::Graphic {

    /**
     * @brief define the backend API for resource sets
     */
    class ResourceSet : public Referable {
    public:

        /**
         * @brief Construct a new Resource Set
         * 
         * @param set a pointer to the frontend resource set
         */
        ResourceSet(GLGE::Graphic::ResourceSet* set)
         : m_resourceSet(set)
        {}

        /**
         * @brief Destroy the Resource Set
         */
        virtual ~ResourceSet() = default;

        /**
         * @brief bind the resource set to a command buffer
         * 
         * @param buffer a pointer to the command buffer to bind the set to
         */
        virtual void bind(CommandBuffer* buffer) = 0;

    protected:

        /**
         * @brief store a pointer to the frontend resource set
         */
        GLGE::Graphic::ResourceSet* m_resourceSet = nullptr;

    };

}

#endif