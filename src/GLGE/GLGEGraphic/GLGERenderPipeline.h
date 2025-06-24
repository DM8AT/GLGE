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
//include windows and limiter
#include "GLGEWindow.h"
//include shader
#include "Shader/GLGEShader.h"
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
    Framebuffer_t* framebuffer;

    //check for C++ to create a constructor
    #if GLGE_CPP
    /**
     * @brief Construct a new instance of data for a clear pass 
     * 
     * @param _framebuffer a pointer to the framebuffer to clear
     */
    s_ClearStageData(Framebuffer* _framebuffer) : framebuffer((Framebuffer_t*)_framebuffer) {}
    #endif //C++ section
} ClearStageData;

/**
 * @brief store the data that is used to change the size of a framebuffer
 */
typedef struct s_ResizeFramebufferData {
    /**
     * @brief store a pointer to the framebuffer to change the size of
     */
    Framebuffer_t* framebuffer;
    /**
     * @brief store the new size for the framebuffer
     */
    uvec2 size;

    //check for C++ to create a constructor
    #if GLGE_CPP
    s_ResizeFramebufferData(Framebuffer* _fbuff, const uvec2& _size)
     : framebuffer((Framebuffer_t*)_fbuff), size(_size)
    {}
    #endif //end of C++ section

} ResizeFramebufferData;

/**
 * @brief store the data for a blit stage
 */
typedef struct s_BlitStageData {
    /**
     * @brief store the source framebuffer
     */
    Framebuffer_t* src;
    /**
     * @brief store the destinatoin framebuffer
     */
    Framebuffer_t* dst;

    //check for C++ to create a constructor
    #if GLGE_CPP

    /**
     * @brief Construct a new instance of data for a blit pass
     * 
     * @param _src the framebuffer to blit from
     * @param _dst the framebuffer to blit to
     */
    s_BlitStageData(Framebuffer* _src, Framebuffer* _dst)
     : src((Framebuffer_t*)_src), dst((Framebuffer_t*)_dst) {}

    #endif //C++ section
} BlitStageData;

/**
 * @brief store the data for copying the content of a framebuffer to a window
 */
typedef struct s_BlitToWindowStageData {
    /**
     * @brief store the framebuffer to copy from
     */
    Framebuffer_t* src;
    /**
     * @brief store the window to copy to
     */
    Window_t* dst;

    //check for C++ to create a constructor
    #if GLGE_CPP

    /**
     * @brief copy the content of a framebuffer to a window's back buffer
     * 
     * @param _src the framebuffer to copy the data from
     * @param _dst the window to write the data to
     */
    s_BlitToWindowStageData(Framebuffer* _src, Window* _dst)
     : src((Framebuffer_t*)_src), dst((Window_t*)_dst) {}

    #endif //end of C++ section

} BlitToWindowStageData;

/**
 * @brief store the data for copying the content from a window to a framebuffer
 */
typedef struct s_BlitFromWindowStageData {
    /**
     * @brief store a pointer to the source window
     */
    Window_t* src;
    /**
     * @brief store a pointer to the destination framebuffer
     */
    Framebuffer_t* dst;

    //check for C++ to create a constructor
    #if GLGE_CPP

    /**
     * @brief Construct a new instance of data to copy the content of a window's back buffer to a framebuffer
     * 
     * @param _src a pointer to the window to copy the data from
     * @param _dst a pointer to the framebuffer to copy the data to 
     */
    s_BlitFromWindowStageData(Window* _src, Framebuffer* _dst)
     : src((Window_t*)_src), dst((Framebuffer_t*)_dst) {}

    #endif //end of C++ section

} BlitFromWindowStageData;

/**
 * @brief store the data for a swap command
 */
