/**
 * @file GLGERenderMesh.h
 * @author DM8AT
 * @brief manage the high-level binding between CPU meshes and the GPU
 * @version 0.1
 * @date 2025-06-09
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_RENDER_MESH_
#define _GLGE_RENDER_MESH_

//include the settings
#include "../GLGECore/GLGESettings.h"
//include geometry (for Meshes and related)
#include "../GLGECore/Geometry/GLGEGeometry.h"
//include render materials to structure the surface of a mesh
#include "GLGERenderMaterial.h"
//include render vertex layouts to specify the layout of the verticies
#include "GLGERenderVertexLayout.h"

//check if this is C++
#if GLGE_CPP

/**
 * @brief store an instance of a mesh that can be renderd
 */
class RenderMesh final
{
public:

    /**
     * @brief Construct a new Render Mesh
     * @warning explicetly deleted
     */
    RenderMesh() = delete;

    /**
     * @brief Construct a new Render Mesh
     * 
     * @param mesh the mesh to use for rendering
     * @param material the material to apply to the mesh
     */
    RenderMesh(Mesh* mesh, RenderMaterial* material);

    /**
     * @brief Destroy the Render Mesh
     */
    virtual ~RenderMesh();

    /**
     * @brief Set the data of the material reagion this object owns
     * 
     * @param data the data to set the material data to
     * 
     * @return true : the data was set successfully
     * @return false : failed to set the data
     */
    bool setMaterialData(void* data) noexcept;

    /**
     * @brief Get the Mesh the render mesh renders
     * 
     * @return Mesh* a pointer to the mesh that will be renderd
     */
    inline Mesh* getMesh() const noexcept {return m_mesh;}

    /**
     * @brief Get the Material the render mesh uses for rendering
     * 
     * @return RenderMaterial* a pointer to the used material
     */
    inline RenderMaterial* getMaterial() const noexcept {return m_material;}

    /**
     * @brief Get the pointer into the vertex data
     * 
     * @return const GraphicMemoryArena::GraphicPointer& the pointer into the vertex buffer
     */
    inline const GraphicMemoryArena::GraphicPointer& getVertexPointer() const noexcept {return m_meshPtr->vertexPtr;}

    /**
     * @brief Get the pointer into the index data
     * 
     * @return const GraphicMemoryArena::GraphicPointer& the pointer into the index buffer
     */
    inline const GraphicMemoryArena::GraphicPointer& getIndexPointer() const noexcept {return m_meshPtr->indexPtr;}

    /**
     * @brief Get the pointer into the material data
     * 
     * @return const GraphicMemoryArena::GraphicPointer& the pointer into the material buffer
     */
    inline const GraphicMemoryArena::GraphicPointer& getMaterialPointer() const noexcept {return m_mPtr;}

protected:

    /**
     * @brief store a pointer to the mesh that this renderable mesh will render
     */
    Mesh* m_mesh = 0;
    /**
     * @brief store the material that will be applied to the mesh
     */
    RenderMaterial* m_material = 0;
    /**
     * @brief store a pointer to the pointer into the mesh data of the material
     */
    const RenderMaterial::MeshPointer* m_meshPtr = 0;
    /**
     * @brief store the reagion of the material data buffer the renderable mesh owns
     */
    GraphicMemoryArena::GraphicPointer m_mPtr = {0,0};

};

#endif //end of C++ section

#endif