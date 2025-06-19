/**
 * @file GLGEBuffer.h
 * @author DM8AT
 * @brief define a buffer for arbituary GPU data
 * @version 0.1
 * @date 2025-06-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_BUFFER_
#define _GLGE_BUFFER_

//include the settings
#include "../GLGECore/GLGEInstance.h"

//add graphic instances
#include "../GLGECore/GLGEInstance.h"

//for the GPU access - add Graphic Memory arenas
#include "GraphicAPI/GLGEGraphicAPI_MemoryArena.h"

//check for C++
#if GLGE_CPP

/**
 * @brief store a reagion of memory accessable for rendering
 */
class Buffer : public InstAttachableClass
{
public:

    /**
     * @brief Disallow a creation of a buffer without an instance
     */
    Buffer() = delete;

    /**
     * @brief Construct a new Buffer
     * 
     * @param instance a reference to the instance the buffer will belong to
     */
    Buffer(Instance& instance);

    /**
     * @brief Construct a new Buffer
     * 
     * @param data the inital data for the buffer (set to NULL to fill it with zeros)
     * @param size the size in bytes of the buffer
     * @param instance a reference to the instance the buffer will belong to
     */
    Buffer(void* data, uint64_t size, Instance& instance);

    /**
     * @brief Destroy the Buffer
     */
    inline ~Buffer() noexcept {m_arena = 0;}

    /**
     * @brief Get the size of the memory arena
     * 
     * @return const uint64_t& the size of the memory arena (CAN be 0)
     */
    inline uint64_t getSize() const noexcept {return (m_arena) ? m_arena->getSize() : 0;}

    /**
     * @brief change the size of the buffer
     * 
     * @param newSize the new size of the buffer
     */
    void resize(uint64_t newSize) noexcept {
        //pass the resize call
        if (m_arena) {m_arena->resize(newSize);}
    }

    /**
     * @brief set some arbituary data in the buffer
     * 
     * @param data the data for the section
     * @param size the size of the section
     * @param offset the offset of the section
     * 
     * @return true : successfully set the data
     * @return false : failed to set the data. The data would be out of range. 
     */
    bool set(void* data, uint64_t size, uint64_t offset) noexcept;

protected:

    /**
     * @brief store the own memory arena
     */
    GraphicMemoryArena* m_arena = 0;

};

/**
 * @brief store some data that is formated in a specific way
 * 
 * @tparam T the type of the data to store
 */
template <typename T> class StructuredBuffer : public Buffer
{
public:

    /**
     * @brief Disallow a creation of a structured buffer without an instance
     */
    StructuredBuffer() = delete;

    /**
     * @brief Construct a new Structured Buffer
     * 
     * @param instance a reference to the instance the buffer will belong to
     */
    StructuredBuffer(Instance& instance)
     : Buffer(instance) {}

    /**
     * @brief Construct a new Structured Buffer
     * 
     * @param elements the amount of elements in the array
     * @param instance a reference to the instance the buffer will belong to
     */
    StructuredBuffer(uint64_t elements, Instance& instance)
     : Buffer(0, elements*sizeof(T), instance) {}

    /**
     * @brief Construct a new Buffer
     * 
     * @param data an array of instances of the T class
     * @param count the amount of instance of the type T in the data array
     * @param instance a reference to the instance the buffer will belong to
     */
    StructuredBuffer(T* data, uint64_t count, Instance& instance)
     : Buffer(data, count * sizeof(T), instance) {}

    /**
     * @brief write some element of the structured buffer
     * 
     * @param array an array of elements to write
     * @param count the amount of elements to write
     * @param offset the element offset of the start of the array in the buffer
     * 
     * @return true : the data was set successfully
     * @return false : the data would be out of range of the buffer
     */
    bool set(T* array, uint64_t count, uint64_t offset) noexcept
    {
        //check if the data is in bounds
        if ((count + offset) * sizeof(T) >= m_arena->getSize()) {return false;}

        //update the actual data
        m_arena->update({offset*sizeof(T), count*sizeof(T)}, array);

        //successfully set the data
        return true;
    }

    /**
     * @brief set a single element in the structured buffer
     * 
     * @param element the element to set the data to
     * @param index the index of the element to set
     * 
     * @return true : the element was updated successfully
     * @return false : the element would be out of range of the buffer
     */
    bool set(const T& element, uint64_t index) noexcept
    {
        //check if the data is in bounds
        if ((index + 1) * sizeof(T) >= m_arena->getSize()) {return false;}

        //update the actual element
        m_arena->update({index*sizeof(T), sizeof(T)}, &element);

        //succesfully set the element
        return true;
    }

};

#endif //end of C++ section

#endif