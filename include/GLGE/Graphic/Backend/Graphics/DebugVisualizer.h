/**
 * @file DebugVisualizer.h
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

//add the command system
#include "GLGE/Graphic/Command.h"
//add components
#include "GLGE/Graphic/Components.h"
//add render targets
#include "GLGE/Graphic/RenderTarget.h"

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
    class DebugDrawer : public CommandInvalidator {
    public:

        /**
         * @brief Construct a new Debug Drawer
         * 
         * @param instance a pointer to the graphic instance backend the debug drawer belongs to
         */
        DebugDrawer(GLGE::Graphic::Backend::Graphic::Instance* instance)
         : CommandInvalidator(), m_inst(instance)
        {assert(m_inst == nullptr);}

        /**
         * @brief Destroy the Debug Drawer
         */
        virtual ~DebugDrawer() = default;

        /**
         * @brief start the recording of the debug context
         * 
         * This readies the context for debug recording. This sets the state to `RECORDING` and prepares the backend. 
         * 
         * @warning this call is only valid if the context is NOT in a recording state
         * @note this does not invalidate old recordings. The backend should create a new recording target while leaving a potential old one valid. 
         */
        virtual void beginRecording();

        /**
         * @brief Set the Camera
         * 
         * @warning this call is only valid if the context is in a recording state
         * @note this needs to be submitted before calling any submit renderer commands
         * 
         * @param camera the camera component to use
         */
        virtual void setCamera(const Component::Camera& camera);

        /**
         * @brief Set the Target
         * 
         * @warning this call is only valid if the context is in a recording state
         * @note this needs to be submitted before calling any submit renderer commands
         * 
         * @param target the target to render to
         */
        void setTarget(const RenderTarget& target);

        /**
         * @brief submit a list of objects to render
         * 
         * @warning before this call is recorded the `setCamera` and `setTarget` commands must be called at least once. 
         * 
         * @param renderer the renderer to render from
         */
        void submitRenderer(const DebugRenderer& renderer);

        /**
         * @brief finish the recording
         * 
         * This sets the state to `RECORDED` and finalizes the backend. 
         * 
         * @warning this call is only valid if the context is in a recording state
         * @note this call invalidates the old recording. 
         */
        void endRecording();

        /**
         * @brief register a frontent
         * 
         * @param frontent a reference to the invalidator frontent
         */
        void registerFrontend(CommandInvalidator& frontent)
        {attachInvalidator(frontent);}

    protected:

        /**
         * @brief store a pointer to the backend graphic instance the debug renderer belongs to
         */
        GLGE::Graphic::Backend::Graphic::Instance* m_inst = nullptr;

    };

}

#endif