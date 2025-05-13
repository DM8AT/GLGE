/**
 * @file GLGERenderPipeline.h
 * @author DM8AT
 * @brief define the layout of a render pipeline for GLGE
 * @version 0.1
 * @date 2025-04-18
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_RENDER_PIPELINE_
#define _GLGE_RENDER_PIPELINE_

//include settings
#include "../GLGECore/GLGESettings.h"
//include instances
#include "../GLGECore/GLGEInstance.h"
//include framebuffers
#include "GLGEFramebuffer.h"
//include windows
#include "GLGEWindow.h"
//include geometry (Objects and Worlds)
#include "../GLGECore/Geometry/GLGEGeometry.h"

//include the API interface for the render pipeline
#include "GraphicAPI/GLGEGraphicAPI_RenderPipeline.h"

/**
 * @brief store the data for a clear stage
 */
typedef struct s_ClearStageData {
    /**
     * @brief store a pointer to the framebuffer
     */
    void* framebuffer;
} ClearStageData;

/**
 * @brief store the data for a blit stage
 */
typedef struct s_BlitStageData {
    /**
     * @brief store the source framebuffer
     */
    void* src;
    /**
     * @brief store the destinatoin framebuffer
     */
    void* dst;
} BlitStageData;

/**
 * @brief store the data for copying the content of a framebuffer to a window
 */
typedef struct s_BlitToWindowStageData {
    /**
     * @brief store the framebuffer to copy from
     */
    void* src;
    /**
     * @brief store the window to copy to
     */
    void* dst;
} BlitToWindowStageData;

/**
 * @brief store the data for copying the content from a window to a framebuffer
 */
typedef struct s_BlitFromWindowStageData {
    /**
     * @brief store a pointer to the source window
     */
    void* src;
    /**
     * @brief store a pointer to the destination framebuffer
     */
    void* dst;
} BlitFromWindowStageData;

/**
 * @brief store the data for rendering a world class instance
 */
typedef struct s_RenderWorldStageData {
    /**
     * @brief store a pointer to the world to render
     */
    void* world;
    /**
     * @brief store a pointer to an object to render from
     * @warning the Object must contain a Camera or Light object attatchment
     */
    void* view;
    /**
     * @brief store the framebuffer that is renderd to
     */
    void* target;
    /**
     * @brief store if a depth or a color pass is issued
     */
    bool depth;
} RenderWorldStageData;

/**
 * @brief define the structure of a render stage
 */
typedef struct s_RenderStage {
    /**
     * @brief store a function that is called before this stage is executed. 0 means that no function will be called. Inputed is the user data. 
     */
    void (*before)(void*);
    /**
     * @brief store a function that is called after this stage is executed. 0 means that no function will be called. Inputed is the user data. 
     */
    void (*after)(void*);
    /**
     * @brief store the purpose of this stage
     */
    StageType type;
    /**
     * @brief store some arbituary user data
     */
    void* userData;
    /**
     * @brief store some data that might be required for the render stage
     */
    union Data {
        //store the data for a clear stage
        ClearStageData clear;
        //store the data for a blit stage
        BlitStageData blit;
        //store the data for a stage that copys to a window
        BlitToWindowStageData toWindow;
        //store the data for a stage that copys from a window
        BlitFromWindowStageData fromWindow;
        //store the data for a stage that renders all objects from a world
        RenderWorldStageData renderWorld;
    } data;
} RenderStage;

//check if this is C++
#if GLGE_CPP

/**
 * @brief store a list of stages to execute for rendering
 */
class RenderPipeline : public InstAttachableClass
{
public:

    /**
     * @brief Construct a new Render Pipeline
     */
    RenderPipeline() = default;

    /**
     * @brief Construct a new Render Pipeline
     * 
     * @param stages an array of stages for the render pipeline in the order of execution
     * @param stageCount the amount of stages in the render stage array
     * @param instance the instance the render pipeline will belong to
     */
    RenderPipeline(RenderStage* stages, uint64_t stageCount, Instance& instance);

    /**
     * @brief Destroy the Render Pipeline
     */
    ~RenderPipeline();

    /**
     * @brief Get a specific render stage
     * 
     * @param index the index of the stage to access
     * @return RenderStage& a reference to the render stage
     */
    inline RenderStage& getStage(uint64_t index) noexcept {return m_stages[index];}

    /**
     * @brief run all stages of the render pipeline
     */
    void execute() noexcept;

protected: 

    /**
     * @brief store a pointer to the execution layer to execute the render stages
     */
    GraphicRenderPipeline* m_renderPipeline = 0;
    /**
     * @brief store all render stages for the pipeline
     */
    std::vector<RenderStage> m_stages;

};

#endif //C++ section

#endif