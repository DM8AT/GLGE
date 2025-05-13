/**
 * @file GLGEGraphicAPI_RenderPipeline.h
 * @author DM8AT
 * @brief store the graphic API dependend render pipeline implementation
 * @version 0.1
 * @date 2025-05-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef _GLGE_GRAPHIC_API_RENDER_PIPELINE_
#define _GLGE_GRAPHIC_API_RENDER_PIPELINE_

//include the settings
#include "../../GLGECore/GLGESettings.h"

/**
 * @brief specify the type of render stage
 */
typedef enum e_StageType {
    /**
     * @brief specify that the render stage should do nothing but call the functions. No data required. 
     */
    RENDER_STAGE_NONE,
    /**
     * @brief specify that the render stage should clear the specified framebuffer. The data must contain a pointer to a framebuffer. 
     */
    RENDER_STAGE_CLEAR,
    /**
     * @brief specify that the render stage should copy one framebuffer to another. The data must contain two framebuffers. The first will be used as source, the second as destination. 
     */
    RENDER_STAGE_BLIT,
    /**
     * @brief specify that the render stage should copy a framebuffer to a window. The data must contain first a framebuffer to use as a source and then a window to use as a destination. 
     */
    RENDER_STAGE_BLIT_TO_WINDOW,
    /**
     * @brief specify that the render stage should copy the contents from a window to a framebuffer. The data must contain first a window to use as source and then a framebuffer to use as destination. 
     */
    RENDER_STAGE_BLIT_FROM_WINDOW,
    /**
     * @brief specify that the render stage should render all objects in a world. The data must contain the world to render, an Object to use as Camera and a target framebuffer. The Object must contain a component like a camera or Light source. It must also specify if depth or color should be renderd.
     */
    RENDER_STAGE_RENDER_WORLD,
    /**
     * @brief specify that the render stage should execute a compute shader. The data must contain the compute shader to dispatch as well as a 3D unsigned integer vector defining how many elements to dispatch. 
     */
    RENDER_STAGE_COMPUTE,
    /**
     * @brief specify that the render stage should apply a post processing effect to a framebuffer. The data must contain the framebuffer and the post processing effect
     */
    RENDER_STAGE_POST_PROCESSING
} StageType;

//check if this is C++
#if GLGE_CPP

//define that render pipelines will exist
class RenderPipeline;

/**
 * @brief store a wrapper around the execution of render pipeline stages
 */
class GraphicRenderPipeline
{
public:

    /**
     * @brief Construct a new Graphic Render Pipeline
     */
    GraphicRenderPipeline() = default;

    /**
     * @brief Construct a new Graphic Render Pipeline
     * 
     * @param renderPipeline a pointer to the parent render pipeline
     */
    GraphicRenderPipeline(RenderPipeline* renderPipeline) : m_renderPipeline(renderPipeline) {}

    /**
     * @brief Destroy the Graphic Render Pipeline
     */
    virtual ~GraphicRenderPipeline() {onDestroy(); m_renderPipeline = 0;}

    /**
     * @brief Get all supported stage types
     * 
     * @param types a reference to a vector that will be filled with all supported types of stages
     */
    virtual void getSupportedStageTypes(std::vector<StageType>& types) const noexcept {types.clear();}

    /**
     * @brief a function to set up the render pipeline object
     */
    virtual void onAttatch() noexcept {}

    /**
     * @brief a function to clean up the render pipeline
     */
    virtual void onDestroy() noexcept {}

    /**
     * @brief a function that is called when one stage is executed
     * 
     * @param stageIndex the index of the executed stage
     */
    virtual void onStageExecution(uint64_t) noexcept {}

protected:

    /**
     * @brief store the parent render pipeline
     */
    RenderPipeline* m_renderPipeline = 0;

};

#endif

#endif