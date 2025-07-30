/**
 * @file GLGERenderMaterial.cpp
 * @author DM8AT
 * @brief implement the render materials
 * @version 0.1
 * @date 2025-07-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the render material class
#include "GLGERenderMaterial.h"

//include the graphic APIs
#include "GraphicAPI/GraphicAPIs/GLGE_AllAPIs.h"

RenderMaterial::RenderMaterial(uint64_t elementSize, bool culling, DepthTest depthTest, bool depthWrite, RenderVertexLayout* vertexLayout, Instance& inst) noexcept
   //register the render material to the instance
  : InstAttachableClass(&inst, "Render Material"),
   //store the culling parameter
   m_cullFaces(culling),
   //store the depth parameters
   m_depthTest(depthTest),
   m_depthWrite(depthWrite),
   //store the size of a single element
   m_elementSize(elementSize),
   //create the buffer for the material data
   m_materialData(MEMORY_USAGE_READ_WRITE, inst),
   //store the expected vertex layout
   m_vLayout(vertexLayout)
{
    //register the material data buffer as resizable and API element
    m_materialData.getMemoryArena()->setResizable(true);
    m_materialData.getMemoryArena()->setAPI(true);
}

/**
 * @brief add an instance of a base mesh to the material for rendering
 * 
 * @param mesh a pointer to the default mesh to register
 * @return const MeshPointer* a read-only pointer to the data
 */
const RenderMaterial::MeshPointer* RenderMaterial::addMeshInstance(Mesh* mesh) noexcept
{
    //store mesh location if it exists
    auto pos = m_storedMeshes.find(mesh);
    //check if the mesh exists
    //if it dosn't exist, create the pointer instance
    if (pos == m_storedMeshes.end())
    {
        //allocate the data for the mesh
        MeshPointer ptr = {1, {}, {}};
        ptr.vertexPtr = m_vLayout->getVertexBuffer()->allocate(mesh->getVertexCount() * mesh->getVertexLayout().getVertexSize());
        ptr.indexPtr = m_vLayout->getIndexBuffer()->allocate(mesh->getIndices().size() * sizeof(index_t));
        //buffer the actual mesh data
        m_vLayout->getVertexBuffer()->update(ptr.vertexPtr, (void*)mesh->getVertices());
        m_vLayout->getIndexBuffer()->update(ptr.indexPtr, (void*)mesh->getIndices().data());
        //store the element and map it to the mesh
        m_storedMeshes.try_emplace(mesh, ptr);
        //return a pointer to the mesh pointer
        return &(m_storedMeshes[mesh]);
    }
    //else, the instance exists. Just return the data. 
    else
    {
        //if it exists, increment the instance count by one
        pos->second.instances++;
        //return the pointer
        return &(pos->second);
    }
}

void RenderMaterial::removeMeshInstance(Mesh* mesh) noexcept
{
    //get the location of the mesh
    auto pos = m_storedMeshes.find(mesh);
    //if the mesh isn't stored print an error and exit
    if (pos == m_storedMeshes.end())
    {
        //error and exit
        m_instance->log("Tried to remove a mesh from a render material that isn't registerd to the render material", MESSAGE_TYPE_ERROR);
        return;
    }

    //decrement the instance count
    pos->second.instances--;
    //check if there are no instances left
    if (pos->second.instances == 0)
    {
        //remove the data from the map
        m_vLayout->getVertexBuffer()->release(pos->second.vertexPtr);
        m_vLayout->getIndexBuffer()->release(pos->second.indexPtr);
        //remove the mesh mapping
        m_storedMeshes.erase(pos);
    }
}