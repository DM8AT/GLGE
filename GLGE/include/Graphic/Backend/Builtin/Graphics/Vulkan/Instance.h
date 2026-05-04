/**
 * @file Instance.h
 * @author DM8AT
 * @brief define an overload of the backend instance for Vulkan
 * @version 0.1
 * @date 2026-04-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_INSTANCE_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_INSTANCE_

//add backend instances
#include "Graphic/Backend/Graphics/Instance.h"

//include common stuff
#include "Core/Common.h"
//add atomics
#include <atomic>

//backend windows are defined elsewhere
namespace GLGE::Graphic::Backend::Graphic {class Window;}

//use the graphic backend OpenGL namespace
namespace GLGE::Graphic::Backend::Graphic {
/**
 * @brief a namespace that stores the OpenGL overloads for all graphic backend related classes
 */
namespace Vulkan {

    /**
     * @brief extend the graphic instance for vulkan functionality
     */
    class Instance : public GLGE::Graphic::Backend::Graphic::Instance {
    public:

        /**
         * @brief Construct a new Instance for OpenGL
         * 
         * @param instance a pointer to the frontend instance to attach to
         */
        Instance(GLGE::Graphic::Instance* instance);

        /**
         * @brief Destroy the Instance
         */
        virtual ~Instance();

        /**
         * @brief a hook that is called when the instance is bound
         */
        virtual void onBind() override;

        /**
         * @brief a hook that is called when the instance is unbound
         */
        virtual void onUnbind() override;

        /**
         * @brief a function that is called when a new window using the graphic instance was created
         * 
         * @param window a pointer to the new window
         */
        virtual void onRegisterWindow(GLGE::Graphic::Backend::Graphic::Window* window) override;

        /**
         * @brief a function that is called when a window that belonged to the instance was closed
         * 
         * @param window a pointer to the closed window
         */
        virtual void onRemoveWindow(GLGE::Graphic::Backend::Graphic::Window* window) override;

    protected:

        /**
         * @brief store the amount of known windows
         */
        std::atomic_uint64_t m_windowCount = 0;

        /**
         * @brief store the vulkan instance
         */
        void* m_instance = nullptr;
        /**
         * @brief store the selected physical device
         */
        void* m_physicalDevice = nullptr;

    };

}
}

#endif