/**
 * @file GLGEMeshMaterial.h
 * @author DM8AT
 * @brief define a wrapper for a material class that can be overloaded to implement different functionalities
 * @version 0.1
 * @date 2025-06-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_MESH_MATERIAL_
#define _GLGE_MESH_MATERIAL_

//include the settings
#include "../../GLGESettings.h"

/**
 * @brief store a value that says that the type of a material is unknown
 */
#define MATERIAL_TYPE_UNDEFINED 0
/**
 * @brief store a value that says that the material is for graphics
 */
#define MATERIAL_TYPE_GRAPHIC 1
/**
 * @brief store a value that says that the material is for physics
 */
#define MATERIAL_TYPE_PHYSIC 2
/**
 * @brief store a value that says that the material is for audio
 */
#define MATERIAL_TYPE_AUDIO 3
/**
 * @brief store a value that defines the lowest value free for custom use
 */
#define MATERIAL_TYPE_CUSTOM 16

//check for C++
#if GLGE_CPP

//say that Submeshes will exist
class Mesh;

/**
 * @brief store a overloadable material class
 */
class MeshMaterial
{
public:

    /**
     * @brief Construct a new Mesh Material
     */
    MeshMaterial() = default;

    /**
     * @brief Construct a new Mesh Material
     * 
     * @param type the type of the material to create
     */
    MeshMaterial(uint64_t type)
     : m_type(type)
    {}

    /**
     * @brief Destroy the Mesh Material
     */
    virtual ~MeshMaterial() {}

    /**
     * @brief Get the type of this material
     * 
     * @return const uint64_t& the type of the material encoded as an integer
     */
    inline const uint64_t& getType() const noexcept {return m_type;}

    /**
     * @brief this function is called when the material is applied
     * 
     * @param Mesh* the mesh the material was applied to
     */
    virtual void onApply(Mesh*) noexcept {}

    /**
     * @brief ready this material for rendering
     */
    virtual void readyRender() noexcept {}

    /**
     * @brief a generic function to handle material calls
     * 
     * @param Mesh* the mesh the call comes from, MAY BE NULL when called from material instance
     * @param void* the payload for the call
     * @param uint64_t the size of the payload in bytes
     */
    virtual void genericCall(Mesh*, void*, uint64_t) {}

protected:

    /**
     * @brief store the type of this material
     */
    uint64_t m_type = MATERIAL_TYPE_UNDEFINED;

};

#endif //C++ section

#endif