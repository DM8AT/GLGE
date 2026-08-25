/**
 * @file MeshManager.h
 * @author DM8AT
 * @brief define a class to manage GPU meshes
 * @version 0.1
 * @date 2026-08-20
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHICS_MESH_MANAGER_
#define _GLGE_GRAPHIC_BACKEND_GRAPHICS_MESH_MANAGER_

#include "Core/Common.h"
#include "Core/Mesh.h"
#include "Buffer.h"
#include "GeometryPool.h"
#include "Graphic/VertexLayout.h"
#include <vector>
#include <unordered_map>

//use the backend namespace
namespace GLGE::Graphic::Backend::Graphic {

    /**
     * @brief define the type for mesh handles
     * 
     * Usual layout:
     * 6 bit version + 26 bit index
     */
    using MeshHandle = u32;
    /**
     * @brief define how much the version is shifted up
     * 
     * This is equivalent to the bit count for the index
     */
    inline static constexpr size_t MESH_HANDLE_VERSION_SHIFT = 26;
    /**
     * @brief compute a mask to extract the index
     */
    inline static constexpr u32 MESH_HANDLE_INDEX_MASK = (1u << MESH_HANDLE_VERSION_SHIFT) - 1u;


    /**
     * @brief a class that manages the storage of meshes
     * 
     * @note this class is not designed to be inherited from, even if it lives in the backend. That's why it is explicitly marked final. 
     */
    class MeshManager final {
    public:

        /**
         * @brief store meta data for a single mesh
         */
        struct alignas(16) MeshMeta {
            /**
             * @brief store the offset of the LOD0 from idx 0 in the lod meta table
             */
            u32 lodOffset = 0;
            /**
             * @brief store the amount of LODs in the mesh
             */
            u32 lodCount = 0;
            /**
             * @brief lower AABB bound of the mesh
             */
            float lowerBound[3]{};
            /**
             * @brief upper AABB bound of the mesh
             */
            float upperBound[3]{};
        };

        /**
         * @brief store meta-data for a single LOD
         */
        struct LODMeta {
            /**
             * @brief store the index offset of the indices for this LOD in the index buffer
             */
            u32 indexOffset = 0;
            /**
             * @brief store the amount of indices that make up this LOD
             */
            u32 indexCount = 0;
            /**
             * @brief store the offset of the vertices from index 0 of this vertex buffer
             */
            u32 vertexOffset = 0;
            /**
             * @brief store the amount of vertices that make up this LOD
             */
            u32 vertexCount = 0;
            /**
             * @brief store the error of this LOD
             * 
             * Compared to the LOD0
             */
            f32 error = FLT_MAX;
        };

        /**
         * @brief Construct a new Mesh Manager
         * 
         * @param instance a pointer to the graphic instance that the manager will belong to
         */
        MeshManager(GLGE::Graphic::Instance* instance);

        /**
         * @brief Destroy the Mesh Manager
         */
        ~MeshManager() {}

        /**
         * @brief Create a Mesh
         * 
         * Note that if a mesh is known or can be inferred from a known mesh that the mesh itself will be re-used
         * 
         * @param mesh a constant reference to the core mesh to load to the GPU
         * @param layout a constant reference to the vertex layout to use
         * @return `MeshHandle` the handle for the mesh
         */
        MeshHandle createMesh(const GLGE::Mesh& mesh, const GLGE::Graphic::VertexLayout& layout);
        /**
         * @brief increase the reference counter for a specific mesh
         * 
         * This is used to keep the reference counter for a mesh correct. 
         * 
         * @warning DO NOT USE IF YOU DON'T KNOW WHAT YOU'RE DOING! THIS MAY LEAD TO MEMORY LEAKS!
         * 
         * @param handle the handle to copy
         */
        void copyHandle(MeshHandle handle);
        /**
         * @brief free a specific handle
         * 
         * If all known (known = given out by this class) handles are free'd, the mesh will be deleted
         * 
         * @param handle the handle to free
         */
        void freeMesh(MeshHandle handle);

        /**
         * @brief Get the Pool
         * 
         * @return `GeometryPool&` a reference ot the geometry pool
         */
        inline GeometryPool& getPool() noexcept 
        {return m_gPool;}

        /**
         * @brief Get the Mesh Meta Buffer
         * 
         * @return `const Reference<Backend::Graphic::Buffer>&` a constant reference to the mesh meta-data buffer
         */
        inline const Reference<Backend::Graphic::Buffer>& getMeshMetaBuffer() const noexcept 
        {return m_meshMetaBuff;}

        /**
         * @brief get the LOD metadata buffer
         * 
         * @return `const Reference<Backend::Graphic::Buffer>&` a constant reference to the LOD meta-data buffer
         */
        inline const Reference<Backend::Graphic::Buffer>& getLODMetaBuffer() const noexcept 
        {return m_lodBuff;}

        /**
         * @brief Get the Archetype of a specific mesh
         * 
         * @param handle the handle to query the archetype for
         * @return `u32` the archetype ID of the mesh, `UINT32_MAX` if the handle is invalid
         */
        u32 getArchetypeOf(MeshHandle handle);

        /**
         * @brief Get the LOD metadata of the mesh
         * 
         * @param handle the handle to get the LOD metadata for
         * @return `LODMeta` the LOD meta-data, returns all-zero if the handle is invalid
         */
        LODMeta getLodMetaOf(MeshHandle handle);

        /**
         * @brief Get the Allocation Of a specific mesh
         * 
         * @param handle the handle of the mesh to get the allocation for
         * @return `const std::vector<GeometryPool::Allocation>&` a list of the allocations for all LODs, empty if the handle is invalid
         */
        const std::vector<GeometryPool::Allocation>& getAllocationOf(MeshHandle handle);

    private:

        /**
         * @brief a structure to uniquely identify a mesh
         */
        struct MeshIdentifier {
            /**
             * @brief the UUID of the mesh
             */
            UUID uuid;
            /**
             * @brief the ID of the archetype the mesh occupies
             */
            u32 archetypeId = 0;

            /**
             * @brief check if two identifiers are identical
             * 
             * @param other the identifier to compare against
             * @return `true` if they are equal, `false` if not
             */
            inline bool operator==(const MeshIdentifier& other) const noexcept 
            {return uuid == other.uuid && archetypeId == other.archetypeId;}
        };

        /**
         * @brief a structure to hash a mesh identifier
         * 
         * Simple functor
         */
        struct MeshIdentityHasher {
            /**
             * @brief a function to hash a mesh identifier
             * 
             * @param id the identifier to hash
             * @return `std::size_t` the hash result
             */
            inline std::size_t operator()(const MeshIdentifier& id) const noexcept {
                //hash UUID and archetype id
                std::size_t h1 = std::hash<u64>()(id.uuid);
                std::size_t h2 = std::hash<u32>()(id.archetypeId);
                //BOOST-Style hash combiner
                return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
            }
        };

        /**
         * @brief store an entry for a mesh
         */
        struct MeshEntry {
            /**
             * @brief store how many handles for this mesh were created
             */
            u32 refCount = 0;
            /**
             * @brief store the next free slot
             */
            u32 nextFree = UINT32_MAX;
            /**
             * @brief LOD offset of the mesh from buffer start
             */
            u32 lodOffset = 0;
            /**
             * @brief how many LODs the mesh has
             */
            u32 lodCount = 0;
            /**
             * @brief which archetype is used by the mesh
             */
            u32 archetypeId = 0;
            /**
             * @brief version tracker of the mesh
             */
            u8 version = 0;
            /**
             * @brief is this slot alive?
             */
            bool alive = false;
            /**
             * @brief an identifier for the mesh
             */
            MeshIdentifier identifier;
            
            /**
             * @brief Track allocations per LOD for deallocation
             */
            std::vector<GeometryPool::Allocation> lodAllocations;
        };

        /**
         * @brief make sure that the meta buffers have at least enough capacity to store the required slot count
         * 
         * @param requiredMeshSlots the amount of slots required for the mesh meta buffer
         * @param requiredLodSlots the amount of slots required for the lod meta buffer
         */
        void ensureMetaBufferCapacity(size_t requiredMeshSlots, size_t requiredLodSlots);
        /**
         * @brief update the mesh meta buffer
         * 
         * @param slot the slot to update
         * @param meta the new meta data for that slot
         */
        void updateMeshMetaGPU(u32 slot, const MeshMeta& meta);
        /**
         * @brief update the LOD meta buffer
         * 
         * @param lodOffset the offset to start writing at
         * @param lods a constant pointer to multiple LODs to write
         * @param count the amount of LODs to write
         */
        void updateLODMetaGPU(u32 lodOffset, const LODMeta* lods, u32 count);

        /**
         * @brief store the mesh meta buffer
         */
        Reference<Backend::Graphic::Buffer> m_meshMetaBuff;
        /**
         * @brief store the LOD meta buffer
         */
        Reference<Backend::Graphic::Buffer> m_lodBuff;

        /**
         * @brief store a list of entries
         */
        std::vector<MeshEntry> m_entries;
        /**
         * @brief store the entry free-list top
         */
        u32 m_freeTop = UINT32_MAX;
        /**
         * @brief store the free LOD slot
         */
        u32 m_nextLodFreeSlot = 0;

        /**
         * @brief store a mapping from mesh identifiers to handles
         */
        std::unordered_map<MeshIdentifier, MeshHandle, MeshIdentityHasher> m_handleMap;

        /**
         * @brief store the geometry pool
         */
        GeometryPool m_gPool;
        /**
         * @brief store a pointer ot hte instance that the mesh manager belongs to
         */
        GLGE::Graphic::Instance* m_inst = nullptr;

        /**
         * @brief define a vector to use if a handle is invalid
         */
        inline static const std::vector<GeometryPool::Allocation> ms_failureAllocVec = {};
    };
}

#endif