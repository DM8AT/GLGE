/**
 * @file Instance.h
 * @author DM8AT
 * @brief define the overload of the video backend instance for SDL3
 * @version 0.1
 * @date 2025-12-29
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_VIDEO_SDL3_INSTANCE_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_VIDEO_SDL3_INSTANCE_

//include video API instances
#include "Graphic/Backend/Video/Instance.h"
//include atomics
#include <atomic>
//get graphic APIs
#include "Graphic/GraphicAPI.h"
//add a mutex for thread safety
#include <shared_mutex>
#include <mutex>

//use the namespace
namespace GLGE::Graphic::Backend::Video {
/**
 * @brief a namespace that stores the SDL overloads for all video backend related classes
 */
namespace SDL3 {

    /**
     * @brief an overload of the video backend instance for SDL3
     */
    class Instance : public GLGE::Graphic::Backend::Video::Instance {
    public:

        /**
         * @brief Construct a new Instance
         * 
         * @param instance a pointer to the frontend instance 
         * @param api the graphic API to specialize the instance for
         * @param version the version of the graphic API to use
         */
        Instance(GLGE::Graphic::Instance* instance, GLGE::Graphic::GraphicAPI api, GLGE::Version version);

        /**
         * @brief Destroy the Instance
         */
        virtual ~Instance();

        /**
         * @brief Get the Display Setup known to the video system
         * 
         * @return `const GLGE::Graphic::DisplaySetup&` a constant reference to the video system
         */
        virtual const GLGE::Graphic::DisplaySetup& getDisplaySetup() const noexcept override
        {return msl_displaySetup;}

        /**
         * @brief a function that is called every time an update occurred
         * 
         * This should quarry all events and update what requires updating
         */
        virtual void onUpdate() override;

        /**
         * @brief a function that is used to inform the instance of the existence of a new window
         * 
         * @param window a pointer to the window to register
         */
        virtual void onWindowRegister(GLGE::Graphic::Backend::Video::Window* window) override;

        /**
         * @brief a function that is used to inform the instance of the destruction of a window
         * 
         * @param window a pointer to the window that no longer exists
         */
        virtual void onWindowRemove(GLGE::Graphic::Backend::Video::Window* window) override;

        /**
         * @brief create a new context for a specific graphic API
         * 
         * @param window a pointer to a window to base the context on
         * @return void* a new OPAQUE handle for the context
         */
        virtual void* onContextCreate(GLGE::Graphic::Backend::Video::Window* window) override;

        /**
         * @brief destroy a context of a specific graphic API
         * 
         * @param context a pointer to the OPAQUE context handle to destroy
         */
        virtual void onContextDestroy(void* context) override;

    protected:

        /**
         * @brief map the SDL3 keyboard IDs to the instance's keyboard Ids
         */
        std::unordered_map<int, u8> m_keyboardMap;
        /**
         * @brief map the SDL3 mouse IDs to the instance's mouse Ids
         */
        std::unordered_map<int, u8> m_miceMap;

        /**
         * @brief store the amount of instances of SDL3 that currently exist
         */
        inline static std::atomic_uint64_t msl_instanceCount = 0;

        /**
         * @brief store pointers to all instances that use SDL 3
         */
        inline static std::vector<GLGE::Graphic::Instance*> msl_instances;
        /**
         * @brief a mutex to make the vector of instances thread safe
         */
        inline static std::shared_mutex msl_instanceLock;

        /**
         * @brief store the display setup known to SDL 3
         */
        inline static GLGE::Graphic::DisplaySetup msl_displaySetup;

        /**
         * @brief store mappings from window IDs to the windows known to the instance
         */
        inline static std::unordered_map<u32, GLGE::Graphic::Backend::Video::Window*> msl_windows = {};
        /**
         * @brief store the mutex to use for the window list
         */
        inline static std::shared_mutex msl_windowMutex;

        /**
         * @brief a function that is called from the main thread to pull all events
         */
        static void mainUpdate();

        /**
         * @brief update a key
         * 
         * @param state the state of the key (`true` for pressed, `false` for released)
         * @param keycode the code of the key to press
         * @param keyboardId the ID of the keyboard the key is pressed on
         */
        void keyUpdate(bool state, u64 keycode, u32 keyboardId);

        /**
         * @brief register a new keyboard
         * 
         * @param keyboardId the ID of the keyboard to register
         */
        void registerKeyboard(u32 keyboardId);

        /**
         * @brief remove a keyboard
         * 
         * @param keyboardId the ID of the keyboard to remove
         */
        void removeKeyboard(u32 keyboardId);

    };

}
}

#endif