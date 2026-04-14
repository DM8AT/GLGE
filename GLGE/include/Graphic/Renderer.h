/**
 * @file Renderer.h
 * @author DM8AT
 * @brief define a system that is used to render a whole world at once
 * @version 0.1
 * @date 2026-04-05
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_RENDERER_
#define _GLGE_GRAPHIC_RENDERER_

//add common stuff
#include "Core/Common.h"
//add pase classes
#include "Core/BaseClass.h"
//add core instances
#include "Core/Instance.h"
//add graphic instances
#include "Instance.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief define the renderer system for rendering an entire world
     */
    class Renderer : public BaseClass {
    public:

        /**
         * @brief Construct a new Renderer
         * 
         * @param world a reference to the world to render (reference -> cannot be null)
         * @param camera store a pointer to the object to use as a camera, this can be null
         * @param target a target to render to
         */
        Renderer(World& world, Object* camera, RenderTarget target)
         : BaseClass(), m_renderer(getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicDescription()->createRenderer(world, camera, target))
        {}

        /**
         * @brief Destroy the Renderer
         */
        ~Renderer() = default;

        /**
         * @brief tick the renderer to update camera and object transforms
         */
        inline void update() 
        {m_renderer->update();}

        /**
         * @brief Get the backend
         * 
         * @return `Reference<GLGE::Graphic::Backend::Graphic::Renderer>` a reference to the backend renderer
         */
        inline Reference<GLGE::Graphic::Backend::Graphic::Renderer> getBackend() const noexcept
        {return m_renderer;}

        /**
         * @brief Get the camera buffer
         * 
         * @return `GLGE::Graphic::Buffer*` a pointer to the camera buffer
         */
        inline GLGE::Graphic::Buffer* getCameraBuffer() const noexcept
        {return m_renderer->getCameraBuffer();}

        /**
         * @brief Get the transform buffer
         * 
         * @return `const GLGE::Graphic::Buffer*` a pointer to the transform buffer
         */
        inline GLGE::Graphic::Buffer* getTransformBuffer() const noexcept
        {return m_renderer->getTransformBuffer();}

        /**
         * @brief Get the buffer that stores all the point lights
         * 
         * @return `GLGE::Graphic::Buffer*` a pointer to the buffer of all point lights
         */
        inline GLGE::Graphic::Buffer* getPointLightBuffer() const noexcept
        {return m_renderer->getPointLightBuffer();}

        /**
         * @brief Get the buffer that stores all the spot lights
         * 
         * @return `GLGE::Graphic::Buffer*` a pointer to the buffer of all spot lights
         */
        inline GLGE::Graphic::Buffer* getSpotLightBuffer() const noexcept
        {return m_renderer->getSpotLightBuffer();}

        /**
         * @brief Get the buffer that stores all the directional lights
         * 
         * @return `GLGE::Graphic::Buffer*` a pointer to the buffer of all directional lights
         */
        inline GLGE::Graphic::Buffer* getDirectionalLightBuffer() const noexcept
        {return m_renderer->getDirectionalLightBuffer();}

    protected:

        /**
         * @brief store a reference to the renderer
         */
        Reference<GLGE::Graphic::Backend::Graphic::Renderer> m_renderer;

    };

}

#endif