/**
 * @file VertexAttribute.h
 * @author DM8AT
 * @brief define what a vertex attribute is
 * @version 0.1
 * @date 2026-03-25
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_VERTEX_ATTRIBUTE_
#define _GLGE_GRAPHIC_VERTEX_ATTRIBUTE_

//add common stuff
#include "Core/Common.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief store a single element of a vertex
     */
    class VertexAttribute {
    public:

        /**
         * @brief store the intention of the attribute
         */
        enum class Type : u8 {
            /**
             * @brief something custom, will be ignored by build-in stuff
             */
            Custom = 0,
            /**
             * @brief the element is a position
             */
            Position,
            /**
             * @brief the element is a normal vector
             */
            Normal,
            /**
             * @brief the element is a tangent vector
             */
            Tangent,
            /**
             * @brief the element is a bi-tangent vector
             */
            Bitangent,
            /**
             * @brief the element stores UV texture coordinates, indexed as 0 in the shader
             */
            UV_0,
            /**
             * @brief the element stores UV texture coordinates, indexed as 1 in the shader
             */
            UV_1,
            /**
             * @brief the element stores UV texture coordinates, indexed as 2 in the shader
             */
            UV_2,
            /**
             * @brief the element stores UV texture coordinates, indexed as 3 in the shader
             */
            UV_3,
            /**
             * @brief the element stores vertex colors, indexed as 0 in the shader
             */
            Color_0,
            /**
             * @brief the element stores vertex colors, indexed as 1 in the shader
             */
            Color_1,
            /**
             * @brief the element stores vertex colors, indexed as 2 in the shader
             */
            Color_2,
            /**
             * @brief the element stores vertex colors, indexed as 3 in the shader
             */
            Color_3,

            /**
             * @brief UV is an alias for the first index of the uv intent set
             */
            UV = UV_0,
            /**
             * @brief Color is an alias for the first index of the color intent set
             */
            Color = Color_0
        };

        /**
         * @brief store the type of the stored data
         */
        enum class Format : u8 {
            /**
             * @brief the data is a signed 32 bit integer
             */
            Int = 0,
            /**
             * @brief the data is an unsigned 32 bit integer
             */
            UInt,
            /**
             * @brief the data is a single IEEE 32 bit float
             */
            Float,

            /**
             * @brief the data is a 2D vector of IEEE floats
             */
            vec2,
            /**
             * @brief the data is a 3D vector of IEEE floats
             */
            vec3,
            /**
             * @brief the data is a 4D vector of IEEE floats
             */
            vec4,

            /**
             * @brief the data is a 2D vector of 32 bit signed integers
             */
            ivec2,
            /**
             * @brief the data is a 3D vector of 32 bit signed integers
             */
            ivec3,
            /**
             * @brief the data is a 4D vector of 32 bit signed integers
             */
            ivec4,

            /**
             * @brief the data is a 2D vector of 32 bit unsigned integers
             */
            uvec2,
            /**
             * @brief the data is a 3D vector of 32 bit unsigned integers
             */
            uvec3,
            /**
             * @brief the data is a 4D vector of 32 bit unsigned integers
             */
            uvec4,

            /**
             * @brief the data is a single normalized unsigned 8 bit integer (0 = 0, 255 = 1)
             */
            unorm_u8x1,
            /**
             * @brief the data is a 2D vector of normalized unsigned 8 bit integer (0 = 0, 255 = 1)
             */
            unorm_u8x2,
            /**
             * @brief the data is a 4D vector of normalized unsigned 8 bit integer (0 = 0, 255 = 1)
             */
            unorm_u8x4,
            /**
             * @brief the data is a single normalized signed 8 bit integer (-128 = -1, 127 = 1)
             */
            snorm_u8x1,
            /**
             * @brief the data is a 2D vector of normalized signed 8 bit integer (-128 = -1, 127 = 1)
             */
            snorm_u8x2,
            /**
             * @brief the data is a 4D vector of normalized signed 8 bit integer (-128 = -1, 127 = 1)
             */
            snorm_u8x4
        };

        /**
         * @brief Construct a new Vertex Attribute
         */
        VertexAttribute() = default;

        /**
         * @brief Construct a new Vertex Attribute
         * 
         * @param type the type of vertex attribute this is
         * @param format the format of the vertex attribute
         * @param offset the byte offset from the start of the vertex structure
         * @param binding the binding point to bind the element to in the shader
         */
        VertexAttribute(Type type, Format format, size_t offset, u8 binding)
         : m_type(type), m_format(format), m_offset(offset), m_binding(binding)
        {}

        /**
         * @brief Get the Type
         * 
         * @return `Type` the type of the vertex attribute
         */
        inline Type getType() const noexcept
        {return m_type;}

        /**
         * @brief Get the Format
         * 
         * @return `Format` the format of the vertex attribute
         */
        inline Format getFormat() const noexcept
        {return m_format;}
    
        /**
         * @brief Get the Offset
         * 
         * @return `size_t` the offset in bytes from the start of the vertex structure
         */
        inline size_t getOffset() const noexcept
        {return m_offset;}

        /**
         * @brief Get the Binding
         * 
         * @return `u8` the binding point of the attribute in the shader
         */
        inline u8 getBinding() const noexcept
        {return m_binding;}

        /**
         * @brief check if the vertex attribute is valid
         * 
         * @return `true` if the attribute is valid, `false` if it is not
         */
        inline bool valid() const noexcept
        {return !((m_binding == UINT8_MAX) && (m_offset == SIZE_MAX));}

        /**
         * @brief Get the Size Of a Format
         * 
         * @param form the format to get the size for
         * @return `u64` the byte size of the format
         */
        inline static u64 getSizeOfFormat(Format form) noexcept {
            //for each format store the corresponding size
            static u64 sizes[] = {
                4,  4,  4, //normal types
                8, 12, 16, //float vectors
                8, 12, 16, //integer vectors
                8, 12, 16, //unsigned integer vectors
                1,  2,  4, //packed unorm vectors
                1,  2,  4  //packed snorm vectors
            };
            //use a cast to integer + list lookup to determine the size
            return sizes[static_cast<u8>(form)];
        }

    protected:

        /**
         * @brief store the type of the vertex attribute
         */
        Type m_type = Type::Custom;
        /**
         * @brief store the format of the vertex attribute
         */
        Format m_format = Format::Int;
        /**
         * @brief store the byte offset from the start of the vertex
         */
        size_t m_offset = SIZE_MAX;
        /**
         * @brief store the binding location in the shader
         */
        u8 m_binding = UINT8_MAX;

    };

}

#endif