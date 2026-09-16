/**
 * @file OpenGL.h
 * @author DM8AT
 * @brief define the contract between any video backend and the OpenGL graphic API
 * @version 0.1
 * @date 2026-05-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_VIDEO_CONTRACTS_OPENGL_
#define _GLGE_GRAPHIC_BACKEND_VIDEO_CONTRACTS_OPENGL_

//add the contract base
#include "Contracts.h"

//add vsync
#include "Graphic/VSync.h"

//dummy definitions of required classes
namespace GLGE::Graphic::Backend::Video 
{class Window;}

//use the backend video namespace
namespace GLGE::Graphic::Backend::Video::Contracts {

    /**
     * @brief define the contract base class for all OpenGL contracts
     */
    class OpenGL : public BaseContract {
    public:

        /**
         * @brief Construct a new Open GL contract
         * 
         * @param instance a pointer to the video instance that owns the contract
         */
        OpenGL(GLGE::Graphic::Backend::Video::Instance* instance)
         : BaseContract(instance)
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
        virtual void createContext(GLGE::Graphic::Backend::Video::Window* window) = 0;

        /**
         * @brief destroy the stored context
         */
        virtual void destroyContext() = 0;

        /**
         * @brief make a window the current window in eyes of the context
         * 
         * @param window a pointer to the window to make current
         */
        virtual void makeCurrent(GLGE::Graphic::Backend::Video::Window* window) = 0;

        /**
         * @brief swap a specific window
         * 
         * @param window a pointer to the window to swap
         */
        virtual void swap(GLGE::Graphic::Backend::Video::Window* window) = 0;

        /**
         * @brief Set the vsync swap interval
         * 
         * @param vsync the vsync mode to use
         * @return `true` if the state was set successfully, `false` if something went wrong
         */
        virtual bool setVsync(GLGE::Graphic::VSync vsync) = 0;
        
    protected:

        /**
         * @brief store the actual OpenGL context
         * 
         * This is done as a nullptr so it can store anything. The layout is defined by the video backend and opaque for the graphic backend. 
         */
        void* m_context = nullptr;

    };

}

#endif