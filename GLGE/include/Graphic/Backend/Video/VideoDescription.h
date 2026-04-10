/**
 * @file VideoDescription.h
 * @author DM8AT
 * @brief A virtual base class that contains factories for all functions required for video backends
 * @version 0.1
 * @date 2025-12-29
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_VIDEO_DESCRIPTION_
#define _GLGE_GRAPHIC_BACKEND_VIDEO_DESCRIPTION_

//include the graphic API
#include "Graphic/GraphicAPI.h"

//define some pointers to frontend classes
namespace GLGE::Graphic {
    class Instance;
    class Window;
}

//use the backend namespace
namespace GLGE::Graphic::Backend::Video {

    //video backend instances are defined elsewhere
    class Instance;

    //video backend windows are defined elsewhere
    class Window;

    /**
     * @brief a class with virtual factories for all video backend classes
     */
    class Description {
    public:
    
        /**
         * @brief Construct a new Description for a video backend
         */
        Description() = default;

        /**
         * @brief Destroy the Description of a video backend
         */
        virtual ~Description() {}

        /**
         * @brief Get the name of the video backend
         * 
         * @return const char* the name of the video backend
         */
        virtual const char* getName() const noexcept = 0;

        /**
         * @brief Create a new video backend Instance 
         * 
         * @param instance a pointer to the frontend instance 
         * @param api the graphic API to specialize the instance for
         * @param version the version of the graphic API to use
         * @return `GLGE::Graphic::Backend::Video::Instance*` a pointer to a new video backend instance
         */
        virtual GLGE::Graphic::Backend::Video::Instance* createInstance(GLGE::Graphic::Instance* instance, GLGE::Graphic::GraphicAPI api, GLGE::Version version) = 0;

        /**
         * @brief Create a new Window for a video backend
         * 
         * @param window a pointer to the frontend window this video backend belongs to
         * 
         * @return `GLGE::Graphic::Backend::Video::Window*` a pointer to a new video backend window
         */
        virtual GLGE::Graphic::Backend::Video::Window* createWindow([[maybe_unused]]GLGE::Graphic::Window* window) = 0;

        /**
         * @brief Get a list of the graphic APIs supported by the backend
         * 
         * @return `const std::vector<GLGE::Graphic::GraphicAPI>&` a constant reference to a vector containing all the supported APIs
         */
        virtual const std::vector<GLGE::Graphic::GraphicAPI>& getSupportedAPIs() = 0;

    };

}

#endif