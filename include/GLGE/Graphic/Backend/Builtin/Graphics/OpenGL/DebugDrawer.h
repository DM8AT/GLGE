/**
 * @file DebugDrawer.h
 * @author DM8AT
 * @brief define the OpenGL debug drawer
 * @version 0.1
 * @date 2026-09-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHIC_OGL_DEBUG_DRAWER_
#define _GLGE_GRAPHIC_BACKEND_GRAPHIC_OGL_DEBUG_DRAWER_

//add the debug drawer
#include "GLGE/Graphic/Backend/Graphics/DebugDrawer.h"

//use the library namespace
namespace GLGE::Graphic::Backend::Graphic::OpenGL {

    /**
     * @brief the OpenGL debug drawer class
     */
    class DebugDrawer : public GLGE::Graphic::Backend::Graphic::DebugDrawer {
    public:

        /**
         * @brief Construct a new Debug Drawer
         * 
         * @param instance a pointer to the graphic instance backend the debug drawer belongs to
         */
        DebugDrawer(GLGE::Graphic::Backend::Graphic::Instance* instance);

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
        virtual void beginRecording() = 0;

        /**
         * @brief Set the Camera
         * 
         * @warning this call is only valid if the context is in a recording state
         * @note this needs to be submitted before calling any submit renderer commands
         * 
         * @param camera the camera component to use
         */
        virtual void setCamera(const Component::Camera& camera) = 0;

        /**
         * @brief Set the Target
         * 
         * @warning this call is only valid if the context is in a recording state
         * @note this needs to be submitted before calling any submit renderer commands
         * 
         * @param target the target to render to
         */
        virtual void setTarget(const RenderTarget& target) = 0;

        /**
         * @brief submit a list of objects to render
         * 
         * @warning before this call is recorded the `setCamera` and `setTarget` commands must be called at least once. 
         * 
         * @param renderer the renderer to render from
         */
        virtual void submitRenderer(const DebugRenderer& renderer) = 0;

        /**
         * @brief finish the recording
         * 
         * This sets the state to `RECORDED` and finalizes the backend. 
         * 
         * @warning this call is only valid if the context is in a recording state
         * @note this call invalidates the old recording. 
         */
        virtual void endRecording() = 0;

    protected:



    };

}

#endif