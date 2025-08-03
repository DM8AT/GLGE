/**
 * @file GLGE_Software_RenderPipeline.h
 * @author JuNi4
 * @brief 
 * @version 0.1
 * @date 2025-07-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_SOFTWARE_RENDER_PIPELINE_
#define _GLGE_SOFTWARE_RENDER_PIPELINE_

//include graphic render pipelines
#include "../../GLGEGraphicAPI_RenderPipeline.h"
//include render pipelines
#include "../../../GLGERenderPipeline.h"

//check for C++
#if GLGE_CPP

/**
 * @brief store a render pipeline 
 */
class Software_RenderPipeline : public GraphicRenderPipeline
{
public:

    /**
     * @brief Construct a new software render pipeline 
     * 
     */
    Software_RenderPipeline() = default;

    /**
     * @brief Construct a new software render pipeline
     * 
     * @param renderPipeline a pointer to the high level render pipeline object
     */
    Software_RenderPipeline(RenderPipeline* renderPipeline) : GraphicRenderPipeline(renderPipeline) {}

    /**
     * @brief this function is called when the render pipeline is created
     */
    virtual void onAttatch() noexcept override;

    /**
     * @brief this function is called when the render pipeline is destroyed
     */
    virtual void onDestroy() noexcept override;

    /**
     * @brief Get all the Supported Stage Types
     * 
     * @param types a vector to fill with all the identifyers this render pipeline supports
     */
    virtual void getSupportedStageTypes(std::vector<StageType>& types) const noexcept override;

    /**
     * @brief execute a single stage of the render pipeline
     * 
     * @param stageIndex the index of the stage to execute
     */
    virtual void onStageExecution(uint64_t stageIndex) noexcept override;

protected:

    /**
     * @brief handle the ququing of a blit from a window to window render stage
     * 
     * @param stage a reference to the stage that requested the blit
     */
    void blitToWindow(RenderStage& stage);

    /**
     * @brief execute a bufferd blit command from a framebuffer to a window
     * 
     * @param data the data for the command execution
     */
    static void software_blitToWindow(void* data, uint64_t) noexcept;

    /**
     * @brief execute a compute shader
     * 
     * @param data the data to execute a compute shader
     */
    static void software_executeCompute(void* data, uint64_t) noexcept;

    /**
     * @brief Renders the world
     * 
     * @param data the world data
     */
    static void software_renderWorld(void* data, uint64_t);

    //check for ImGui
    #if GLGE_3RD_PARTY_INCLUDE_DEAR_IMGUI

    /**
     * @brief start the ImGui frame
     */
    static void software_ImGui_StartFrame(void*, uint64_t) noexcept;

    /**
     * @brief end the ImGui frame
     */
    static void software_Imgui_EndFrame(void*, uint64_t) noexcept;

    /**
     * @brief execute a function that uses Dear ImGui
     * 
     * @param data the data used to execute the function
     */
    static void software_ImGui_Execute(void* data, uint64_t) noexcept;

    #endif //end of ImGui section
};

#endif

#endif