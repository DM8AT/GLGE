/**
 * @file GLGE_OGL4_6_MemoryArena.h
 * @author DM8AT
 * @brief overload a memory arena for GPU access using OpenGL 4.6
 * @version 0.1
 * @date 2025-06-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_OGL_4_6_MEMORY_ARENA_

//include memory arenas
#include "../../GLGEGraphicAPI_MemoryArena.h"

//check for C++
#if GLGE_CPP

class OGL4_6_MemoryArena : public GraphicMemoryArena
{
public:

    /**
     * @brief Construct a new memory arena for OpenGL
     * 
     * @param size the size of the arena in bytes
     * @param allowResize spezify if dynamic resizing is allowed
     * @param usage specify how the memory reagion will be used
     * @param instance a reference to the instance the graphic memory will belong to
     */
    OGL4_6_MemoryArena(uint64_t size, bool allowResize, MemoryUsage usage, Instance& instance)
     : GraphicMemoryArena(size, allowResize, usage, instance)
    {}

    /**
     * @brief a hook that is called when the memory arena is created
     */
    virtual void onCreate() noexcept;

    /**
     * @brief a hook that is called when the memory arena is destroyed
     */
    virtual void onDestroy() noexcept;

protected:

    /**
     * @brief update all changed sections of GPU memory
     */
    virtual void gpuUpdate() noexcept;

    /**
     * @brief register that the size of the whole arena changed
     */
    virtual void gpuSizeChanged() noexcept;

    /**
     * @brief queue the size change
     * 
     * @param data the data for the execution of the size change
     */
    static void ogl_sizeChange(void* data, uint64_t) noexcept;
    /**
     * @brief store a mutex to stop multiple resize events from being queued simoultaniously
     */
    std::mutex m_resizeMutex;

    /**
     * @brief queue the content update
     * 
     * @param data the data for the execution of the content update (NOT the content to write)
     */
    static void ogl_contentUpdate(void* data, uint64_t) noexcept;

    /**
     * @brief queue the creation of the GPU side buffer
     * 
     * @param data the data for the buffer creation
     */
    static void ogl_create(void* data, uint64_t) noexcept;

    /**
     * @brief queue the destruction of the GPU side buffer
     * 
     * @param data the data for the buffer destruction
     */
    static void ogl_destroy(void* data, uint64_t) noexcept;

    /**
     * @brief store thee OpenGL buffer
     */
    uint32_t m_buff = 0;
    /**
     * @brief store the binding point the buffer will bind to (0 = undefined)
     */
    uint32_t m_bindingType = 0;

};

#endif //end of C++ reagion

#endif