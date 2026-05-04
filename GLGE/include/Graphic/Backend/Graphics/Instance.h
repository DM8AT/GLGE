/**
 * @file Instance.h
 * @author DM8AT
 * @brief define the abstract base class for a backend instance
 * @version 0.1
 * @date 2025-12-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHICS_INSTANCE_
#define _GLGE_GRAPHIC_BACKEND_GRAPHICS_INSTANCE_

//add references
#include "Core/Reference.h"
//include display setups
#include "Graphic/DisplaysSetup.h"

//graphic instances will be defined later
namespace GLGE::Graphic {class Instance;}

//use the graphic backend namespace
namespace GLGE::Graphic::Backend {
/**
 * @brief a namespace that stores all graphic backend related classes
 */
namespace Graphic {

    //windows are defined elsewhere
    class Window;

    /**
     * @brief a structure that holds information about all extensions
     */
    struct ExtensionInfo {
        /**
         * @brief store information about the anisotropic extension
         */
        struct Anisotropic {
            /**
             * @brief `true` if the extension is supported, `false` otherwise
             */
            bool supported = false;
            /**
             * @brief store the maximal supported anisotropy level
             */
            float maxAnisotropy = 1;
        };
        /**
         * @brief store information about the multi sampling extension
         */
        struct MultiSample {
            /**
             * @brief store the maximum amount of allowed samples per pixel
             */
            i32 maxSampleCount = 1;
        };
        /**
         * @brief store information about the anisotropic extension
         */
        Anisotropic anisotropic;
        /**
         * @brief store information about the multi sample extension
         */
        MultiSample multiSample;
    };

    /**
     * @brief an abstract class that is used to interface with the instance backend
     */
    class Instance : public Referable {
    public:

        /**
         * @brief Construct a new Instance backend
         * 
         * @param instance a pointer to the frontend instance the backend belongs to
         */
        Instance(GLGE::Graphic::Instance* instance) noexcept
         : m_instance(instance)
        {}

        /**
         * @brief Destroy the Instance backend
         */
        virtual ~Instance() {}

        /**
         * @brief a hook that is called when the instance is bound
         */
        virtual void onBind() = 0;

        /**
         * @brief a hook that is called when the instance is unbound
         */
        virtual void onUnbind() = 0;

        /**
         * @brief a function that is called when a new window using the graphic instance was created
         * 
         * @param window a pointer to the new window
         */
        virtual void onRegisterWindow(GLGE::Graphic::Backend::Graphic::Window* window) = 0;

        /**
         * @brief a function that is called when a window that belonged to the instance was closed
         * 
         * @param window a pointer to the closed window
         */
        virtual void onRemoveWindow(GLGE::Graphic::Backend::Graphic::Window* window) = 0;

        /**
         * @brief get if HDR is supported on instance level
         * 
         * @return `true` if HDR is supported, `false` otherwise
         */
        inline bool supportsHDR() const noexcept
        {return m_supportsHDR;}

        /**
         * @brief Get the Extension Info
         * 
         * @return `const ExtensionInfo&` a constant reference to the extension info
         */
        inline const ExtensionInfo& getExtensionInfo() const noexcept
        {return m_extensionInfo;}

    protected:

        /**
         * @brief store the frontend instance the backend belongs to
         */
        GLGE::Graphic::Instance* m_instance;
        /**
         * @brief store if the instance supports HDR
         */
        bool m_supportsHDR = false;

        /**
         * @brief store information about the extensions
         */
        ExtensionInfo m_extensionInfo;
        
    };

}
}

#endif