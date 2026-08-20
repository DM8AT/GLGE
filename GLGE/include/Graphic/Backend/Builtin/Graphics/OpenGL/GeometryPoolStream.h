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
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_GEOMETRY_POOL_STREAM_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_GEOMETRY_POOL_STREAM_

//add the geometry pool
#include "Graphic/Backend/Graphics/GeometryPool.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::OpenGL {

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
         * @brief store the buffer
         */
        u32 m_buff = 0;
        /**
         * @brief store the target type
         */
        u32 m_target = 0;
        /**
         * @brief store the mapped data
         */
        void* m_mapped = nullptr;

    };

}

#endif