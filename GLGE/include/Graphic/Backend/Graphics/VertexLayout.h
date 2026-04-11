/**
 * @file VertexLayout.h
 * @author DM8AT
 * @brief define the backends for vertex related backends
 * @version 0.1
 * @date 2026-03-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHICS_VERTEX_
#define _GLGE_GRAPHIC_BACKEND_GRAPHICS_VERTEX_

//add common stuff
#include "Core/Common.h"
//add vertex attributes
#include "Graphic/VertexAttribute.h"
//add mesh pools
#include "MeshPool.h"
//include instances
#include "Instance.h"

//use the backend namespace
namespace GLGE::Graphic::Backend::Graphic {

    /**
     * @brief define the backend for a vertex layout
     */
    class VertexLayout : public Referable {
    public:

        /**
         * @brief store the maximum allowed attribute count
         */
        inline static constexpr const u8 MAX_ATTRIBUTE_COUNT = 16;

        /**
         * @brief Construct a new Vertex Layout
         * 
         * @param ptr the pointer to the beginning of a continues array of attributes
         * @param size the amount of attributes in the array
         * @param stride the byte size of a single vertex
         * @param pool a reference to the mesh pool the layout attaches to
         */
        VertexLayout(const VertexAttribute* ptr, size_t size, size_t stride, Reference<GLGE::Graphic::Backend::Graphic::MeshPool> pool) 
         : m_stride(stride), m_pool(pool), m_attributeCount(size)
        {
            //copy over the data
            for (size_t i = 0; i < size; ++i)
            {m_attributes[i] = ptr[i];}
        }

        /**
         * @brief Get a specific attribute
         * @param index the storage index of the attribute to get
         * @return `const VertexAttribute&` a constant reference to the attribute
         */
        inline const VertexAttribute& getAttribute(u8 index) const noexcept
        {return m_attributes[index];}

        /**
         * @brief Get the amount of used attributes
         * 
         * @return `u64` the amount of attributes in use
         */
        u64 getAttributeCount() const noexcept
        {return m_attributeCount;}
        
        /**
         * @brief bind the VAO
         * 
         * @param offset the byte offset of the VBO from the start
         */
        virtual void bind(size_t offset) = 0;

    protected:

        /**
         * @brief store the size in bytes of a whole vertex
         */
        size_t m_stride = 0;
        /**
         * @brief store all vertex attributes
         */
        VertexAttribute m_attributes[MAX_ATTRIBUTE_COUNT];
        /**
         * @brief store the amount of used attributes
         */
        u64 m_attributeCount = 0;
        /**
         * @brief store a reference to the mesh pool to use
         */
        Reference<GLGE::Graphic::Backend::Graphic::MeshPool> m_pool;

    };

}

#endif