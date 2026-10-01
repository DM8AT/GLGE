/**
 * @file ImGUIProvider.h
 * @author DM8AT
 * @brief provide a default provider for Dear ImGUI
 * @version 0.1
 * @date 2026-09-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_DEFAULT_GUI_PROVIDER_IMGUI_PROVIDER_
#define _GLGE_GRAPHIC_DEFAULT_GUI_PROVIDER_IMGUI_PROVIDER_

//add the GUI provider
#include "GLGE/Graphic/GUIProvider.h"

//add ImGUI
#include "imgui.h"

//add render targets
#include "GLGE/Graphic/RenderTarget.h"

//add base classes
#include "GLGE/Core/BaseClass.h"

//add cursors
#include "GLGE/Graphic/Cursor.h"

//add chrono
#include <chrono>

//library namespace
namespace GLGE::Graphic {

    /**
     * @brief a GUI provider designed for Dear ImGUI
     */
    class ImGuiProvider : public GUIProvider, public BaseClass {
    public:

        /**
         * @brief Construct a new ImGui Provider
         * 
         * @param target the target to draw the GUI to
         * @param context a pointer to the ImGUI context to attach to, use nullptr to bind to the current context
         */
        explicit ImGuiProvider(const RenderTarget& target, ImGuiContext* context = nullptr);

        /**
         * @brief Destroy the ImGui Provider
         */
        ~ImGuiProvider();

        /**
         * @brief start a new frame
         * 
         * Notify the backend that a new frame started. This prepares internal state. 
         */
        void newFrame();

        /**
         * @brief render a frame
         * 
         * @param drawData a pointer to the draw data to render from. Input `nullptr` to fetch the draw data yourself. 
         */
        void render(ImDrawData* drawData = nullptr);

        /**
         * @brief Set the Target
         * 
         * @note This function is valid independent from the recording state
         * 
         * @param target the new target
         */
        inline void setTarget(const RenderTarget& target)
        {assert(target.getTarget() == nullptr); m_target = target;}

        /**
         * @brief Get the Target
         * 
         * @return `const RenderTarget&` a constant reference to the current target
         */
        inline const RenderTarget& getTarget() const noexcept
        {return m_target;}

        /**
         * @brief Get the Graphic Instance
         * 
         * @return `Graphic::Instance*` a pointer to the graphic instance
         */
        inline Graphic::Instance* getGraphicInstance() const noexcept
        {return m_gInst;}

        /**
         * @brief Set the Clipboard Data
         * 
         * @param clipboard the clipboard data to store
         */
        inline void setClipboardData(const std::string& clipboard) noexcept
        {m_clipboardText = clipboard;}

        /**
         * @brief Get the Clipboard Text
         * 
         * @return `const std::string&` the clipboard text
         */
        inline const std::string& getClipboardData() const noexcept
        {return m_clipboardText;}

    protected:

        /**
         * @brief define which clock the provider is using
         */
        using clock = std::chrono::steady_clock;

        /**
         * @brief store the time point the last frame computed delta time at
         */
        clock::time_point m_lastFrame = clock::now();

        /**
         * @brief store the graphic instance
         */
        Graphic::Instance* m_gInst = nullptr;

        /**
         * @brief store the render target to render to by default
         */
        RenderTarget m_target;

        /**
         * @brief store the ImGui context the provider belongs to
         */
        ImGuiContext* m_ctx = nullptr;

        /**
         * @brief store the clipboard text
         */
        std::string m_clipboardText;

        /**
         * @brief store the shader used by the ImGui backend
         */
        Shader m_shader;

        /**
         * @brief store cursor defaults
         */
        GLGE::Graphic::Cursor m_cursors[ImGuiMouseCursor_COUNT] {};

        /**
         * @brief store all the images currently referenced
         */
        std::vector<GLGE::Graphic::Image*> m_images;

    };

}

#endif