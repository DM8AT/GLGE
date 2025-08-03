/**
 * @file GLGE_Software_Instance.h
 * @author JuNi4
 * @brief deine instances for the software rasterizer
 * @version 0.1
 * @date 2025-06-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_SOFTWARE_INSTANCE_
#define _GLGE_SOFTWARE_INSTANCE_

//include instances
#include "../../GLGEGraphicAPI_Instance.h"
#include "GLGE_Software_CommandBuffer.h"

//check if this is C++
#if GLGE_CPP

class Software_Window;

class Software_Instance : public GraphicInstance
{
public:

    /**
     * @brief Construct a new software instance
     */
    Software_Instance() = default;

    /**
     * @brief Construct a new software instance
     * 
     * @param instance a pointer to the instance the graphic instance belongs to
     */
    Software_Instance(Instance* instance) : GraphicInstance(instance) { onCreate(); }

    /**
     * @brief run the rendering for the software rasterizer instance
     */
    void onRender() override;

    void onCreate() override;

    void onDestroy() override;

    //check if ImGui should be used
    #if GLGE_3RD_PARTY_INCLUDE_DEAR_IMGUI
    /**
     * @brief Initializes ImGui
     * 
     * @param window The window ImGui will be initialized to
     */
    void initializeImGui(Software_Window* window);

    /**
     * @brief Check if ImGui was initialized
     * 
     * @return true ImGui is initialized
     * @return false ImGui is not initialized
     */
    inline bool isImGuiInitialized() noexcept { return m_isImGuiInitialized; }
    #endif

protected:

    Software_CommandBuffer m_dataBuffer;

    //check if ImGui should be used
    #if GLGE_3RD_PARTY_INCLUDE_DEAR_IMGUI
    bool m_isImGuiInitialized = false;
    #endif
};

#endif //C++ section

#endif