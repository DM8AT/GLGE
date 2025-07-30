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
//meshes are needed for meshes
#include "../GLGECore/Geometry/Mesh/GLGEMesh.h"
//information on how the verticies are stored is needed for rendering
#include "GLGERenderVertexLayout.h"

/**
 * @brief store how the depth test works
 */
typedef enum e_DepthTest
{
    /**
     * @brief the depth test will always pass
     */
    DEPTH_TEST_ALWAYS = 0,
    /**
     * @brief the depth test will never pass
     */
    DEPTH_TEST_NEVER,
    /**
     * @brief the depth test will only pass if the clipspace z component is less than the stored depth value
     */
    DEPTH_TEST_LESS,
    /**
     * @brief the depth test will only pass if the clipspace z component is less or equal than the stored depth value
     */
    DEPTH_TEST_LESS_EQUALS,
    /**
     * @brief the depth test will only pass if the clipspace z component is greater than the stored depth value
     */
    DEPTH_TEST_GREATER,
    /**
     * @brief the depth test will only pass if the clipspace z component is greater or equal than the stored depth value
     */
    DEPTH_TEST_GREATER_EQUALS,
    /**
     * @brief the depth test will only pass if the clipspace z component is equal to the stored depth value
     */
    DEPTH_TEST_EQUALS,
    /**
     * @brief the depth test will only pass if the clipspace z component is not equal to the stored depth value
     */
    DEPTH_TEST_NOT_EQUALS
} DepthTest;

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
     * @param culling true : back faces will be cullded (clockwise order) | false : back faces won't be culled
     * @param depthTest an enum dictating how the depth test should be done
     * @param depthWrite true : the value from the z component of the clip space position will be written to the depth buffer | false : the z component of the clip space is ignored
     * @param vLayout store information on the layout of the verticies
     * @param inst a reference to the instance the render material will belong to
     */
    RenderMaterial(uint64_t elementSize, bool culling, DepthTest depthTest, bool depthWrite, RenderVertexLayout* vLayout, Instance& inst) noexcept;

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
     * @brief get if backface culling is enabled
     * 
     * @return true : back faces will be culled
     * @return false : back faces will be drawn
     */
    inline bool isCullingEnabled() const noexcept {return m_cullFaces;}

    /**
     * @brief get the state of the depth test
     * 
     * @return DepthTest an enum value dictating the depth testing methode
     */
    inline DepthTest getDepthTesting() const noexcept {return m_depthTest;}

    /**
     * @brief get if depth writing is enabled
     * 
     * @return true : the depth value is written to a depth buffer
     * @return false : the depth value is ignored
     */
    inline bool isDepthWriteEnabled() const noexcept {return m_depthWrite;}

    /**
     * @brief Get the Buffer that contains the material instance data in VRam
     * @warning ONLY USE IF YOU KNOW WHAT YOU'RE DOING
     * 
     * @return Buffer& a reference to the buffer
     */
    inline Buffer& getBuffer() noexcept {return m_materialData;}

    /**
     * @brief Get the Vertex Buffer of the vertex layout
     * 
     * @return GraphicMemoryArena* a pointer to the vertex buffer
     */
    inline GraphicMemoryArena* getVertexBuffer() const noexcept {return m_vLayout->getVertexBuffer();}

    /**
     * @brief Get the Index Buffer of the vertex layout
     * 
     * @return GraphicMemoryArena* a pointer to the index buffer of the vertex layout
     */
    inline GraphicMemoryArena* getIndexBuffer() const noexcept {return m_vLayout->getIndexBuffer();}

    /**
     * @brief Get the Vertex Layout that is expected by this material
     * 
     * @return RenderVertexLayout* a pointer to the expected vertex layout
     */
    inline RenderVertexLayout* getVertexLayout() const noexcept {return m_vLayout;}

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
     * @return true : sucsessfully added it
     * @return false : error - something went wrong
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
     * @brief manage the indexing data for one mesh
     */
    struct MeshPointer
    {
        /**
         * @brief store in how many render meshes the pointer is referenced
         */
        uint64_t instances = 0;
        /**
         * @brief store a pointer into the vertex data
         */
        GraphicMemoryArena::GraphicPointer vertexPtr = {0,0};
        /**
         * @brief store a pointer into the index data
         */
        GraphicMemoryArena::GraphicPointer indexPtr = {0,0};
    };

    /**
     * @brief add an instance of a base mesh to the material for rendering
     * 
     * @param mesh a pointer to the default mesh to register
     * @return const MeshPointer* a read-only pointer to the data
     */
    const MeshPointer* addMeshInstance(Mesh* mesh) noexcept;

    /**
     * @brief remove an instance of a base mesh from the material
     * 
     * @param mesh a pointer to the mesh to remove from this material
     */
    void removeMeshInstance(Mesh* mesh) noexcept;

    /**
     * @brief store if back faces should be culled
     */
    bool m_cullFaces = true;
    /**
     * @brief store the methode used for depth testing
     */
    DepthTest m_depthTest = DEPTH_TEST_ALWAYS;
    /**
     * @brief store if the fragment depth write is enabled
     */
    bool m_depthWrite = true;

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
     * @brief store layout information for the verticies of the mesh
     */
    RenderVertexLayout* m_vLayout = 0;
    /**
     * @brief store all the meshes that use this material
     */
    std::vector<RenderMesh*> m_meshes;
    /**
     * @brief store pointer to all meshes that are loaded to the render material
     */
    std::unordered_map<Mesh*, MeshPointer> m_storedMeshes;

};

#endif //end of C++ section

#endif