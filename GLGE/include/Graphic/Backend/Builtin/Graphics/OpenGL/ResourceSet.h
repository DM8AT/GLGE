/**
 * @file ResourceSet.h
 * @author DM8AT
 * @brief define the overload of a resource set for OpenGL
 * @version 0.1
 * @date 2026-03-15
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_RESOURCE_SET_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_RESOURCE_SET_

//add the resource set API
#include "Graphic/Backend/Graphics/ResourceSet.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::OpenGL {

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
         * @brief bind the resource set to a command buffer
         * 
         * @param buffer a pointer to the command buffer to bind the set to
         */
        void bind(CommandBuffer* buffer) override;

        /**
         * @brief register a binder function for a resource
         * 
         * @param func a function pointer to use for binding
         * @param resource a pointer to the resource backend to bind
         * @param unit the unit to register the resource on
         */
        void registerBinder(void (*func)(u32, void*), void* resource, u32 unit)
        {m_binders.emplace_back(func, std::pair{resource, unit});}

    protected:

        /**
         * @brief store a list of functions used to bind the resources
         */
        std::vector<std::pair<void (*)(u32, void*), std::pair<void*, u32>>> m_binders;

    };

}

#endif