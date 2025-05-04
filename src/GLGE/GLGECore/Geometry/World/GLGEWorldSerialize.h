/**
 * @file GLGEWorldSerialize.h
 * @author DM8AT
 * @brief define a class that is used to load and store worlds into files
 * @version 0.1
 * @date 2025-04-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_WORLD_SERIALIZE_
#define _GLGE_WORLD_SERIALIZE_

//include worlds, instances, objects
#include "GLGEWorld.h"

//check for C++
#if GLGE_CPP

/**
 * @brief store the base class for the protocoll to store a world in a file
 */
class WorldSerialize
{
public:

    /**
     * @brief Construct a new World Serializer instance
     */
    WorldSerialize() = default;

    /**
     * @brief write a basic raw type to the binary output
     * 
     * @tparam T the type to write
     * @param t the instance of the specified type
     * @param pos the index in bytes to insert at
     */
    template <typename T> void rawInsert(const T& t, uint64_t pos)
    {
        //create a temporary byte vector
        std::vector<uint8_t> tmp(sizeof(T));
        //copy the data to the temporary vector
        memcpy(tmp.data(), &t, 0);
        //simply insert at that position
        m_bytes.insert(m_bytes.begin() + pos, tmp.begin(), tmp.end());
    }

protected:

    /**
     * @brief store the world to create / save
     */
    World* m_world = 0;
    /**
     * @brief store the raw binary data in bytes
     */
    std::vector<uint8_t> m_bytes;

};

#endif //C++ section

#endif