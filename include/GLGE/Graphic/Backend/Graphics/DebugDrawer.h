/**
 * @file DebugDrawer.h
 * @author DM8AT
 * @brief define an abstract interface to interact with the debug context to submit debug draw data and record it into a backend command buffer
 * @version 0.1
 * @date 2026-09-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHIC_DEBUG_VISUALIZER_
#define _GLGE_GRAPHIC_BACKEND_GRAPHIC_DEBUG_VISUALIZER_

//add ref-counting
#include "GLGE/Core/Reference.h"

//add the command system
#include "GLGE/Graphic/Command.h"
//add components
#include "GLGE/Graphic/Components.h"
//add render targets
#include "GLGE/Graphic/RenderTarget.h"

//add the command buffer
#include "CommandBuffer.h"

//forward declaration
namespace GLGE::Graphic {
    class DebugContext;
}

//use the library namespace
namespace GLGE::Graphic::Backend::Graphic {

    //forward 
    class Instance;

    /**
     * @brief define a class to render from a debug context
     */
    class DebugDrawer : public Referable, public CommandInvalidator {
    public:

        /**
         * @brief Construct a new Debug Drawer
         * 
         * @param instance a pointer to the graphic instance backend the debug drawer belongs to
         */
        DebugDrawer(GLGE::Graphic::DebugContext* frontend, GLGE::Graphic::Backend::Graphic::Instance* instance);

        /**
         * @brief Destroy the Debug Drawer
         */
        virtual ~DebugDrawer() = default;

        /**
         * @brief a function that is called to notify the debug drawer that is should record itself
         * 
         * @param cmdBuff a reference to the command buffer to record to
         */
        virtual void record(GLGE::Graphic::Backend::Graphic::CommandBuffer& cmdBuff) = 0;

    protected:

        /**
         * @brief store a pointer to the backend graphic instance the debug renderer belongs to
         */
        GLGE::Graphic::Backend::Graphic::Instance* m_inst = nullptr;

    };

}

#endif