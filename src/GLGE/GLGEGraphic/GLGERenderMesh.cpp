/**
 * @file GLGERenderMesh.cpp
 * @author DM8AT
 * @brief implement the render meshes
 * @version 0.1
 * @date 2025-07-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include renderable meshes
#include "GLGERenderMesh.h"

bool RenderMesh::setMaterialData(void* data) noexcept
{
    //stop with a failure if the element is not attatched to an object
    if (!m_object) {return false;}

    //update the data
    m_material->getBuffer().getMemoryArena()->update(m_mPtr, data);
    //successfully updated
    return true;
}

void RenderMesh::onAttatch() noexcept
{
    //check if the material has a shader
    if (!m_material->getShader())
    {
        //log an error - incomplete material
        m_object->getInstance()->log("Invalid material for rendering - the material has no shader", MESSAGE_TYPE_ERROR);
    }

    //register to the material
    m_material->registerMesh(this);

    //allocate the vertex and index buffer
    m_vPtr = m_material->getVertexBuffer().getMemoryArena()->allocate(m_mesh->getVertexCount() * m_mesh->getVertexLayout().getVertexSize());
    m_iPtr = m_material->getIndexBuffer().getMemoryArena()->allocate(m_mesh->getIndices().size() * sizeof(index_t));
    //buffer the mesh data
    m_material->getVertexBuffer().getMemoryArena()->update(m_vPtr, (void*)m_mesh->getVertices());
    m_material->getIndexBuffer().getMemoryArena()->update(m_iPtr, (void*)m_mesh->getIndices().data());

    //allocate the material data
    m_mPtr = m_material->getBuffer().getMemoryArena()->allocate(m_material->getElementSize());
}

void RenderMesh::onRemove() noexcept
{
    //if the render mesh was not set up correctly, stop
    if (m_vPtr.size == 0) {return;}

    //free all the reagions
    //first, check if the graphic instance still exists
    if (m_object->getInstance()->getGraphicInstance())
    {
        //if it exists, remove the vertex and index buffer
        m_material->getVertexBuffer().getMemoryArena()->release(m_vPtr);
        m_material->getIndexBuffer().getMemoryArena()->release(m_iPtr);
    }
    //check if the memory arena still exists. It exists as long as the graphic instance exists and the material ist not deleted
    //but the material must not be deleted untill the render mesh is freed. 
    if (m_object->getInstance()->getGraphicInstance())
    {
        //then free the material data
        m_material->getBuffer().getMemoryArena()->release(m_mPtr);
    }
    //remove from the material
    m_material->removeMesh(this);
}