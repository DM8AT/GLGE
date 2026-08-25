/**
 * @file Material.h
 * @author DM8AT
 * @brief overload the material class for OpenGL
 * @version 0.1
 * @date 2026-03-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_MATERIAL_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_MATERIAL_

//add the material class
#include "Graphic/Backend/Graphics/Material.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::OpenGL {

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
         * @param fbuff a pointer to the framebuffer the material will use
         * @param cullMode the mode for the backface culling
         * @param depthMode the mode to use for depth compares
         * @param depthWrite `true` to enable depth writing, `false` to disable depth writing
         */
        Material(GLGE::Reference<GLGE::Graphic::Backend::Graphic::Shader> shader, GLGE::Graphic::VertexLayout* layout, Reference<GLGE::Graphic::Backend::Graphic::Framebuffer> fbuff, Graphic::Material::CullMode cullMode, Graphic::Material::DepthMode depthMode, bool depthWrite);

        /**
         * @brief Destroy the Material
         */
        virtual ~Material() = default;

        /**
         * @brief bind the material
         * 
         * @param buffer the buffer to bind to
         */
        virtual void bind(GLGE::Graphic::Backend::Graphic::CommandBuffer* buffer) override;

        /**
         * @brief Get the VAO of the material
         * 
         * @return `u32` the VAO object of the material
         */
        inline u32 getVao() const noexcept
        {return m_vao;}

    protected:

        /**
         * @brief a function to build / re-build the internal VAO
         */
        void updateVAO();

        /**
         * @brief store the VAO of the material
         */
        u32 m_vao = 0;

    };

}

#endif