/**
 * @file GeometryPoolStream.h
 * @author DM8AT
 * @brief define the geometry pool stream
 * @version 0.1
 * @date 2026-06-12
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_GEOMETRY_POOL_STREAM_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_GEOMETRY_POOL_STREAM_

//add the geometry pool
#include "Graphic/Backend/Graphics/GeometryPool.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::Vulkan {

    /**
     * @brief define a geometry pool stream class
     */
    class GeometryPoolStream : public GeometryPool::Stream {
    public:

        /**
         * @brief a region is the result of an allocation of the stream
         */
        using Region = GeometryPool::Region;

        /**
         * @brief Construct a new Geometry Pool Stream
         * 
         * @param size the inital size of the Stream
         * @param isIbo `true` if this is an index buffer, `false` for a vertex buffer
         */
        GeometryPoolStream(u64 size, bool isIbo);

        /**
         * @brief Destroy the Geometry Pool Stream
         */
        virtual ~GeometryPoolStream() override;

        /**
         * @brief flush the newly written data to the GPU
         * 
         * Depending on the backend this may be required or not, so make sure to always use it. The backend decides if it is required or not. 
         */
        virtual void onFlush() override;

    protected:

        /**
         * @brief a dirty range in the staging buffer
         */
        struct DirtyRange {
            /**
             * @brief the byte offset of the dirty range
             */
            u64 offset = 0;
            /**
             * @brief the size of the dirty range
             */
            u64 size = 0;
        };

        /**
         * @brief a function that is called when the buffer is resized
         * 
         * @param newSize the new size of the Stream
         */
        virtual void onResize(u64 newSize) override;

        /**
         * @brief a function that is called on writing
         * 
         * @param region the region to write to
         * @param data a pointer to the data to write
         * @param offset the region-internal offset of the data
         * @param size the size of the data
         */
        virtual void onWrite(Region region, const void* data, u64 offset, u64 size) override;

        /**
         * @brief wait for a previous upload to finish
         *
         * This is primarily used before resources are destroyed or replaced.
         */
        void waitForUpload();

        /**
         * @brief destroy the current GPU resources
         */
        void destroyResources();

        /**
         * @brief merge a new dirty range into the dirty range list
         *
         * @param offset beginning of the range
         * @param size size of the range
         */
        void addDirtyRange(
            VkDeviceSize offset,
            VkDeviceSize size
        );

        //NOTE: All types are "vulkan-erased" to ensure that no vulkan-types are leaked. 

        /**
         * @brief store the vulkan device
         */
        void* m_device = nullptr;
        /**
         * @brief store the own allocator
         */
        void* m_allocator = nullptr;
        /**
         * @brief store the own buffer
         */
        void* m_buffer = nullptr;
        /**
         * @brief store the allocation for the buffer
         */
        void* m_allocation = nullptr;
        /**
         * @brief store the staging buffer
         */
        void* m_stagingBuffer = nullptr;
        /**
         * @brief store the allocation of the staging buffer
         */
        void* m_stagingBufferAlloc = nullptr;
        /**
         * @brief store a pointer into the mapped staging buffer
         */
        void* m_mapped = nullptr;
        /**
         * @brief store what ranges are dirty
         */
        std::vector<DirtyRange> m_dirtyRanges;
        /**
         * @brief store the command buffer used to upload
         */
        void* m_uploadCmdBuff = nullptr;
        /**
         * @brief fence to enforce syncing
         */
        void* m_fence = nullptr;
        /**
         * @brief store if an upload is in flight
         */
        bool m_uploadInFlight = false;
    };

}

#endif