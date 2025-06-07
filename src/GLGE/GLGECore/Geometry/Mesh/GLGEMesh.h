/**
 * @file GLGESubmesh.h
 * @author DM8AT
 * @brief define submeshes for GLGE
 * @version 0.1
 * @date 2025-06-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_SUBMESH_
#define _GLGE_SUBMESH_

//include the settings
#include "../../GLGESettings.h"
//include vertices
#include "../GLGEVertex.h"
//include mesh materials
#include "GLGEMeshMaterial.h"

/**
 * @brief store the type used for indices
 */
typedef uint32_t index_t;

/**
 * @brief define the indices of a triangle
 */
typedef struct s_idx_Triangle {
    /**
     * @brief store the indices of the triangle for the mesh
     */
    index_t a, b, c;
} idx_Triangle;

//check for C++
#if GLGE_CPP

/**
 * @brief store some 3D geometry
 */
class Mesh
{
public:

    /**
     * @brief Construct a new Mesh
     */
    Mesh() = default;

    /**
     * @brief Construct a new Mesh
     * 
     * @param vertices a list of vertices for the submesh
     * @param vertexCount the amount of vertices in the vertex array
     * @param triangles a list of triangles containing indices for the submesh
     * @param triangleCount the amout of triangles in the index array
     */
    Mesh(SimpleVertex* vertices, uint64_t vertexCount, idx_Triangle* triangles, uint32_t triangleCount) noexcept;

    /**
     * @brief Destroy the Mesh
     */
    ~Mesh();

    /**
     * @brief set a material for this submesh
     * 
     * @param material the material for the submesh
     */
    inline void setMaterial(MeshMaterial* material) noexcept 
    {
        //store the material at its specific index
        m_materials[material->getType()] = material;
    }

    /**
     * @brief check if a material of a specific type exists
     * 
     * @param type the type identifyer to check for
     * @return true : a material of that type is added to this submesh
     * @return false : no material of this type is applied to this submesh
     */
    inline bool hasMaterialOfType(uint64_t type) const noexcept {return (m_materials.find(type) != m_materials.end());}

protected:

    /**
     * @brief store the layout of a vertex used for this submesh
     */
    VertexLayout m_layout;
    /**
     * @brief store the indices of the submesh
     */
    std::vector<index_t> m_indices;
    /**
     * @brief store all the vertices for the submesh
     */
    void* m_vertices = 0;
    /**
     * @brief store the amount of stored vertices
     */
    uint64_t m_vertexCount = 0;

    /**
     * @brief store all materials applied to the submesh mapped by theire type
     */
    std::unordered_map<uint64_t, MeshMaterial*> m_materials;

};

#endif //C++ section

#endif