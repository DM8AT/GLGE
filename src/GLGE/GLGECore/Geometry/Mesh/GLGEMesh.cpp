/**
 * @file GLGESubmesh.cpp
 * @author DM8AT
 * @brief define the functionality of submeshes in GLGE
 * @version 0.1
 * @date 2025-06-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include submeshes
#include "GLGEMesh.h"

Mesh::Mesh(SimpleVertex* vertices, uint64_t vertexCount, idx_Triangle* triangles, uint32_t triangleCount) noexcept
{
    //store the vertex default vertex layout as implied by the SimpleVertex type in the constructor. 
    //The structure of SimpleVertex is as follows:
    // 0 : pos : vec3
    // 1 : tex : vec2
    // 2 : normal : vec3
    VertexLayoutElement elements[] = {
        {"pos", sizeof(vec3)},
        {"tex", sizeof(vec2)},
        {"normal", sizeof(vec3)}
    };
    m_layout = VertexLayout(elements, sizeof(elements) / sizeof(*elements));

    //store the vertices
    m_vertexCount = vertexCount;
    m_vertices = malloc(m_layout.getVertexSize() * m_vertexCount);
    //security check to check if malloc was successfull
    if (!m_vertices)
    {
        //set the amount of vertices to 0
        m_vertexCount = 0;
        //stop this function, can't load indices without vertices
        return;
    }
    //finaly, copy the vertex data
    memcpy(m_vertices, vertices, m_layout.getVertexSize() * m_vertexCount);

    //store the indices
    m_indices.resize(triangleCount*3);
    memcpy(m_indices.data(), triangles, sizeof(*triangles)*triangleCount);
}

Mesh::~Mesh()
{
    //check if vertex data exists
    if (m_vertexCount > 0)
    {
        //clean up the allocated data
        free(m_vertices);
    }
    //clean up the index vector
    m_indices.clear();
}