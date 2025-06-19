/**
 * @file GLGE_OGL4_6_RenderPipeline.h
 * @author DM8AT
 * @brief define an overload for OpenGL 4.6 of the render pipeline
 * @version 0.1
 * @date 2025-06-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_OGL_4_6_RENDER_PIPELINE_
#define _GLGE_OGL_4_6_RENDER_PIPELINE_

//include graphic render pipelines
#include "../../GLGEGraphicAPI_RenderPipeline.h"
//include render pipelines
#include "../../../GLGERenderPipeline.h"

//check for C++
#if GLGE_CPP

/**
 * @brief store a render pipeline 
 */
class OGL4_6_RenderPipeline : public GraphicRenderPipeline
{
public:

    /**
     * @brief Construct a new OpenGL 4.6 render pipeline 
     * 
     */
    OGL4_6_RenderPipeline() = default;

    /**
     * @brief Construct a new OpenGL 4.6 render pipeline
     * 
     * @param renderPipeline a pointer to the high level render pipeline object
     */
    OGL4_6_RenderPipeline(RenderPipeline* renderPipeline) : GraphicRenderPipeline(renderPipeline) {}

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
    static void ogl_blitToWindow(void* data, uint64_t) noexcept;

    /**
     * @brief execute a compute shader
     * 
     * @param data the data to execute a compute shader
     */
    static void ogl_executeCompute(void* data, uint64_t) noexcept;
};

#endif

#endif