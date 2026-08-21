/**
 * @file MeshPool.h
 * @author DM8AT
 * @brief define a binding structure for the mesh pool, a structure that combines vertex and index storage in pools
 * @version 0.1
 * @date 2026-03-25
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHICS_MESH_POOL_
#define _GLGE_GRAPHIC_BACKEND_GRAPHICS_MESH_POOL_

//add common stuff
#include "Core/Common.h"
//add references
#include "Core/Reference.h"

//add vertex attributes
#include "Graphic/VertexAttribute.h"

//frontend instances are defined elsewhere
namespace GLGE::Graphic
{class Instance;}

//use the backend namespace
namespace GLGE::Graphic::Backend::Graphic {

    /**
     * @brief define a structure that stores mesh metadata in pools
     * 
     * The exact storage implementations are defined by the backend. 
     * The mesh pool may be implemented using ANY layout that fits best as long as all interface methods are implemented. 
     */
    class MeshPool : public GLGE::Referable {
    public:

        /**
         * @brief store the maximum allowed attribute count
         */
        inline static constexpr const u8 MAX_ATTRIBUTE_COUNT = 16;

        /**
         * @brief define the maximum amount of LODs a single mesh may have
         */
        inline static constexpr size_t MAX_LOD_COUNT = 12;

        /**
         * @brief store storage information about a level of detail
         */
        struct LODInfo {
            /**
             * @brief store a section of an input buffer
             */
            struct Section {
                /**
                 * @brief store the amount of stored elements
                 */
                size_t count;
                /**
                 * @brief the byte offset from the inputted buffer start
                 */
                size_t offset;
                /**
                 * @brief the byte size of a single element stored in the section
                 */
                size_t size;
            };

            /**
             * @brief store the section of the vertex buffer that belongs to this LOD
             */
            Section vertex;
            /**
             * @brief store the section of the index buffer that belongs to this LOD
             */
            Section index;
        };

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
             * @brief store all vertex attributes
             */
            VertexAttribute attributes[MAX_ATTRIBUTE_COUNT]{};
            /**
             * @brief store the amount of used vertex attributes
             */
            u64 attributeCount = 0;

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
         * @brief Construct a new Mesh Pool
         * 
         * @param instance a pointer to the instance the mesh pool will belong to
         */
        MeshPool(GLGE::Graphic::Instance* instance)
         : m_instance(instance)
        {}

        /**
         * @brief Destroy the Mesh Pool
         */
        virtual ~MeshPool() = default;

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
         * @param attributes a pointer to a continues array of attributes the mesh should use
         * @param attributeCount the amount of vertex attributes of the mesh
         * 
         * @return `u64` the mesh ID that can be used to quarry general mesh metadata from the mesh metadata buffer
         */
        virtual u64 allocate(const void* vertices, size_t vertexSize, size_t vertexCount, const u32* indices, size_t indexCount, const LODInfo* lod, u8 LODCount, const VertexAttribute* attributes, u64 attributeCount) = 0;

        /**
         * @brief get the amount of LODs for a specific mesh
         * 
         * @param meshId the ID of the mesh to quarry the LOD count from
         * @return `u8` the amount of LODs the mesh has
         */
        virtual u8 getLODCount(u64 meshId) const = 0;

        /**
         * @brief get information about the LOD structures of a specific mesh
         * 
         * @param meshId the ID of the mesh to get the LOD informations from
         * @param lod the index of the level of detail to get
         * @return `LODInfo` informations about the LODs of the mesh
         */
        virtual LODInfo getLODInfo(u64 meshId, u8 lod) const = 0;

        /**
         * @brief Get the amount of vertices of a specific mesh
         * 
         * @param meshId the ID of the mesh to get the vertex count from
         * @return `u64` the amount of vertices the mesh has
         */
        virtual u64 getVertexCount(u64 meshId) const = 0;

        /**
         * @brief Get the vertex data of a specific mesh
         * 
         * @param meshId the ID of the mesh to get the vertex data from
         * @return `const void*` the constant vertex data of the mesh
         */
        virtual const void* getVertexData(u64 meshId) const = 0;

        /**
         * @brief Get the vertex size in bytes of a specific mesh
         * 
         * This refers to the size of a SINGLE vertex, not the VBO size. 
         * 
         * @param meshId the ID of the mesh to get the vertex size from
         * @return `u64` the vertex size in bytes
         */
        virtual u64 getVertexSize(u64 meshId) const = 0;

        /**
         * @brief Get the index count of a specific mesh
         * 
         * @param meshId the ID of the mesh to get the index count from
         * @return `u64` the amount of indices the mesh has
         */
        virtual u64 getIndexCount(u64 meshId) const = 0;

        /**
         * @brief Get the index data from the mesh
         * 
         * @param meshId the ID of the mesh to get the index data from
         * @return `const void*` the constant index data of the mesh
         */
        virtual const void* getIndexData(u64 meshId) const = 0;

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
        virtual u8 getIndexTypeSize(u64 meshId) const = 0;

        /**
         * @brief Get the Index Section
         * 
         * @param meshId the ID of the mesh to get the whole index section from
         * @return `const LODInfo::Section&` a constant reference to the whole index section
         */
        virtual const LODInfo::Section& getIndexSection(u64 meshId) const = 0;

        /**
         * @brief Get the Vertex Section
         * 
         * @param meshId the ID of the mesh to get the whole vertex section from
         * @return `const LODInfo::Section&` a constant reference to the whole vertex section
         */
        virtual const LODInfo::Section& getVertexSection(u64 meshId) const = 0;

        /**
         * @brief Get the vertex attribute count
         * 
         * @param meshId the ID of the mesh to get the attribute count from
         * @return `u64` the amount of vertex attributes the mesh uses
         */
        virtual u64 getVertexAttributeCount(u64 meshId) const = 0;

        /**
         * @brief Get a specific vertex attribute from a mesh
         * 
         * @param meshId the mesh ID of the mesh to get the attribute from
         * @param attributeId the index of the attribute to quarry
         * @return `VertexAttribute` the vertex attribute of the mesh at the ID
         */
        virtual VertexAttribute getVertexAttribute(u64 meshId, u64 attributeId) const = 0;

        /**
         * @brief destroy a specific mesh
         * 
         * @param meshId the ID of the mesh to destroy
         */
        virtual void destroy(u64 meshId) = 0;

    protected:

        /**
         * @brief store a pointer to the owning backend instance
         */
        GLGE::Graphic::Instance* m_instance = nullptr;

    };

}

#endif