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

RenderMaterial::RenderMaterial(uint64_t elementSize, Instance& inst) noexcept
   //register the render material to the instance
  : InstAttachableClass(&inst, "Render Material"),
   //store the size of a single element
   m_elementSize(elementSize),
   //create the buffer for the material data
   m_materialData(MEMORY_USAGE_READ_WRITE, inst),
   //create the vertex and index buffer
   m_vertices(MEMORY_USAGE_READ_WRITE, inst),
   m_indices(MEMORY_USAGE_READ_WRITE, inst)
{
    //register the material data buffer as resizable and API element
    m_materialData.getMemoryArena()->setResizable(true);
    m_materialData.getMemoryArena()->setAPI(true);
    //also register the vertex and index buffer as API element and resizable
    m_vertices.getMemoryArena()->setResizable(true);
    m_vertices.getMemoryArena()->setAPI(true);
    m_indices.getMemoryArena()->setResizable(true);
    m_indices.getMemoryArena()->setAPI(true);
}