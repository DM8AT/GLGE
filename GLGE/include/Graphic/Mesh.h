/**
 * @file Mesh.h
 * @author DM8AT
 * @brief define what a mesh is
 * @version 0.1
 * @date 2026-03-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_MESH_
#define _GLGE_GRAPHIC_MESH_

//add base classes
#include "Core/BaseClass.h"
//add core instances
#include "Core/Instance.h"
//add graphic instances
#include "Instance.h"

//use the libraries namespace
namespace GLGE::Graphic {

    /**
     * @brief store a mesh including a LOD chain
     */
    class Mesh : public BaseClass {
    public:

        /**
         * @brief store information about the LOD structures
         */
        using LODInfo = GLGE::Graphic::Backend::Graphic::MeshPool::LODInfo;

        /**
         * @brief Construct a new Mesh
         * 
         * @param vertices a pointer to the vertices to store
         * @param vertexSize the size in bytes of a single vertex
         * @param vertexCount the amount of vertices in the list
         * @param indices a pointer to the indices to store
         * @param indexCount the amount of indices in the list
         * @param lod a pointer to the level of detail info array
         * @param LODCount store the amount of level of detail info sets
         */
        Mesh(const void* vertices, size_t vertexSize, size_t vertexCount, const u32* indices, size_t indexCount, const LODInfo* lod, u8 LODCount)
         : BaseClass(), m_pool(getInstance()->getExtension<GLGE::Graphic::Instance>()->getMeshPool()), 
           m_id(m_pool->allocate(vertices, vertexSize, vertexCount, indices, indexCount, lod, LODCount))
        {}

        /**
         * @brief Destroy the Mesh
         */
        ~Mesh() 
        {destroy();}

        /**
         * @brief re-create the stored mesh by dropping the old mesh and storing the new mesh
         * 
         * @param vertices a pointer to the vertices to store
         * @param vertexSize the size in bytes of a single vertex
         * @param vertexCount the amount of vertices in the list
         * @param indices a pointer to the indices to store
         * @param indexCount the amount of indices in the list
         * @param lod a pointer to the level of detail info array
         * @param LODCount store the amount of level of detail info sets
         */
        void recreate(const void* vertices, size_t vertexSize, size_t vertexCount, const u32* indices, size_t indexCount, const LODInfo* lod, u8 LODCount) 
        {if (m_id != UINT64_MAX){destroy();} m_id = m_pool->allocate(vertices, vertexSize, vertexCount, indices, indexCount, lod, LODCount);}

        /**
         * @brief destroy the stored mesh
         */
        void destroy()
        {m_pool->destroy(m_id); m_id = UINT64_MAX;}

        /**
         * @brief get the ID of the mesh
         * 
         * @return `u64` the ID of the mesh
         */
        inline u64 getID() const noexcept
        {return m_id;}

        /**
         * @brief Get the Pool
         * 
         * @return `Reference<GLGE::Graphic::Backend::Graphic::MeshPool>` a reference to the mesh pool
         */
        inline Reference<GLGE::Graphic::Backend::Graphic::MeshPool> getPool() const noexcept
        {return m_pool;}

        /**
         * @brief get the LOD count of the mesh
         * 
         * @return `u8` the amount of LODs the mesh has
         */
        inline u8 getLODCount() const noexcept
        {return m_pool->getLODCount(m_id);}

        /**
         * @brief Get information about the level of detail of the mesh
         * 
         * @param lod the index of the level of detail to get
         * @return `LODInfo` the level of detail information of the mesh
         */
        inline LODInfo getLODInfo(u8 lod) const noexcept
        {return m_pool->getLODInfo(m_id, lod);}

        /**
         * @brief Get the amount of vertices the mesh has in total
         * 
         * This is the sum of all level of detail's vertices
         * 
         * @return `u64` the total vertex count of the mesh
         */
        inline u64 getVertexCount() const noexcept
        {return m_pool->getVertexCount(m_id);}

        /**
         * @brief Get the vertex data
         * 
         * @return `const void*` the vertex data of the mesh
         */
        inline const void* getVertexData() const noexcept
        {return m_pool->getVertexData(m_id);}

        /**
         * @brief Get the size of a single vertex in bytes
         * 
         * @return `u64` the size of a single vertex in bytes
         */
        inline u64 getVertexElementSize() const noexcept
        {return m_pool->getVertexSize(m_id);}

        /**
         * @brief Get the amount of total indices of the mesh
         * 
         * This is the sum of all level of detail's indices
         * 
         * @return `u64` the amount of total indices owned by the mesh
         */
        inline u64 getIndexCount() const noexcept
        {return m_pool->getIndexCount(m_id);}

        /**
         * @brief Get the index data of the mesh
         * 
         * @return `const void*` a pointer to the index data of the mesh
         */
        inline const void* getIndexData() const noexcept
        {return m_pool->getIndexData(m_id);}

        /**
         * @brief Get the size of the index type in bytes
         * 
         * @return `u8` the size of the index type in bytes
         */
        inline u8 getIndexTypeSize() const noexcept
        {return m_pool->getIndexTypeSize(m_id);}

        /**
         * @brief Get the index section
         * 
         * @return `const LODInfo::Section&` a constant reference to the whole index section
         */
        inline const LODInfo::Section& getIndexSection() const noexcept
        {return m_pool->getIndexSection(m_id);}

        /**
         * @brief Get the vertex section
         * 
         * @return `const LODInfo::Section&` a constant reference to the whole vertex section
         */
        inline const LODInfo::Section& getVertexSection() const noexcept
        {return m_pool->getVertexSection(m_id);}

    protected:

        /**
         * @brief store the mesh pool reference
         */
        Reference<GLGE::Graphic::Backend::Graphic::MeshPool> m_pool;
        /**
         * @brief store the mesh ID
         */
        u64 m_id = 0;

    };

}

#endif