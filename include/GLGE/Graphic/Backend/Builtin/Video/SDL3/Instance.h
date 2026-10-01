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
#include "GLGE/Graphic/Backend/Video/Instance.h"
//include atomics
#include <atomic>
//get graphic APIs
#include "GLGE/Graphic/GraphicAPI.h"
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
         * @brief open a specific URL
         * 
         * @param url the URL to open
         */
        virtual void openURL(const std::string& url);

        /**
         * @brief Get the Types currently present in the clipboard
         * 
         * @return `std::vector<std::string>` a list of all data types present in the clipboard
         */
        virtual std::vector<std::string> getClipboardTypes() override;

        /**
         * @brief Get the specific clipboard Data
         * 
         * @param typeName the name of the data to get
         * @return `std::vector<u8>` the data stored in the clipboard
         */
        virtual std::vector<u8> getClipboardData(const std::string& typeName) override;

        /**
         * @brief Set the Clipboard Data
         * 
         * @param data the data to write to the clipboard
         * @param size the length of the data to write
         * @param typeName the type of the data to set
         */
        virtual void setClipboardData(const void* data, size_t size, const std::string& typeName) override;

        /**
         * @brief Get the Clipboard Text
         * 
         * @return `std::string` the string stored in the clipboard
         */
        virtual std::string getClipboardText() override;

        /**
         * @brief Set the Clipboard Text
         * 
         * @param text the new text for the clipboard
         */
        virtual void setClipboardText(const std::string& text) override;

        /**
         * @brief hide the mouse cursor
         */
        virtual void hideCursor() override;
        /**
         * @brief show the mouse cursor
         */
        virtual void showCursor() override;
        /**
         * @brief check if the mouse cursor is hidden
         * 
         * @return `true` if the cursor is hidden, `false` if the cursor is visible
         */
        virtual bool isCursorHidden() override;

        /**
         * @brief open a file selector dialog
         * 
         * @warning The OS may ignore any of the options
         * 
         * @param callback a file selector callback (called whenever the selector is closed). If zero elements are parsed, the user canceled the selection. 
         * @param allowMultiSelect `true` to allow for multiple files to be selected, `false` if not
         * @param filter a list of filters. The first string is the human-readable name, the second one is the pattern of inclusion. 
         * @param defaultLocation the location to open the selector at
         * @param parent the parent window to attach to, nullptr means no window
         */
        virtual void openFileSelector(Pfn_SelectorCallback callback, bool allowMultiSelect, const std::vector<std::pair<std::string, std::string>>& filter = {}, const std::filesystem::path& defaultLocation = "", GLGE::Graphic::Window* parent = nullptr) override;

        /**
         * @brief open a file selector dialog
         * 
         * @warning The OS may ignore any of the options
         * 
         * @param callback a file selector callback (called whenever the selector is closed). If zero elements are parsed, the user canceled the selection. 
         * @param filter a list of filters. The first string is the human-readable name, the second one is the pattern of inclusion. 
         * @param defaultLocation the location to open the selector at
         * @param parent the parent window to attach to, nullptr means no window
         */
        virtual void openSaveSelector(Pfn_SelectorCallback callback, const std::vector<std::pair<std::string, std::string>>& filter = {}, const std::filesystem::path& defaultLocation = "", GLGE::Graphic::Window* parent = nullptr) override;

        /**
         * @brief open a file selector dialog
         * 
         * @warning The OS may ignore any of the options
         * 
         * @param callback a folder selector callback (called whenever the selector is closed). If zero elements are parsed, the user canceled the selection. 
         * @param allowMultiSelect `true` to allow for multiple files to be selected, `false` if not
         * @param defaultLocation the location to open the selector at
         * @param parent the parent window to attach to, nullptr means no window
         */
        virtual void openFolderSelector(Pfn_SelectorCallback callback, bool allowMultiSelect, const std::filesystem::path& defaultLocation = "", GLGE::Graphic::Window* parent = nullptr) override;

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

        /**
         * @brief register a mouse button update
         * 
         * @param state the new state for the button
         * @param buttonId the ID of the button
         * @param mouseId the ID of the mouse to update
         */
        void mouseButtonUpdate(bool state, u8 buttonId, u32 mouseId);

        /**
         * @brief register a mouse movement
         * 
         * @param pos the new position of the mouse
         * @param mouseId the ID of the mouse
         */
        void mousePositionUpdate(const vec2& pos, u32 mouseId);

        /**
         * @brief register the scrolling of a mouse wheel
         * 
         * @param scroll how much the wheel was scrolled (yes, that can be 2D)
         * @param mouseId the Id of the mouse that was scrolled
         */
        void mouseWheelUpdate(const vec2& scroll, u32 mouseId);

        /**
         * @brief register a new mice
         * 
         * @param miceId the ID of the mice to add
         */
        void registerMice(u32 miceId);

        /**
         * @brief remove a mice
         * 
         * @param miceId the ID of the mice to remove
         */
        void removeMice(u32 miceId);

    };

}
}

#endif