/**
 * @file GLGERenderMaterial.h
 * @author DM8AT
 * @brief define what a material instance and a material system are
 * @version 0.1
 * @date 2025-07-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_RENDER_MATERIAL_
#define _GLGE_RENDER_MATERIAL_

//include instances, graphic instances, Memory arenas and the settings
#include "GraphicAPI/GLGEGraphicAPI_Instance.h"
//buffer are used for access to memory arenas and VRam
#include "GLGEBuffer.h"
//shader are needed to render a material
#include "Shader/GLGEShader.h"

//check for C++ (only C++ supports classes)
#if GLGE_CPP

//render meshes will be defined later
class RenderMesh;

/**
 * @brief store a class to manage the structure of a material
 * This class can create material instances. A material instance can't be created on its own. 
 */
class RenderMaterial final : public InstAttachableClass
{
public:

    /**
     * @brief Construct a new Render Material
     * @warning explicetly deleted
     */
    RenderMaterial() = delete;

    /**
     * @brief Construct a new Render Material
     * 
     * @param elementSize the size of a single material instance in VRam
     * @param inst a reference to the instance the render material will belong to
     */
    RenderMaterial(uint64_t elementSize, Instance& inst) noexcept;

    /**
     * @brief Destroy the Render Material
     */
    virtual ~RenderMaterial()
    {
        //check if there are still meshes that exist
        if (m_meshes.size() != 0)
        {
            //print an error
            m_instance->log("Deleting a render material before all render meshes that use this material where deleted", MESSAGE_TYPE_ERROR);
        }
    }

    /**
     * @brief Get the Buffer that contains the material instance data in VRam
     * @warning ONLY USE IF YOU KNOW WHAT YOU'RE DOING
     * 
     * @return Buffer& a reference to the buffer
     */
    inline Buffer& getBuffer() noexcept {return m_materialData;}

    /**
     * @brief Get the Index Buffer
     * 
     * @return Buffer& a buffer containing all indices of all meshes bound to the instance. All sub-meshes are zero based. 
     */
    inline Buffer& getIndexBuffer() noexcept {return m_indices;}

    /**
     * @brief Get the Vertex Buffer
     * @warning the format of the vertices is unknown. 
     * 
     * @return Buffer& a buffer containing all vertices of all meshes bound to the instance
     */
    inline Buffer& getVertexBuffer() noexcept {return m_vertices;}

    /**
     * @brief Set the Shader of the material
     * 
     * @param shader the shader the material will use
     */
    inline void setShader(Shader* shader) noexcept
    {
        //check if no shader exists
        if (!m_shader)
        {
            //store the new shader
            m_shader = shader;
        }
        else
        {
            //can't change a shader - log an error
            m_instance->log("Can't change a shader of a material once it is bound - need to re-create material", MESSAGE_TYPE_CRITICAL_WARNING);
        }
    }

    /**
     * @brief Get the Shader the material uses. If this returns 0, the material's shader is not set. 
     * 
     * @return Shader* a pointer to the shader the material will use
     */
    inline Shader* getShader() const noexcept {return m_shader;}

    /**
     * @brief Get the size of a single instance of this material in VRam
     * 
     * @return uint64_t the size of a single element in vram in bytes
     */
    inline uint64_t getElementSize() const noexcept {return m_elementSize;}

    //add render meshes as a friend class
    friend class RenderMesh;

protected:

    /**
     * @brief add a render mesh to this material and say that it uses this material
     * 
     * @param mesh a pointer to the mesh that uses this material
     * @return true 
     * @return false 
     */
    inline bool registerMesh(RenderMesh* mesh) noexcept
    {
        //check if the element allready exists
        if (std::find(m_meshes.begin(), m_meshes.end(), mesh) == m_meshes.end())
        {
            //if it dosn't exist, add it
            m_meshes.push_back(mesh);
            //successfully added it
            return true;
        }
        //else, the mesh is alleady added
        return false;
    }

    /**
     * @brief remove a render mesh from this material and say that it won't use it any longer
     * 
     * @param mesh a pointer to the mesh to remove from this material
     * @return true : successfully remove the mesh
     * @return false failed to remove the element
     */
    inline bool removeMesh(RenderMesh* mesh) noexcept
    {
        //search the mesh to remove
        auto el = std::find(m_meshes.begin(), m_meshes.end(), mesh);
        //check if the element exists
        if (el != m_meshes.end())
        {
            //remove the mesh
            m_meshes.erase(el);
            return true;
        }
        //else, the element was not added or allready removed
        return false;
    }

    /**
     * @brief store the size of a single material instance in VRam
     */
    uint64_t m_elementSize = 0;
    /**
     * @brief store the shader that is used to render this material
     */
    Shader* m_shader = 0;
    /**
     * @brief store the material data in VRam
     */
    Buffer m_materialData;
    /**
     * @brief store all vertices for all meshes the material owns, regardless of the structure
     */
    Buffer m_vertices;
    /**
     * @brief store all indices for all meshes the material owns. Every segment is zero-based. 
     */
    Buffer m_indices;
    /**
     * @brief store all the meshes that use this material
     */
    std::vector<RenderMesh*> m_meshes;

};

#endif //end of C++ section

#endif