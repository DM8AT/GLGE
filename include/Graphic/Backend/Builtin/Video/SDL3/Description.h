/**
 * @file Description.h
 * @author DM8AT
 * @brief define the overload for the SDL 3 backend
 * @version 0.1
 * @date 2025-12-29
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_VIDEO_SDL_3_DESCRIPTION_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_VIDEO_SDL_3_DESCRIPTION_

//include the backend description for video backends
#include "Graphic/Backend/Video/VideoDescription.h"

//use the builtin namespace
namespace GLGE::Graphic::Builtin {
/**
 * @brief a namespace that stores all video API related build-in descriptor types
 */
namespace Video {

    /**
     * @brief define the description for the SDL 3 video backend
     */
    class SDL3 : public GLGE::Graphic::Backend::Video::Description {
    public:

        /**
         * @brief Construct a new SDL3 description
         */
        SDL3() = default;

        /**
         * @brief Destroy the SDL3 description
         */
        virtual ~SDL3() {}

        /**
         * @brief Get the Name of the backend
         * 
         * @return const char* the constant backend name
         */
        virtual const char* getName() const noexcept override;

        /**
         * @brief Create a Instance
         * 
         * @param instance a pointer to the frontend instance 
         * @param api the graphic API to specialize the instance for
         * @param version the version of the graphic API to use
         * @return `GLGE::Graphic::Backend::Instance*` a pointer to the new backend instance
         */
        virtual GLGE::Graphic::Backend::Video::Instance* createInstance(GLGE::Graphic::Instance* instance, GLGE::Graphic::GraphicAPI api, GLGE::Version version) override;
        /**
         * @brief Create a Window
         * 
         * @param window a pointer to the window to create the backend window for
         * @return `GLGE::Graphic::Backend::Window*` a pointer to the new backend window
         */
        virtual GLGE::Graphic::Backend::Video::Window* createWindow(GLGE::Graphic::Window* window) override;

        /**
         * @brief Get a list of the graphic APIs supported by the backend
         * 
         * @return `const std::vector<GLGE::Graphic::GraphicAPI>&` a constant reference to a vector containing all the supported APIs
         */
        virtual const std::vector<GLGE::Graphic::GraphicAPI>& getSupportedAPIs() override;

    };

}
}

#endif