typedef struct s_SwapWindowData {
    /**
     * @brief store a pointer to the window to swap
     */
    Window_t* window;

    //check for C++ to create a constructor
    #if GLGE_CPP

    /**
     * @brief Construct a new instance of data for swapping a window
     * 
     * @param _window a pointer to the window to swap
     */
    s_SwapWindowData(Window* _window) : window((Window_t*)_window) {}

    #endif //end of C++ section

} SwapWindowData;

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
 * @brief store the data for a compute shader execution
 */
typedef struct s_ComputeStageData {
    /**
     * @brief store the shader to execute
     */
    void* shader;
    /**
     * @brief store the amount for executions
     */
    uvec3 executions;

    //check for C++
    #if GLGE_CPP

    /**
     * @brief Store a new pack of data for a compute shader execution
     * 
     * @param _shader a reference to the shader to execute (must be a compute shader)
     * @param _executions the amount of thread groups to execute per axis
     */
    s_ComputeStageData(Shader& _shader, const uvec3& _executions)
     : shader((void*)(&_shader)), executions(_executions)
    {}

    #endif //end of C++ section

} ComputeStageData;

//check if a Dear ImGui binding should be included
#if GLGE_3RD_PARTY_INCLUDE_DEAR_IMGUI

/**
 * @brief store a stage to handle a Dear ImGui drawing
 */
typedef struct s_DearImGuiStageData {

    /**
     * @brief the function to call for the Dear ImGui implementation
     */
    void (*imGuiFunction)(void*, uint64_t);
    /**
     * @brief store the data the function will recive
     */
    void* data;
    /**
     * @brief store the data size (can be literally anything, just mostly the size) that is given to the function
     */
    uint64_t size;

    //check for C++ to create the constructor
    #if GLGE_CPP

    /**
     * @brief Construct a new pack of data for a ImGui construction
     * 
     * @param _func a pointer to the function to execute ImGui in
     * @param _data a pointer to some arbituary data to be given to the function
     * @param _size the size of the data
     */
    s_DearImGuiStageData(void (*_func)(void*, uint64_t), void* _data, uint64_t _size) : imGuiFunction(_func), data(_data), size(_size) {}

    #endif //C++ section

} DearImGuiStageData;

