/**
 * @file GLGEBuffer.cpp
 * @author DM8AT
 * @brief implement the buffer for GLGE
 * @version 0.1
 * @date 2025-06-16
 * 
 * @copyright Copyright (c) 2025
 */

//include buffers
#include "GLGEBuffer.h"

//include all APIs
#include "GraphicAPI/GraphicAPIs/GLGE_AllAPIs.h"

Buffer::Buffer(Instance& instance)
 : InstAttachableClass(&instance, "buffer")
{
    //switch over the graphic API
    switch (m_instance->getAPI())
    {
    case API_OPENGL_4_6:
        //create an opengl 4.6 memory arena
        m_arena = new OGL4_6_MemoryArena(0, false, MEMORY_USAGE_READ_WRITE, instance);
        break;
    
    default:
        break;
    }

    //check if the arena was successfully created
    if (!m_arena) {m_instance->log("Failed to create a new memory arena for a buffer. Out of memory?", MESSAGE_TYPE_ERROR); return;}

    //initalize the memory arena
    m_arena->onCreate();
}

Buffer::Buffer(void* data, uint64_t size, Instance& instance)
 : InstAttachableClass(&instance, "buffer")
{
    //switch over the graphic API
    switch (m_instance->getAPI())
    {
    case API_OPENGL_4_6:
        //create an opengl 4.6 memory arena
        m_arena = new OGL4_6_MemoryArena(size, false, MEMORY_USAGE_READ_WRITE, instance);
        break;
    
    default:
        break;
    }

    //check if the arena was successfully created
    if (!m_arena) {m_instance->log("Failed to create a new memory arena for a buffer. Out of memory?", MESSAGE_TYPE_ERROR); return;}

    //initalize the memory arena
    m_arena->onCreate();

    //if data exists, update it
    if (data)
    {
        //write the data to the memory arena
        m_arena->update({0, size}, data);
    }
}

bool Buffer::set(void* data, uint64_t size, uint64_t offset) noexcept
{
    //quick bounds check
    if ((size + offset) >= m_arena->getSize()) {return false;}

    //update the data
    m_arena->update({offset, size}, data);

    //successfully updated data
    return true;
}