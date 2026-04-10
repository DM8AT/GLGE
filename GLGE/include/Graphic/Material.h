/**
 * @file Material.h
 * @author DM8AT
 * @brief define a material frontend wrapper
 * @version 0.1
 * @date 2026-03-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_MATERIAL_
#define _GLGE_GRAPHIC_MATERIAL_

//add references
#include "Core/Reference.h"
//add the material backend
#include "Backend/Graphics/Material.h"
//add resources
#include "Resource.h"
//add instances
#include "Core/Instance.h"

//add shader and vertex layout frontends
#include "Shader.h"
#include "VertexLayout.h"

//add graphic instances
#include "Instance.h"

//use the libraries namespace
namespace GLGE::Graphic {

    /**
     * @brief a material stores all information required to draw a mesh
     */
    class Material : public BaseClass {
    public:

        /**
         * @brief use the depth mode of the backend
         */
        using DepthMode = GLGE::Graphic::Backend::Graphic::Material::DepthMode;
        /**
         * @brief use the cull mode of the backend
         */
        using CullMode = GLGE::Graphic::Backend::Graphic::Material::CullMode;

        /**
         * @brief Construct a new Material
         * 
         * @param shader the shader for the material
         * @param layout the layout for the material
         * @param cullMode the cull mode for the material
         * @param depthMode the mode to use for depth compares
         * @param depthWrite `true` to enable depth writing, `false` to disable depth writing
         */
        Material(const Shader& shader, const VertexLayout& layout, CullMode cullMode, DepthMode depthMode, bool depthWrite)
         : BaseClass(), m_material(getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicDescription()->createMaterial(shader.getBackend(), layout.getLayout(), cullMode, depthMode, depthWrite))
        {}

        /**
         * @brief Get the Backend
         * 
         * @return `Reference<GLGE::Graphic::Backend::Graphic::Material>` a reference to the backend
         */
        inline Reference<GLGE::Graphic::Backend::Graphic::Material> getBackend() const noexcept
        {return m_material;}

    protected:

        /**
         * @brief store a reference to the material backend
         */
        Reference<GLGE::Graphic::Backend::Graphic::Material> m_material;

    };

}

#endif