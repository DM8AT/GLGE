/**
 * @file GLGEGraphicAPI_MemoryArena.cpp
 * @author DM8AT
 * @brief implement the default functionality of a memory arena
 * @version 0.1
 * @date 2025-06-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the GPU memory arena
#include "GLGEGraphicAPI_MemoryArena.h"

GraphicMemoryArena::GraphicMemoryArena(uint64_t size, bool allowResize, MemoryUsage usage, Instance& instance) noexcept
 : GraphicInstAttatchable(instance.getGraphicInstance())
{
    //store the inputed size as the new size of the memory arena
    m_size = size;
    //store if resizing is allowed
    m_allowResize = allowResize;
    //store the memory reagion's usage
    m_usage = usage;
    //allocate the RAM data
    m_data = calloc(1, m_size);

    //check if the memory allocated successfully
    if (!m_data)
    {
        //error : calloc failed
        m_graphicInstance->getInstance()->log("Failed to allocate memory for memory arena", MESSAGE_TYPE_ERROR);
        return;
    }

    //store the new free reagion
    m_free.push_back({0, m_size});
}

GraphicMemoryArena::~GraphicMemoryArena()
{
    //check if the memory arena is set up. Stop if it is not. 
    if (!m_data) {return;}

    //lock the data
    lock();

    //only print if this is not an API arena
    if (!m_isAPI)
    {
        //check if there is space in the memory arena
        if (m_free.size() == 0)
        {
            //no space. Print a warning
            m_graphicInstance->getInstance()->log("Freed a full graphic memory arena", MESSAGE_TYPE_WARNING);
        }
        else
        {
            //there's space. Check if the whole space is free. 
            if (m_free[0].size != m_size)
            {
                //not everything was freed. Print a warning
                m_graphicInstance->getInstance()->log("Freed a graphic memory arena that still contains data", MESSAGE_TYPE_WARNING);
            }
        }
    }

    //clear all vectors
    m_free.clear();
    m_changed.clear();
    //free the RAM data
    free(m_data);
    m_data = 0;
    //say that no data is stored
    m_size = 0;
    //unlock for deletion
    unlock();
}

GraphicMemoryArena::GraphicPointer GraphicMemoryArena::allocate(uint64_t size) noexcept
{
    //if the instance is not set, return a null pointer
    if (!m_graphicInstance) {return {0,0};}
    //iterate over all free reagions
    for (uint64_t i = 0; i < m_free.size(); ++i)
    {
        //check if the free area is sufficent
        if (m_free[i].size >= size)
        {
            //store the index
            uint64_t idx = m_free[i].startIdx;

            //push the found reagion forward by the size and remove the now allocated reagion
            m_free[i].startIdx += size;
            m_free[i].size -= size;

            //check if the size is 0 to save RAM by deleting the reagion
            if (m_free[i].size == 0)
            {
                //erase the reagion
                m_free.erase(m_free.begin()+i);
            }

            //return the found pointer
            return {idx, size};
        }
    }

    //before resizing, check if resizing is allowed
    if (!m_allowResize)
    {
        //if not, log an error and stop
        m_graphicInstance->getInstance()->log("Dynamic resizing is not allowed and a memory arena ran out of space", MESSAGE_TYPE_ERROR);
        return {0,0};
    }

    //store if the last element should be extended
    bool m_extend = m_free.size() > 0;
    //check if extend is currently possible
    if (m_extend)
    {
        //check if it is also the correct one
        m_extend = ((m_free[m_free.size()-1].startIdx + m_free[m_free.size()-1].size) == m_size);
    }
    //not enough free space is found. Append or grow the end of the array to fit the data
    if (m_extend)
    {
        //the last element reaches to the end of the array. Just increase the size. 

        //calculate the new size
        uint64_t nSize = m_size + (size - m_free[m_free.size()-1].size);

        //resize the memory arena. If it failes, return a null pointer
        if (!resize(nSize)) {return {0,0};}

        //store the index of the last element
        uint64_t idx = m_free[m_free.size()-1].startIdx;
        //erase the last element. It is now full. 
        m_free.erase(m_free.end());

        //return the last element
        return {idx, size};
    }
    else
    {
        //the last element is allready filled. Add a new element to the end. 

        //store the index as the current size
        uint64_t idx = m_size;
        //change the size by the missing size. If the resizing failes, return a null pointer
        if (!resize(m_size + size)) {return {0,0};}
        //return a new pointer to the end of the array
        return {idx, size};
    }
}

bool GraphicMemoryArena::release(GraphicPointer& ptr) noexcept
{
    //store the index the free section will be inserted at
    uint64_t insert = 0;
    //safty check if the requested area is not free
    //check all free segments for overlap with the pointer
    for (uint64_t i = 0; i < m_free.size(); ++i)
    {
        //get a reference to the current pointer
        const GraphicPointer& pointer = m_free[i];
        //check for overlap
        if (((pointer.startIdx < ptr.startIdx) && ((pointer.startIdx + pointer.size) <= ptr.startIdx)) || 
            ((pointer.startIdx > ptr.startIdx) && (pointer.startIdx >= (ptr.startIdx + ptr.size))))
        {
            //check if a next element exists
            if (i != m_free.size()-1)
            {
                //check if the element is in between this and the next element
                //it is allready enshured to not collide, so only check the pointer indices
                //collision with the next element is not checked, so make sure to add the length
                //of the reagion to the pointer to prevent overlap
                if ((pointer.startIdx < ptr.startIdx) && ((ptr.startIdx + ptr.size) <= m_free[i+1].startIdx))
                {
                    //store this as the insertion index
                    insert = i;
                }
            }
            continue;
        }

        //return the intersection
        return false;
    }

    //add the free section

    //first, check if a free section alligns with another free section
    //check if a section at this one exists (if free sections exist)
    if (m_free.size() != 0)
    {
        //check the section before this one
        //first, fetch the section
        GraphicPointer& pointer = m_free[insert];
        //now, check if the end section perfectly alligns with the start section
        if ((pointer.startIdx + pointer.size) == ptr.startIdx)
        {
            //just increase the size of the reagion before this one
            pointer.size += ptr.size;

            //check if the section should merge with the next one
            //first, check if a next one exists
            if (insert != m_free.size()-1)
            {
                //fetch the next section
                GraphicPointer& next = m_free[insert+1];
                //check if the end allignes with the next section
                if ((pointer.startIdx + pointer.size) == next.startIdx)
                {
                    //merge the sections by growing the first one by the size of the second one
                    pointer.size += next.size;
                    //erase the later section
                    m_free.erase(m_free.begin() + insert + 1);
                }
            }

            //successfully freed it
            return true;
        }

        //now, check if a section is after this one exists
        if (insert != m_free.size()-1)
        {
            //first, fetch the section
            GraphicPointer& next = m_free[insert+1];
            //check if the end of the to free pointer alligns with the start of the next one
            if ((ptr.startIdx + ptr.size) == next.startIdx)
            {
                //decrease the start index and increase the size by the size of the free'd pointer
                next.startIdx -= ptr.size;
                next.size += ptr.size;

                //check if a section should merge with the one before this
                //first, check if there is a section before this one
                if (insert != 0)
                {
                    //check if the sections alligne
                    if ((pointer.startIdx + pointer.size) == next.startIdx)
                    {
                        //grow the previous section to include this one
                        pointer.size += next.size;
                        //erase this section
                        m_free.erase(m_free.begin() + insert + 1);
                    }
                }

                //free'd the pointer successfully
                return true;
            }
        }
    }

    //no allignement found. Insert the freed section. 
    m_free.insert(m_free.begin() + insert, ptr);

    //return the successfull freeing
    return true;
}

void GraphicMemoryArena::update(const GraphicPointer& ptr, void* data) noexcept
{
    //lock the data
    lock();

    //write the data to the CPU side buffer
    memcpy(((uint8_t*)m_data) + ptr.startIdx, data, ptr.size);

    //check if an update for the reagion is queued
    for (size_t i = 0; i < m_changed.size(); ++i)
    {
        //check if the pointers are the same
        if (ptr == m_changed[i])
        {
            //if they are the same, stop - no GPU update needed if an update is pending
            unlock();
            return;
        }
    }

    //store that the memory updated
    m_changed.push_back(ptr);

    //register the GPU change
    gpuUpdate();

    //unlock the data again
    unlock();
}

bool GraphicMemoryArena::resize(uint64_t size) noexcept
{
    //lock the data
    lock();
    //allocatea area of the new size
    void* dat = calloc(1, size);

    //check if the memory allocation was successfull
    if (!dat)
    {
        //error : memory could not be allocated
        m_graphicInstance->getInstance()->log("Failed to allocate memory for graphic memory arena during resizing", MESSAGE_TYPE_ERROR);
        return false;
    }

    //transfear over the data
    memcpy(dat, m_data, m_size);
    //free the old data (do not leak the memory, it is potentialy gigabytes)
    free(m_data);
    //the created dat pointer is the new data
    m_data = dat;
    m_size = size;

    //register the resizing
    gpuSizeChanged();

    //unlock the data
    unlock();

    //return success
    return true;
}

void GraphicMemoryArena::lock() noexcept
{
    //lock the mutex
    m_mutex.lock();
    //store that the memory is locked
    m_locked = true;
}

void GraphicMemoryArena::unlock() noexcept
{
    //unlock the memory
    m_mutex.unlock();
    //store that the memory is not locked
    m_locked = false;
}