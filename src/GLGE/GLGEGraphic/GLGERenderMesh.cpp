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

RenderMesh::RenderMesh(Mesh* mesh, RenderMaterial* material)
 //store the inputs
 : m_mesh(mesh),
   m_material(material)
{
    //check if the vertex layout is correct
    if (m_material->getVertexLayout()->getLayout() != (VertexLayout&)mesh->getVertexLayout())
    {
        //print a CRITICAL warning, this will lead to wierd behaviour
        m_material->getInstance()->log("Mesh vertex layout and render vertex layout of the material passed to a render mesh are NOT the same - This can lead to wierd / undefined behaviour", MESSAGE_TYPE_CRITICAL_WARNING);
    }

    //register to the material
    m_material->registerMesh(this);
    //add the actual mesh
    m_meshPtr = m_material->addMeshInstance(m_mesh);

    //allocate the material instance data
    m_mPtr = m_material->getBuffer().getMemoryArena()->allocate(material->getElementSize());
}

RenderMesh::~RenderMesh()
{
    //check if the pointers are set
    if (!m_mesh)
    {
        //free the mesh instance
        m_material->removeMesh(this);
        m_material->removeMeshInstance(m_mesh);
        //zero the pointer
        m_material = 0;
        m_mesh = 0;
    }
}

bool RenderMesh::setMaterialData(void* data) noexcept
{
    //update the data
    m_material->getBuffer().getMemoryArena()->update(m_mPtr, data);
    //successfully updated
    return true;
}