#endif //end of the Dear ImGui binding

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
        //store the data to change a framebuffer's size
        ResizeFramebufferData resizeFramebuffer;
        //store the data for a blit stage
        BlitStageData blit;
        //store the data for a stage that copys to a window
        BlitToWindowStageData toWindow;
        //store the data for a stage that copys from a window
        BlitFromWindowStageData fromWindow;
        //store the data used to swap a specific window
        SwapWindowData swapWindow;
        //store the data for a stage that renders all objects from a world
        RenderWorldStageData renderWorld;
        //store the data for a stage that executions a compute shader
        ComputeStageData compute;
        //store the data for the dear imgui stage to execute a function
        DearImGuiStageData imguiFunc;

        //check for C++ to create a constructor
        #if GLGE_CPP

        /**
         * @brief Construct some new data
         * @warning the data is just garbage
         */
        Data() : clear(0) {}

        /**
         * @brief Construct some new data
         * 
         * @param _clear a constant reference to the clear data to store
         */
        Data(const ClearStageData& _clear) : clear(_clear) {}

        /**
         * @brief Construct some new data
         * 
         * @param _changeFbuffSize a constant reference to the data to change a framebuffer's size
         */
        Data(const ResizeFramebufferData& _changeFbuffSize) : resizeFramebuffer(_changeFbuffSize) {}

        /**
         * @brief Construct some new data
         * 
         * @param _blit a constant reference to the blit data to store
         */
        Data(const BlitStageData& _blit) : blit(_blit) {}

        /**
         * @brief Construct some new data
         * 
         * @param _toWindow a constant reference to the blit data to blit to a window
         */
        Data(const BlitToWindowStageData& _toWindow) : toWindow(_toWindow) {}

        /**
         * @brief Construct some new data
         * 
         * @param _fromWindow a constant reference to the blit data to blit from a window
         */
        Data(const BlitFromWindowStageData& _fromWindow) : fromWindow(_fromWindow) {}

        /**
         * @brief Construct some new data
         * 
         * @param _swapWindow a constant reference to the data used to swap the window
         */
        Data(const SwapWindowData& _swapWindow) : swapWindow(_swapWindow) {}

        /**
         * @brief Construct some new data
         * 
         * @param _renderWorld a constant reference to the data to render a world instance
         */
        Data(const RenderWorldStageData& _renderWorld) : renderWorld(_renderWorld) {}

        /**
         * @brief Construct some new data
         * 
         * @param _compute a constant refenrence to the data to execute a compute shader
         */
        Data(const ComputeStageData& _compute) : compute(_compute) {}

        /**
         * @brief Construct some new data
         * 
         * @param _imguiFunc a constant refenrence to the data to execute a imgui function stage
         */
        Data(const s_DearImGuiStageData& _imguiFunc) : imguiFunc(_imguiFunc) {}

        #endif //end of C++ section
    } data;

    //check for C++ to add constructors
    #if GLGE_CPP

    /**
     * @brief Construct a new render stage
     */
    s_RenderStage() = default;

    /**
     * @brief Construct a new render stage
     * 
     * @param _type the type of the render stage
     * @param _data the data for the render stage. Must be the type spezified by the render stage
     * @param _userData a pointer to some user data, can be 0
     * @param _before a pointer to a function that will be executed before the stage is executed. Can be 0. 
     * @param _after a pointer to a function that will be executed after the stage is executed. Can be 0. 
     */
    s_RenderStage(StageType _type, Data _data, void* _userData, void (*_before)(void*), void (*_after)(void*)) 
     : before(_before), after(_after), type(_type), userData(_userData), data(_data) {}

    #endif //end of C++ section

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
     * @param start define if the render pipeline should start imediatly after creation
     * @param instance the instance the render pipeline will belong to
     */
    RenderPipeline(RenderStage* stages, uint64_t stageCount, bool start, Instance& instance);

    /**
     * @brief Destroy the Render Pipeline
     */
    ~RenderPipeline();

    /**
     * @brief start the render pipeline if it isn't running
     */
    void start();

    /**
     * @brief stop the render pipeline if it is running
     */
    void stop();

    /**
     * @brief get if the render pipeline is currently running
     * 
     * @return true : the render pipeline is running
     * @return false : the render pipeline is not running
     */
    inline bool isRunning() const noexcept {return m_run;}

    /**
     * @brief Get a specific render stage
     * 
     * @param index the index of the stage to access
     * @return RenderStage& a reference to the render stage
     */
    inline RenderStage& getStage(uint64_t index) noexcept {return m_stages[index];}

    /**
     * @brief Get the amount of stages in the stage array
     * 
     * @return uint64_t the amount of single stages
     */
    inline uint64_t getStageCount() const noexcept {return m_stages.size();}

    /**
     * @brief Get the limiter instance responsible for controlling the iteration rate
     * 
     * @return Limiter& a reference to the limiter instance
     */
    inline Limiter& getLimiter() noexcept {return m_limiter;}

protected: 

    /**
     * @brief execute a render pipeline by executing all stages
     * 
     * @param pipeline a pointer to the pipeline to exeucte
     */
    static void execute(RenderPipeline* pipeline) noexcept;

    /**
     * @brief store a pointer to the execution layer to execute the render stages
     */
    GraphicRenderPipeline* m_renderPipeline = 0;
    /**
     * @brief store all render stages for the pipeline
     */
    std::vector<RenderStage> m_stages = {};
    /**
     * @brief store if the render pipeline should run
     */
    bool m_run = false;
    /**
     * @brief store the thread the render pipeline runs upon
     */
    std::thread m_thread;
    /**
     * @brief store a limiter to define the iteration rate of the pipeline's execution
     */
    Limiter m_limiter = 60;

};

#endif //C++ section

#endif