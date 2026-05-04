/**
 * @file OpenGL.h
 * @author DM8AT
 * @brief define the contract overload for an OpenGL contract for SDL3
 * @version 0.1
 * @date 2026-05-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_VIDEO_SDL3_INSTANCE_CONTRACT_OPENGL_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_VIDEO_SDL3_INSTANCE_CONTRACT_OPENGL_

//add the OpenGL contract
#include "Graphic/Backend/Video/APIContracts/OpenGL.h"

//use the namespace
namespace GLGE::Graphic::Backend::Video::SDL3 {

    /**
     * @brief store all contracts for the SDL3 default implementations
     */
    namespace Contracts {

        /**
         * @brief the overload of the default OpenGL contract for SDL3
         */
        class OpenGL : public GLGE::Graphic::Backend::Video::Contracts::OpenGL {
        public:

            /**
             * @brief Construct a new Open GL contract
             * 
             * @param instance a pointer to the video instance that owns the contract
             */
            OpenGL(GLGE::Graphic::Backend::Video::Instance* instance)
            : GLGE::Graphic::Backend::Video::Contracts::OpenGL(instance)
            {}

            /**
             * @brief Destroy the Open GL contract
             */
            virtual ~OpenGL() = default;

            /**
             * @brief Create a Context
             * 
             * @param window a pointer to the video window that the context should be created with
             */
            virtual void createContext(GLGE::Graphic::Backend::Video::Window* window) override;

            /**
             * @brief destroy the stored context
             */
            virtual void destroyContext() override;

            /**
             * @brief make a window the current window in eyes of the context
             * 
             * @param window a pointer to the window to make current
             */
            virtual void makeCurrent(GLGE::Graphic::Backend::Video::Window* window) override;

            /**
             * @brief swap a specific window
             * 
             * @param window a pointer to the window to swap
             */
            virtual void swap(GLGE::Graphic::Backend::Video::Window* window) override;

            /**
             * @brief Set the vsync swap interval
             * 
             * @param vsync the vsync mode to use
             * @return `true` if the state was set successfully, `false` if something went wrong
             */
            virtual bool setVsync(GLGE::Graphic::VSync vsync) override;

        };

    }

}

#endif