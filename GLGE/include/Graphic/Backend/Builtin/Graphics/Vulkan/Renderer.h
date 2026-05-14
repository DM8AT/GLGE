/**
 * @file Renderer.h
 * @author DM8AT
 * @brief define a vulkan renderer
 * @version 0.1
 * @date 2026-05-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_RENDERER_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_RENDERER_

//include the renderer backend
#include "Graphic/Backend/Graphics/Renderer.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::Vulkan {

    /**
     * @brief the renderer overload for the Vulkan backend
     */
    class Renderer : public GLGE::Graphic::Backend::Graphic::Renderer {
    public:

        /**
         * @brief Construct a new Renderer
         * 
         * @param world a reference to the world to render (reference -> cannot be null)
         * @param camera store a pointer to the object to use as a camera, this can be null
         * @param target the target to adapt to
         */
        Renderer(World& world, Object* camera, RenderTarget target);

        /**
         * @brief Destroy the Renderer
         */
        virtual ~Renderer();

        /**
         * @brief render the world
         * 
         * @param cmdBuff the buffer to record the drawing to
         */
        virtual void record(GLGE::Graphic::Backend::Graphic::CommandBuffer& cmdBuff) override;

        /**
         * @brief update the internal data buffers
         */
        virtual void update() override;

    };

}

#endif