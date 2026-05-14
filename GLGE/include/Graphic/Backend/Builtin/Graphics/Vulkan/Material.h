/**
 * @file Material.h
 * @author DM8AT
 * @brief define the material backend overload for vulkan
 * @version 0.1
 * @date 2026-05-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_MATERIAL_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_MATERIAL_

//add the material class
#include "Graphic/Backend/Graphics/Material.h"
//add vulkan instances
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::Vulkan {

    /**
     * @brief overload the material class for OpenGL
     */
    class Material : public GLGE::Graphic::Backend::Graphic::Material {
    public:

        /**
         * @brief Construct a new Material
         * 
         * @param shader a reference to the used shader backend
         * @param layout a reference to the used vertex layout
         * @param fbuff a reference to the framebuffer the material will use
         * @param cullMode the mode for the backface culling
         * @param depthMode the mode to use for depth compares
         * @param depthWrite `true` to enable depth writing, `false` to disable depth writing
         */
        Material(Reference<GLGE::Graphic::Backend::Graphic::Shader> shader, Reference<GLGE::Graphic::Backend::Graphic::VertexLayout> layout, Reference<GLGE::Graphic::Backend::Graphic::Framebuffer> fbuff, CullMode cullMode, DepthMode depthMode, bool depthWrite);

        /**
         * @brief Destroy the Material
         */
        virtual ~Material();

        /**
         * @brief bind the material
         * 
         * @param buffer the buffer to bind to
         * @param VBOOffset the VBO offset from 0 in bytes
         */
        virtual void bind(GLGE::Graphic::Backend::Graphic::CommandBuffer* buffer, size_t VBOOffset) override;

        /**
         * @brief a function to re-bind the material to update the VBO offset
         * 
         * @param buffer the buffer to bind to
         * @param VBOOffset the VBO offset from 0 in bytes
         */
        virtual void rebindMesh(GLGE::Graphic::Backend::Graphic::CommandBuffer* buffer, size_t VBOOffset) override;

    protected:

        /**
         * @brief store the vulkan instance
         */
        GLGE::Graphic::Backend::Graphic::Vulkan::Instance* m_instance = nullptr;
        /**
         * @brief store the graphic pipeline
         */
        void* m_pipe = nullptr;
        /**
         * @brief store the pipeline layout
         */
        void* m_pipeLayout = nullptr;

    };

}

#endif