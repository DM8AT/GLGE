/**
 * @file MeshPool.h
 * @author DM8AT
 * @brief define the OpenGL overload for the mesh pool system
 * @version 0.1
 * @date 2026-03-25
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_MESH_POOL_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_MESH_POOL_

//add the resource set API
#include "Graphic/Backend/Graphics/MeshPool.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::OpenGL {

    /**
     * @brief overload the mesh pool
     */
    class MeshPool : public GLGE::Graphic::Backend::Graphic::MeshPool {
    public:

        /**
         * @brief Construct a new Mesh Pool
         */
        MeshPool();

        /**
         * @brief Destroy the Mesh Pool
         */
        virtual ~MeshPool();

        /**
         * @brief Construct a new Mesh Pool
         * 
         * @param vertices a pointer to the vertices to store
         * @param vertexSize the size in bytes of a single vertex
         * @param vertexCount the amount of vertices in the list
         * @param indices a pointer to the indices to store
         * @param indexCount the amount of indices in the list
         * @param lod a pointer to the level of detail info array
         * @param LODCount store the amount of level of detail info sets
         * 
         * @return `u64` the mesh ID that can be used to quarry general mesh metadata from the mesh metadata buffer
         */
        virtual u64 allocate(const void* vertices, size_t vertexSize, size_t vertexCount, const u32* indices, size_t indexCount, const LODInfo* lod, u8 LODCount) override;

        /**
         * @brief get the amount of LODs for a specific mesh
         * 
         * @param meshId the ID of the mesh to quarry the LOD count from
         * @return `u8` the amount of LODs the mesh has
         */
        virtual u8 getLODCount(u64 meshId) const override;

        /**
         * @brief destroy a specific mesh
         * 
         * @param meshId the ID of the mesh to destroy
         */
        virtual void destroy(u64 meshId) override;

        /**
         * @brief Get the Index Section
         * 
         * @param meshId the ID of the mesh to get the whole index section from
         * @return `const LODInfo::Section&` a constant reference to the whole index section
         */
        virtual const LODInfo::Section& getIndexSection(u64 meshId) const override;

        /**
         * @brief Get the Vertex Section
         * 
         * @param meshId the ID of the mesh to get the whole vertex section from
         * @return `const LODInfo::Section&` a constant reference to the whole vertex section
         */
        virtual const LODInfo::Section& getVertexSection(u64 meshId) const override;

        /**
         * @brief get the currently used VBO
         * 
         * @return `u32` the currently used VBO
         */
        inline u32 getVBO() const noexcept
        {return m_vbo;}

        /**
         * @brief get the currently used IBO
         * 
         * @return `u32` the currently used IBO
         */
        inline u32 getIBO() const noexcept
        {return m_ibo;}

        /**
         * @brief store metadata about a single mesh
         */
        struct MetaData {
            /**
             * @brief store the whole owned vertex section
             */
            LODInfo::Section vertex;
            /**
             * @brief store the whole owned index section
             */
            LODInfo::Section index;

            /**
             * @brief store all the LODs
             */
            LODInfo lod[MAX_LOD_COUNT]{};
            /**
             * @brief store the amount of LODs
             */
            u8 lodCount = 0;
        };

        /**
         * @brief store metadata about a slot
         */
        struct MetaSlot {
            /**
             * @brief store the data union
             */
            union Data {
                /**
                 * @brief the metadata
                 */
                MetaData data;
                /**
                 * @brief store the next free list slot
                 */
                size_t freeList;

                /**
                 * @brief Construct a new Data
                 */
                Data() : freeList(0) {}
            };
            /**
             * @brief store the metadata
             */
            Data data;
            /**
             * @brief `true` if this slot stores a mesh, `false` if this is part of the free index list
             */
            bool alive = false;

            /**
             * @brief Construct a new Meta Slot
             */
            MetaSlot() = default;
        };

        /**
         * @brief get information about the LOD structures of a specific mesh
         * 
         * @param meshId the ID of the mesh to get the LOD informations from
         * @param lod the index of the level of detail to get
         * @return `LODInfo` informations about the LODs of the mesh
         */
        virtual LODInfo getLODInfo(u64 meshId, u8 lod) const override;

        /**
         * @brief Get the amount of vertices of a specific mesh
         * 
         * @param meshId the ID of the mesh to get the vertex count from
         * @return `u64` the amount of vertices the mesh has
         */
        virtual u64 getVertexCount(u64 meshId) const override;

        /**
         * @brief Get the vertex data of a specific mesh
         * 
         * @param meshId the ID of the mesh to get the vertex data from
         * @return `const void*` the constant vertex data of the mesh
         */
        virtual const void* getVertexData(u64 meshId) const override;

        /**
         * @brief Get the vertex size in bytes of a specific mesh
         * 
         * This refers to the size of a SINGLE vertex, not the VBO size. 
         * 
         * @param meshId the ID of the mesh to get the vertex size from
         * @return `u64` the vertex size in bytes
         */
        virtual u64 getVertexSize(u64 meshId) const override;

        /**
         * @brief Get the index count of a specific mesh
         * 
         * @param meshId the ID of the mesh to get the index count from
         * @return `u64` the amount of indices the mesh has
         */
        virtual u64 getIndexCount(u64 meshId) const override;

        /**
         * @brief Get the index data from the mesh
         * 
         * @param meshId the ID of the mesh to get the index data from
         * @return `const void*` the constant index data of the mesh
         */
        virtual const void* getIndexData(u64 meshId) const override;

        /**
         * @brief Get the byte size of the index data type
         * 
         * 1 byte  : unsigned byte (unsigned char)  
         * 2 bytes : unsigned short  
         * 4 bytes : unsigned int  
         * Anything else is invalid
         * 
         * @param meshId the ID of the mesh to get the index size from
         * @return `u8` the index type size used by the mesh in bytes
         */
        virtual u8 getIndexTypeSize(u64 meshId) const override;

    protected:

        /**
         * @brief store the free sections of the vertex buffer
         */
        std::vector<LODInfo::Section> m_vertexFreeList;
        /**
         * @brief store the free sections of the index buffer
         */
        std::vector<LODInfo::Section> m_indexFreeList;

        /**
         * @brief store the mapping from mesh IDs to metadata on the CPU
         * 
         * This also stores the free index slot list inline
         */
        std::vector<MetaSlot> m_slots;
        /**
         * @brief store the index of the top of the free list
         */
        size_t m_freeTop = SIZE_MAX;

        /**
         * @brief store the actual vertex buffer
         */
        u32 m_vbo = 0;
        /**
         * @brief store a CPU side copy of the vertex buffer
         */
        void* m_vboCopy = nullptr;
        /**
         * @brief store the last VBO
         * 
         * This is done so that the OS can deffer actual buffer deletion until it is no longer needed
         */
        u32 m_lastVBO = 0;
        /**
         * @brief store the size of the vertex buffer in bytes
         */
        u64 m_vboSize = 0;
        /**
         * @brief store the actual index buffer
         */
        u32 m_ibo = 0;
        /**
         * @brief store a CPU side copy of the index buffer
         */
        void* m_iboCopy = nullptr;
        /**
         * @brief store the last IBO
         * 
         * This is done so that the OS can deffer actual buffer deletion until it is no longer needed
         */
        u32 m_lastIBO = 0;
        /**
         * @brief store the size of the index buffer in bytes
         */
        u64 m_iboSize = 0;

    };

}

#endif