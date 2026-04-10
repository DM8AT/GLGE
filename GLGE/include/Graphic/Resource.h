/**
 * @file Resource.h
 * @author DM8AT
 * @brief define the base class for all resources
 * @version 0.1
 * @date 2026-03-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_RESOURCE_
#define _GLGE_GRAPHIC_RESOURCE_

//add common stuff
#include "Core/Common.h"

//use the library namespace
namespace GLGE::Graphic {

    //resource sets are defined later
    class ResourceSet;

    /**
     * @brief define general and abstract resource types
     */
    enum ResourceType : u8 {
        /**
         * @brief a wild card type, will accept anything
         */
        UNDEFINED = 0,
        /**
         * @brief the resource is a simple image
         */
        IMAGE,
        /**
         * @brief the resource is a sampled texture
         */
        SAMPLED_TEXTURE,
        /**
         * @brief the resource is a storage buffer
         */
        STORAGE_BUFFER,
        /**
         * @brief the resource is a uniform buffer
         */
        UNIFORM_BUFFER,

        /**
         * @brief spacer for a custom type
         * 
         * @warning This value is preserved for a general error
         * 
         * To add a custom type, use `CUSTOM + n`, where `n` is a custom increment
         */
        CUSTOM = 0xf
    };

    /**
     * @brief a base class that all resources must inherit from
     */
    class Resource {
    public:

        /**
         * @brief Construct a new Resource
         */
        Resource() = default;

        /**
         * @brief Destroy the Resource
         */
        virtual ~Resource() = default;
        
        /**
         * @brief a function that is called to build a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to bind to
         * @param unit the unit to bind to
         */
        virtual void onBuildBinding(ResourceSet* set, u32 unit) = 0;

        /**
         * @brief Get the type of resource this is
         * 
         * @return `ResourceType` the type to identify the resource
         */
        virtual ResourceType getType() const noexcept = 0;

        /**
         * @brief Get the hash of the actual resource type
         * 
         * @return `u64` the hash of the type
         */
        virtual u64 getTypeHash() const noexcept = 0;
        
    };

}

#endif