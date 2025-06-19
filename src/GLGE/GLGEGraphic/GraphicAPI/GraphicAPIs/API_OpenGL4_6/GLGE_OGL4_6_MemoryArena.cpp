/**
 * @file GLGE_OGL4_6_MemoryArena.cpp
 * @author DM8AT
 * @brief implement the overloads and static functions for OpenGL memory arenas
 * @version 0.1
 * @date 2025-06-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include OpenGL 4.6 memory arenas
#include "GLGE_OGL4_6_MemoryArena.h"
//add instances to access the buffers
#include "GLGE_OGL4_6_Instance.h"
//add command buffers to use command buffers
#include "GLGE_OGL4_6_CommandBuffer.h"

//add OpenGL
#include <GL/glew.h>

void OGL4_6_MemoryArena::onCreate() noexcept
{
    //lock the memory arena to stop threads from interfearing with construction
    lock();

    //queue the object creation
    m_graphicInstance->getInstance()->getGraphicInstance()->getBuffers()[0]->add(0, (void*)ogl_create, this, sizeof(this));

    //queuing done, continue
    unlock();
}

void OGL4_6_MemoryArena::onDestroy() noexcept
{
    //make sure that there is no interfearance from other threads
    lock();

    //queue the object destruction
    m_graphicInstance->getInstance()->getGraphicInstance()->getBuffers()[0]->add(0, (void*)ogl_destroy, this, sizeof(this));

    //no need to unlock. To make sure no threads interfear in the destruction, the object will remain locked untill the destruction finishes
}

void OGL4_6_MemoryArena::gpuUpdate() noexcept
{
    //queue the data update command
    m_graphicInstance->getInstance()->getGraphicInstance()->getBuffers()[0]->add(0, (void*)ogl_contentUpdate, this, sizeof(this));
}

void OGL4_6_MemoryArena::gpuSizeChanged() noexcept
{
    //queue the size change
    m_graphicInstance->getInstance()->getGraphicInstance()->getBuffers()[0]->add(0, (void*)ogl_sizeChange, this, sizeof(this));
    //lock the mutex for resizing
    m_resizeMutex.lock();
}

void OGL4_6_MemoryArena::ogl_sizeChange(void* data, uint64_t) noexcept
{
    //extract the pointer to the object
    OGL4_6_MemoryArena& arena = *((OGL4_6_MemoryArena*)data);

    //construct a new, temporary buffer
    uint32_t buff;
    glGenBuffers(1, &buff);

    //write the data into it, with the new size
    glBindBuffer(arena.m_bindingType, buff);
    glBufferData(arena.m_bindingType, arena.m_size, arena.m_data, GL_STREAM_COPY);

    //swap this with the old buffer and delete the old one
    glDeleteBuffers(1, &arena.m_buff);
    arena.m_buff = buff;

    //unlock the resizing mutex to allow another resize command
    arena.m_resizeMutex.unlock();
}

void OGL4_6_MemoryArena::ogl_contentUpdate(void* data, uint64_t) noexcept
{
    //extract the pointer to the object
    OGL4_6_MemoryArena& arena = *((OGL4_6_MemoryArena*)data);

    //early out if no changes are stored (how?)
    if (arena.m_changed.size() == 0) {return;}

    //pull the data reagion to update
    const GraphicPointer& ptr = arena.m_changed[0];
    //update the data
    glBindBuffer(arena.m_bindingType, arena.m_buff);
    //map the data to the CPU
    void* dat = glMapBufferRange(arena.m_bindingType, ptr.startIdx, ptr.size, GL_MAP_WRITE_BIT);
    //write the data
    memcpy(dat, &(((uint8_t*)arena.m_data)[ptr.startIdx]), ptr.size);
    //unmap the buffer
    glUnmapBuffer(arena.m_bindingType);
    //remove the written element from the buffer
    arena.m_changed.erase(arena.m_changed.begin());
}

void OGL4_6_MemoryArena::ogl_create(void* data, uint64_t) noexcept
{
    //no need for thread safty here, only the graphic thread reads and writes to:
    // - m_bindingType
    // - m_buff

    //extract the pointer to the object
    OGL4_6_MemoryArena& arena = *((OGL4_6_MemoryArena*)data);
    //construct a new buffer for the data
    glGenBuffers(1, &arena.m_buff);

    //determine the buffer's binding point
    switch (arena.m_usage)
    {
    case MEMORY_USAGE_UNKNOWN:
        //just use shader storage buffers
        arena.m_bindingType = GL_SHADER_STORAGE_BUFFER;
        break;
    //specify for shader storage buffers
    case MEMORY_USAGE_READ:
    case MEMORY_USAGE_WRITE:
    case MEMORY_USAGE_READ_WRITE:
        arena.m_bindingType = GL_SHADER_STORAGE_BUFFER;
        break;
    //uniform buffers are declared directly
    case MEMORY_USAGE_UNIFORM:
        arena.m_bindingType = GL_UNIFORM_BUFFER;
        break;
    
    default:
        //log that something went wront
        arena.m_graphicInstance->getInstance()->log("Failed to determine correct buffer type for a OpenGL 4.6 Buffer", MESSAGE_TYPE_ERROR);
        return;
        break;
    }

    //bind the buffer to the requested target
    glBindBuffer(arena.m_bindingType, arena.m_buff);
    //fill the buffer with the current data. As usage, assume dynamic read
    glBufferData(arena.m_bindingType, arena.m_size, arena.m_data, GL_DYNAMIC_READ);
}

void OGL4_6_MemoryArena::ogl_destroy(void* data, uint64_t) noexcept
{
    //extract the data
    OGL4_6_MemoryArena& arena = *((OGL4_6_MemoryArena*)data);

    //delete the OpenGL buffer object
    glDeleteBuffers(1, &arena.m_buff);

    //unlock the buffer to continue destruction
    arena.unlock();
}