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

//check if this is C++
#if GLGE_CPP

/**
 * @brief store the binding between a GPU mesh and a CPU mesh
 */
class RenderMesh
{
public:

    RenderMesh() = default;

    RenderMesh(Mesh* mesh)
     : m_mesh(mesh) 
    {}

protected:

    /**
     * @brief 
     */
    Mesh* m_mesh = 0;

};

#endif //end of C++ section

#endif