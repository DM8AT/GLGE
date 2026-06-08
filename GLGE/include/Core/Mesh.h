/**
 * @file Mesh.h
 * @author DM8AT
 * @brief define what a mesh is
 * @version 0.1
 * @date 2026-06-05
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_CORE_MESH_
#define _GLGE_CORE_MESH_

//add common stuff
#include "Common.h"
//add exceptions
#include "Exception.h"
//for type reflection
#include "TypeInfo.h"
//add AABBs
#include "AABB.h"
//add base classes
#include "BaseClass.h"

//add type info for type information
#include <typeinfo>

//use the library namespace
namespace GLGE {

    /**
     * @brief a namespace that stores all vertex attribute tag types
     */
    namespace VertexAttribute {

        /**
         * @brief define that a vertex attribute is incomplete / undefined
         */
        struct Undefined {
            /**
             * @brief store an unused blob
             * 
             * This is required for MSVC
             */
            u8 _unused;
        };

        /**
         * @brief define that an attribute is a position
         */
        struct Position {
            /**
             * @brief store an unused blob
             * 
             * This is required for MSVC
             */
            u8 _unused;
        };

        /**
         * @brief define that an attribute is a normal
         */
        struct Normal {
            /**
             * @brief store an unused blob
             * 
             * This is required for MSVC
             */
            u8 _unused;
        };

        /**
         * @brief define that an attribute is a tangent
         */
        struct Tangent {
            /**
             * @brief store an unused blob
             * 
             * This is required for MSVC
             */
            u8 _unused;
        };

        /**
         * @brief define that an attribute is a bitangent
         */
        struct Bitangent {
            /**
             * @brief store an unused blob
             * 
             * This is required for MSVC
             */
            u8 _unused;
        };

    }

    /**
     * @brief define a triangle as indices
     */
    struct Triangle {
        /**
         * @brief store the first index
         */
        u32 a = 0;
        /**
         * @brief store the second index
         */
        u32 b = 0;
        /**
         * @brief store the third index
         */
        u32 c = 0;
    };

    /**
     * @brief a class that stores geometric information
     */
    class Mesh : public BaseClass {
    public:

        /**
         * @brief define the layout of a single vertex
         * 
         * An attribute count of 0 marks an invalid / uninitialized object
         * 
         * @warning Attributes are ALLOWED to overlap. If they do, the type validity of the overlap is in the user's hands. DO NOT USE UNLESS YOU KNOW WHAT YOU'RE DOING. Alignment rues are still enforced on overlapping attributes. 
         */
        class VertexLayout {
        public:

            /**
             * @brief store the data of a single vertex attribute
             * 
             * A vertex attribute describes how a subregion of a vertex is structured and what the intended meaning of the data is. 
             */
            struct VertexAttribute {
                        
                /**
                 * @brief store the type of the stored data
                 */
                enum class Type : u8 {
                    /**
                     * @brief say that a vertex attribute is unused
                     */
                    Unused = 0,

                    /**
                     * @brief the data is a signed 32 bit integer
                     */
                    Int,
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
                 * @brief check if an inputted type is of the expected format
                 * 
                 * This only checks for exact matches
                 * 
                 * @tparam T the type to check for
                 * @param type the type enum value to check for
                 * @return `true` if the type was expected, `false` if not
                 */
                template <typename T>
                inline static bool isValidType(Type type) noexcept {
                    switch (type) {
                    case Type::Int:       return std::is_same_v<T, i32>;
                    case Type::UInt:      return std::is_same_v<T, u32>;
                    case Type::Float:     return std::is_same_v<T, f32>;
                    case Type::vec2:      return std::is_same_v<T, GLGE::vec2>;
                    case Type::vec3:      return std::is_same_v<T, GLGE::vec3>;
                    case Type::vec4:      return std::is_same_v<T, GLGE::vec4>;
                    case Type::ivec2:     return std::is_same_v<T, GLGE::ivec2>;
                    case Type::ivec3:     return std::is_same_v<T, GLGE::ivec3>;
                    case Type::ivec4:     return std::is_same_v<T, GLGE::ivec4>;
                    case Type::uvec2:     return std::is_same_v<T, GLGE::uvec2>;
                    case Type::uvec3:     return std::is_same_v<T, GLGE::uvec3>;
                    case Type::uvec4:     return std::is_same_v<T, GLGE::uvec4>;
                    case Type::unorm_u8x1:return std::is_same_v<T, GLGE::u8>;
                    case Type::unorm_u8x2:return std::is_same_v<T, GLGE::u8vec2_p>;
                    case Type::unorm_u8x4:return std::is_same_v<T, GLGE::u8vec4_p>;
                    case Type::snorm_u8x1:return std::is_same_v<T, GLGE::i8>;
                    case Type::snorm_u8x2:return std::is_same_v<T, GLGE::i8vec2_p>;
                    case Type::snorm_u8x4:return std::is_same_v<T, GLGE::i8vec4_p>;
                    
                    //default: invalid
                    default: return false;
                    }
                }

                /**
                 * @brief store information about a specific type
                 */
                struct TypeInfo {
                    /**
                     * @brief store the size of the type
                     */
                    size_t size = 0;
                    /**
                     * @brief store the alignment of the type
                     */
                    size_t alignment = 0;
                    /**
                     * @brief store if the type is constructed from multiple sub-types
                     * 
                     * e.g. `vec3`: true (3 floats), `float`: false, just a float
                     */
                    bool isCompound = false;
                    /**
                     * @brief store if the type should be treated as normalized
                     * 
                     * This identifies the unorm and snorm types
                     */
                    bool isNormalized = false;
                    /**
                     * @brief store if the type supports negative numbers
                     * 
                     * This does NOT state how they are supported, just that they are. 
                     * The implementation of the type may use a sign bit (floats) or two's complement (typical for integers)
                     */
                    bool isSigned = false;
                };

                /**
                 * @brief Get information about a specific type
                 * 
                 * @param type the type to get information for
                 * @return `TypeInfo` information about the type
                 */
                inline static TypeInfo getTypeInfo(Type type) noexcept {
                    switch (type) {
                    case Type::Int:       return TypeInfo {
                        .size = sizeof(i32),
                        .alignment = alignof(i32),
                        .isCompound = false,
                        .isNormalized = false,
                        .isSigned = true
                    };
                    case Type::UInt:      return TypeInfo {
                        .size = sizeof(u32),
                        .alignment = alignof(u32),
                        .isCompound = false,
                        .isNormalized = false,
                        .isSigned = false
                    };
                    case Type::Float:     return TypeInfo {
                        .size = sizeof(f32),
                        .alignment = alignof(f32),
                        .isCompound = false,
                        .isNormalized = false,
                        .isSigned = true
                    };
                    case Type::vec2:      return TypeInfo {
                        .size = sizeof(GLGE::vec2),
                        .alignment = alignof(GLGE::vec2),
                        .isCompound = true,
                        .isNormalized = false,
                        .isSigned = true
                    };
                    case Type::vec3:      return TypeInfo {
                        .size = sizeof(GLGE::vec3),
                        .alignment = alignof(GLGE::vec3),
                        .isCompound = true,
                        .isNormalized = false,
                        .isSigned = true
                    };
                    case Type::vec4:      return TypeInfo {
                        .size = sizeof(GLGE::vec4),
                        .alignment = alignof(GLGE::vec4),
                        .isCompound = true,
                        .isNormalized = false,
                        .isSigned = true
                    };
                    case Type::ivec2:     return TypeInfo {
                        .size = sizeof(GLGE::ivec2),
                        .alignment = alignof(GLGE::ivec2),
                        .isCompound = true,
                        .isNormalized = false,
                        .isSigned = true
                    };
                    case Type::ivec3:     return TypeInfo {
                        .size = sizeof(GLGE::ivec3),
                        .alignment = alignof(GLGE::ivec3),
                        .isCompound = true,
                        .isNormalized = false,
                        .isSigned = true
                    };
                    case Type::ivec4:     return TypeInfo {
                        .size = sizeof(GLGE::ivec4),
                        .alignment = alignof(GLGE::ivec4),
                        .isCompound = true,
                        .isNormalized = false,
                        .isSigned = true
                    };
                    case Type::uvec2:     return TypeInfo {
                        .size = sizeof(GLGE::uvec2),
                        .alignment = alignof(GLGE::uvec2),
                        .isCompound = true,
                        .isNormalized = false,
                        .isSigned = false
                    };
                    case Type::uvec3:     return TypeInfo {
                        .size = sizeof(GLGE::uvec3),
                        .alignment = alignof(GLGE::uvec3),
                        .isCompound = true,
                        .isNormalized = false,
                        .isSigned = false
                    };
                    case Type::uvec4:     return TypeInfo {
                        .size = sizeof(GLGE::uvec4),
                        .alignment = alignof(GLGE::uvec4),
                        .isCompound = true,
                        .isNormalized = false,
                        .isSigned = false
                    };
                    case Type::unorm_u8x1:return TypeInfo {
                        .size = sizeof(GLGE::u8),
                        .alignment = alignof(GLGE::u8),
                        .isCompound = false,
                        .isNormalized = true,
                        .isSigned = false
                    };
                    case Type::unorm_u8x2:return TypeInfo {
                        .size = sizeof(GLGE::u8vec2_p),
                        .alignment = alignof(GLGE::u8vec2_p),
                        .isCompound = true,
                        .isNormalized = true,
                        .isSigned = false
                    };
                    case Type::unorm_u8x4:return TypeInfo {
                        .size = sizeof(GLGE::u8vec4_p),
                        .alignment = alignof(GLGE::u8vec4_p),
                        .isCompound = true,
                        .isNormalized = true,
                        .isSigned = false
                    };
                    case Type::snorm_u8x1:return TypeInfo {
                        .size = sizeof(GLGE::i8),
                        .alignment = alignof(GLGE::i8),
                        .isCompound = false,
                        .isNormalized = true,
                        .isSigned = true
                    };
                    case Type::snorm_u8x2:return TypeInfo {
                        .size = sizeof(GLGE::i8vec2_p),
                        .alignment = alignof(GLGE::i8vec2_p),
                        .isCompound = true,
                        .isNormalized = true,
                        .isSigned = true
                    };
                    case Type::snorm_u8x4:return TypeInfo {
                        .size = sizeof(GLGE::i8vec4_p),
                        .alignment = alignof(GLGE::i8vec4_p),
                        .isCompound = true,
                        .isNormalized = true,
                        .isSigned = true
                    };
                    
                    //default: invalid / unused
                    default: return TypeInfo {};
                    }

                }

                /**
                 * @brief Get the Vertex Alignment
                 * 
                 * This function defines to what a vertex must be aligned
                 * 
                 * @return `size_t` the required alignment of a vertex. The vertex size must fullfil `size % getVertexAlignment() == 0`. 
                 */
                inline static constexpr size_t getVertexAlignment() noexcept
                {return 16;}

                /**
                 * @brief store the type of the attribute
                 * 
                 * This is also used to infer the size of the attribute
                 */
                Type type = Type::Unused;
                /**
                 * @brief store the usage of the attribute
                 * 
                 * This is stored as a 64 byte hash of a human-readable type tag
                 */
                u64 usage = getTypeHash64<GLGE::VertexAttribute::Undefined>();
                /**
                 * @brief store the offset in bytes from the vertex start
                 */
                u64 offset = 0;

                /**
                 * @brief Construct a new Vertex Attribute
                 */
                VertexAttribute() = default;

                /**
                 * @brief Construct a new Vertex Attribute
                 * 
                 * @param _type the type of the attribute's data
                 * @param _usage the usage of the attribute
                 * @param _offset the offset of the attribute's data from the start of the vertex
                 */
                VertexAttribute(Type _type, u64 _usage, u64 _offset)
                 : type(_type), usage(_usage), offset(_offset)
                {}

                /**
                 * @brief Construct a new Vertex Attribute
                 * 
                 * @tparam T the usage type of the vertex attribute
                 * @param _type the type of the attribute's data
                 * @param _tag a tag type instance to extract the hash from
                 * @param _offset the offset of the attribute's data from the start of the vertex
                 */
                template <typename T>
                VertexAttribute(Type _type, [[maybe_unused]] T _tag, u64 _offset)
                 : type(_type), usage(getTypeHash64<T>()), offset(_offset)
                {}
    
            };

            /**
             * @brief use the same type enum as the attribute
             */
            using Type = VertexAttribute::Type;

            /**
             * @brief Construct a new Vertex Layout
             * 
             * @param attributes an initializer list defining all attributes
             * @param size the size of a single vertex (0 = automatic)
             */
            VertexLayout(std::initializer_list<VertexAttribute> attributes, u64 size = 0)
             : VertexLayout(attributes.begin(), attributes.size(), size)
            {}

            /**
             * @brief Construct a new Vertex Layout
             * 
             * @param attributes a vector containing all attributes
             * @param size the size of a single vertex (0 = automatic)
             */
            VertexLayout(const std::vector<VertexAttribute>& attributes, u64 size = 0)
             : VertexLayout(attributes.data(), attributes.size(), size)
            {}

            /**
             * @brief Construct a new Vertex Layout object
             * 
             * @param attr_ptr a pointer to the vertex attribute data
             * @param attrCount the amount of vertex attributes
             * @param size the size of a single vertex (0 = automatic)
             */
            VertexLayout(const VertexAttribute* attr_ptr, u64 attrCount, u64 size = 0) 
             : m_size(size), m_attributes(attr_ptr, attr_ptr + attrCount)
            {
                //validate that this layout does not break alignment rules
                if ((m_size % VertexAttribute::getVertexAlignment() != 0) && m_size) {
                    //clean up
                    m_size = 0;
                    m_attributes.clear();
                    //throw an exception
                    throw GLGE::Exception("Failed to create a vertex layout: The vertex layout breaks vertex size alignment rules", "GLGE::Mesh::VertexLayout::VertexLayout");
                }

                //store the required size
                size_t reqSize = 0;
                //make sure that no duplicates exist
                for (size_t i = 0; i < m_attributes.size(); ++i) {
                    //get the attribute
                    const auto& attr = m_attributes[i];

                    //reflect the type
                    auto ref = VertexAttribute::getTypeInfo(attr.type);
                    //compute the required vertex size
                    size_t req = attr.offset + ref.size;
                    //sanity check it (if the size is not automatic)
                    if ((req > size) && m_size) {
                        //invalidate self
                        m_size = 0;
                        m_attributes.clear();
                        //throw an exception
                        throw GLGE::Exception("Failed to create vertex layout: Found an attribute that does not fit into the specified vertex size", "GLGE::Mesh::VertexLayout::VertexLayout");
                    }
                    //store the required size if it is larger than the current one
                    if (req > reqSize) 
                    {reqSize = req;}

                    //sanity check alignment
                    if (attr.offset % ref.alignment) {
                        //invalidate self
                        m_size = 0;
                        m_attributes.clear();
                        //throw an exception
                        throw GLGE::Exception("Failed to create vertex layout: Found a misaligned attribute", "GLGE::Mesh::VertexLayout::VertexLayout");
                    }

                    //validate unique usage
                    for (size_t j = i+1; j < m_attributes.size(); ++j) {
                        if (m_attributes[j].usage == attr.usage) {
                            //invalidate self
                            m_size = 0;
                            m_attributes.clear();
                            //throw an exception
                            throw GLGE::Exception("Failed to create vertex layout: Multiple attributes with the same usage defined", "GLGE::Mesh::VertexLayout::VertexLayout");
                        }
                    }
                }

                //if the size is automatic, align the requirement up and store it
                if (!static_cast<bool>(m_size)) {
                    //align up
                    reqSize = ((reqSize + VertexAttribute::getVertexAlignment() - 1) / VertexAttribute::getVertexAlignment()) * VertexAttribute::getVertexAlignment();
                    //store the size
                    m_size = reqSize;
                }

                //make sure a position attribute exists (a position is the only required attribute)
                if (!hasUsage<GLGE::VertexAttribute::Position>()) {
                    //invalidate self
                    m_size = 0;
                    m_attributes.clear();
                    //throw an exception
                    throw GLGE::Exception("Failed to create the vertex layout: No position attribute was provided, but positions are required", "GLGE::Mesh::VertexLayout::VertexLayout");
                }
            }

            /**
             * @brief Get the Attribute Count
             * 
             * @warning This is NOT the total amount of USED attributes. There may be unused elements in between slots. 
             * 
             * @return `size_t` the amount of attributes allocated for the layout
             */
            inline size_t getAttributeCount() const noexcept
            {return m_attributes.size();}

            /**
             * @brief Get the Attribute
             * 
             * @param idx the index to query the attribute from
             * @return `const VertexAttribute&` a constant reference to the vertex attribute
             */
            inline const VertexAttribute& getAttribute(size_t idx) const noexcept
            {return m_attributes[idx];}

            /**
             * @brief Get the Attribute
             * 
             * @param idx the index to query the attribute from
             * @return `const VertexAttribute&` a constant reference to the vertex attribute
             */
            inline const VertexAttribute& operator[](size_t idx) const noexcept
            {return getAttribute(idx);}

            /**
             * @brief Get the attribute index of an attribute that has a specific usage
             * 
             * @tparam T the usage of the attribute to query
             * @return `u64` the index of the attribute or `UINT64_MAX` if no attribute of that usage exists
             */
            template <typename T>
            inline u64 getIdxOfUsage() const noexcept {
                //try to find it
                for (size_t i = 0; i < m_attributes.size(); ++i) {
                    if (m_attributes[i].usage == getTypeHash64<T>())
                    {return i;}
                }
                //on failure, return UINT64_MAx
                return UINT64_MAX;
            }

            /**
             * @brief check if an element of the usage exists
             * 
             * @tparam T the usage type to check for
             * @return `true` if an attribute with that usage exists, `false` if not
             */
            template <typename T>
            inline bool hasUsage() const noexcept
            {return getIdxOfUsage<T>() != UINT64_MAX;}

            /**
             * @brief Get an attribute of a specific usage
             * 
             * @warning if the attribute is unknown, calling this function results in erroneous behaviour. 
             * 
             * @tparam T the usage type to get the attribute for
             * @return `const VertexAttribute&` a constant reference to the attribute of that type
             */
            template <typename T>
            inline const VertexAttribute& getAttribute() const 
            {return m_attributes[getIdxOfUsage<T>()];}

            /**
             * @brief Get the size of a single vertex
             * 
             * @return `u64` the size of a single vertex
             */
            inline u64 getSize() const noexcept
            {return m_size;}

        protected:

            /**
             * @brief store all attributes
             * 
             * The list is ordered, attribute 0 is index 0. If an attribute is not set, it is marked using `Type::Unused`. 
             */
            std::vector<VertexAttribute> m_attributes;
            /**
             * @brief store the size of a single vertex
             * 
             * This may be larger than the sum of all type sizes. 
             */
            u64 m_size = 0;

        };

        /**
         * @brief use the same type enum as the vertex layout
         * 
         * This defines format types, not mesh types
         */
        using Type = VertexLayout::Type;
        /**
         * @brief define an alias for vertex attributes
         */
        using VertexAttribute = VertexLayout::VertexAttribute;

        /**
         * @brief a helper class used to access a single vertex
         * 
         * @warning This class is non-owning
         */
        class Vertex {
        public:

            /**
             * @brief define the type used for differences
             */
            using difference_type   = ptrdiff_t;
            /**
             * @brief define the type used for values
             */
            using value_type        = Vertex;
            /**
             * @brief define the type used for pointers
             */
            using pointer           = Vertex*;
            /**
             * @brief define the type used for references
             */
            using reference         = Vertex&;
            /**
             * @brief define the iterator category
             */
            using iterator_category = std::random_access_iterator_tag;

            /**
             * @brief Construct a new Vertex
             * 
             * @warning the vertex will be invalid by default
             */
            Vertex() = default;

            /**
             * @brief Construct a new Vertex
             * 
             * @param data the pointer to the vertices data
             * @param layout a reference to the vertex's layout
             * @param end a pointer to the end
             */
            Vertex(void* data, const VertexLayout& layout, const void* end)
             : m_dataPtr(data), m_layout(&layout), m_end(end)
            {}

            /**
             * @brief Get the Layout of the vertex
             * 
             * @return `const VertexLayout&` a constant reference to the vertex
             */
            inline const VertexLayout& getLayout() const noexcept
            {return *m_layout;}

            /**
             * @brief get a specific attribute
             * 
             * For all valid layouts the alignments are valid. 
             * 
             * @tparam T the type to get it as
             * @tparam Usage the usage type to get
             * @return `T*` a pointer to the MUTABLE data
             */
            template <typename T, typename Usage>
            inline T* get() noexcept {
                //get the attribute index (for UB safety)
                u64 idx = m_layout->getIdxOfUsage<Usage>();
                //sanity check
                if (idx == UINT64_MAX) {return nullptr;}
                //it exists -> get the actual attribute
                const auto& attr = m_layout->getAttribute(idx);
                //if the usage is invalid, return nullptr
                if (!VertexLayout::VertexAttribute::isValidType<T>(attr.type))
                {return nullptr;}
                //extract the data pointer
                return reinterpret_cast<T*>(reinterpret_cast<u8*>(m_dataPtr) + attr.offset);
            }

            /**
             * @brief get a specific attribute
             * 
             * For all valid layouts the alignments are valid. 
             * 
             * @tparam T the type to get it as
             * @tparam Usage the usage type to get
             * @return `const T*` a constant pointer to the data
             */
            template <typename T, typename Usage>
            inline const T* get() const noexcept {
                //get the attribute index (for UB safety)
                u64 idx = m_layout->getIdxOfUsage<Usage>();
                //sanity check
                if (idx == UINT64_MAX) {return nullptr;}
                //it exists -> get the actual attribute
                const auto& attr = m_layout->getAttribute(idx);
                //if the usage is invalid, return nullptr
                if (!VertexLayout::VertexAttribute::isValidType<T>(attr.type))
                {return nullptr;}
                //extract the data pointer
                return reinterpret_cast<T*>(reinterpret_cast<u8*>(m_dataPtr) + attr.offset);
            }

            /**
             * @brief Get the Attribute
             * 
             * @tparam Usage the type to get the usage for
             * @return `const VertexLayout::VertexAttribute*` a constant pointer to the attribute
             */
            template<typename Usage>
            const VertexLayout::VertexAttribute* getAttribute() const {
                //get the attribute index (for UB safety)
                u64 idx = m_layout->getIdxOfUsage<Usage>();
                //sanity check
                if (idx == UINT64_MAX) {return nullptr;}
                //it exists, return a pointer to the actual attribute
                return &m_layout->getAttribute(idx);
            }

            /**
             * @brief a function to check if two vertices are identical
             * 
             * This uses pointer-based comparison, not element-based comparison
             * 
             * @param other the vertex to compare against
             * @return `true` if they are, `false` if not
             */
            inline bool operator==(const Vertex& other) const noexcept 
            {return m_dataPtr == other.m_dataPtr;}

            /**
             * @brief a function to check if two vertices are NOT identical
             * 
             * This uses pointer-based comparison, not element-based comparison
             * 
             * @param other the vertex to compare against
             * @return `true` if they are not identical, `false` if not
             */
            inline bool operator!=(const Vertex& other) const noexcept 
            {return m_dataPtr != other.m_dataPtr;}

            /**
             * @brief a function to check if a vertex iterator is below another
             * 
             * This uses pointer-based comparison, not element-based comparison
             * 
             * @param other the other element to compare against
             * @return `true` if this is below other, `false` for equal or above
             */
            inline bool operator<(const Vertex& other) const noexcept 
            {return m_dataPtr < other.m_dataPtr;}

            /**
             * @brief a function to check if a vertex iterator is below another
             * 
             * This uses pointer-based comparison, not element-based comparison
             * 
             * @param other the other element to compare against
             * @return `true` if this is below other, `false` for equal or above
             */
            inline bool operator<=(const Vertex& other) const noexcept 
            {return m_dataPtr <= other.m_dataPtr;}

            /**
             * @brief a function to check if a vertex iterator is above another
             * 
             * This uses pointer-based comparison, not element-based comparison
             * 
             * @param other the other element to compare against
             * @return `true` if this is above other, `false` for equal or below
             */
            inline bool operator>(const Vertex& other) const noexcept 
            {return m_dataPtr > other.m_dataPtr;}

            /**
             * @brief a function to check if a vertex iterator is below or equal to another
             * 
             * This uses pointer-based comparison, not element-based comparison
             * 
             * @param other the other element to compare against
             * @return `true` if this is below or equal to other, `false` for above
             */
            inline bool operator>=(const Vertex& other) const noexcept 
            {return m_dataPtr >= other.m_dataPtr;}

            /**
             * @brief increment the iterator
             * 
             * @return `Vertex&` a reference to this AFTER incrementing
             */
            inline Vertex& operator++() noexcept {
                //update, then return the reference
                m_dataPtr = reinterpret_cast<u8*>(m_dataPtr) + m_layout->getSize();
                return *this;
            }

            /**
             * @brief decrement the iterator
             * 
             * @return `Vertex&` a reference to this AFTER decrementing
             */
            inline Vertex& operator--() noexcept {
                //update, then return the reference
                m_dataPtr = reinterpret_cast<u8*>(m_dataPtr) - m_layout->getSize();
                return *this;
            }

            /**
             * @brief increment the iterator
             * 
             * @return `Vertex` a copy of this BEFORE incrementing
             */
            inline Vertex operator++(int) noexcept {
                //copy, update, then return the copy
                Vertex copy = *this;
                ++(*this);
                return copy;
            }

            /**
             * @brief decrement the iterator
             * 
             * @return `Vertex` a copy of this BEFORE decrementing
             */
            inline Vertex operator--(int) noexcept {
                //copy, update, then return the copy
                Vertex copy = *this;
                --(*this);
                return copy;
            }

            /**
             * @brief increment the iterator by a specific amount
             * 
             * @param count the amount to step up by
             * @return `Vertex&` a reference to this after incrementing
             */
            inline Vertex& operator+=(ptrdiff_t count) noexcept {
                //move the data by the amount to move times the size of a vertex
                m_dataPtr = reinterpret_cast<u8*>(m_dataPtr) + count * static_cast<ptrdiff_t>(m_layout->getSize());
                return *this;
            }

            /**
             * @brief decrement the iterator by a specific amount
             * 
             * @param count the amount to step down by
             * @return `Vertex&` a reference to this after decrementing
             */
            inline Vertex& operator-=(ptrdiff_t count) noexcept 
            {return (*this += -count);}

            /**
             * @brief increment the iterator by a specific amount
             * 
             * @param count the amount to increment it by
             * @return `Vertex` a copy of this BEFORE incrementing
             */
            inline Vertex operator+(ptrdiff_t count) const noexcept {
                //copy, move up, return the copy
                Vertex copy = *this;
                copy += count;
                return copy;
            }

            /**
             * @brief decrement the iterator by a specific amount
             * 
             * @param count the amount to decrement by
             * @return `Vertex` a copy of this before decrementing
             */
            inline Vertex operator-(ptrdiff_t count) const noexcept {
                //copy, move down, return the copy
                Vertex copy = *this;
                copy -= count;
                return copy;
            }

            /**
             * @brief compute the difference of two iterators
             * 
             * @param other the other iterator to compute the difference with
             * @return `ptrdiff_t` the difference of the iterator
             */
            inline ptrdiff_t operator-(const Vertex& other) const noexcept {
                //directly update, then return
                return (reinterpret_cast<const u8*>(m_dataPtr) - reinterpret_cast<const u8*>(other.m_dataPtr)) / static_cast<ptrdiff_t>(m_layout->getSize());
            }

            /**
             * @brief check if the iterator is at or above the end
             * 
             * @return `true` if the iterator is above or at the end, `false` if it is below the end
             */
            inline bool isEnd() const noexcept 
            {return m_dataPtr >= m_end;}

            /**
             * @brief check if the iterator is valid
             * 
             * @return `true` if the iterator is valid, `false` if not
             */
            inline bool isValid() const noexcept 
            {return m_dataPtr != nullptr && m_layout != nullptr && m_end != nullptr;}

            /**
             * @brief dereference the vector
             * 
             * @return `Vertex&` a reference to this
             */
            inline Vertex& operator*() noexcept 
            {return *this;}

            /**
             * @brief dereference the vector
             * 
             * @return `const Vertex&` a constant reference to this
             */
            inline const Vertex& operator*() const noexcept 
            {return *this;}

            /**
             * @brief get as a pointer
             * 
             * @return `Vertex*` a pointer to this
             */
            inline Vertex* operator->() noexcept 
            {return this;}

            /**
             * @brief get as a pointer
             * 
             * @return `const Vertex*` a constant pointer to this
             */
            inline const Vertex* operator->() const noexcept 
            {return this;}

        protected:

            /**
             * @brief store a pointer to the actual vertex data
             */
            void* m_dataPtr = nullptr;

            /**
             * @brief store the vertex layout
             */
            const VertexLayout* m_layout = nullptr;

            /**
             * @brief store a pointer to the end
             */
            const void* m_end = nullptr;

        };

        //LODs are defined later
        class LOD;

        /**
         * @brief define a general-purpose bounding volume hierarchy
         * 
         * this defines spacial relation data for a specific mesh
         */
        class BVH {
        public:

            /**
             * @brief store the maximum amount of children a node may have
             */
            inline static constexpr u64 MaxChildCount = 4;

            /**
             * @brief store a single node of the BVH
             * 
             * This has a size of 48 bytes (3/4th of a cache line on modern systems)
             */
            struct alignas(16) Node {
                /**
                 * @brief store the axis aligned bounding box of the node
                 * 
                 * AABBs must be 24 bytes in size
                 */
                AABB aabb;
                /**
                 * @brief store how many children are used
                 * 
                 * 0 means that this is a leaf node
                 */
                u16 childCount = 0;

                /**
                 * @brief padding to align data to 16 bytes
                 */
                u16 _padding = 0;

                /**
                 * @brief store all data for a leaf node
                 */
                struct Leaf {
                    /**
                     * @brief store the index into the triangle index index buffer that points at the start of the BVH's triangle index index section
                     */
                    u32 firstTriangleIdxIdx = 0;
                    /**
                     * @brief store the amount of triangles this node owns
                     */
                    u32 triangleCount = 0;
                };

                /**
                 * @brief store the data for the BVH
                 */
                union alignas(16) Data {
                    /**
                     * @brief store the leaf data
                     */
                    Leaf leaf;
                    /**
                     * @brief store all the children
                     */
                    u32 childrenIds[BVH::MaxChildCount] {};
                };

                /**
                 * @brief store the data
                 */
                Data data;
            };

            /**
             * @brief Construct a new BVH
             */
            BVH() = default;

            /**
             * @brief Construct a new BVH
             * 
             * @param lod a pointer to the level of detail to generate the BVH from
             * @param highQuality `true` to generate a high quality BVH, this may take longer in generation, but is faster in traversal. `false` to generate a lower-quality BVH. This is faster than high quality in generation, but slower in traversal. 
             * @param minLeafTriangles the minimum amount of triangles in a leaf node
             * @param maxLeafTriangles the maximum amount of triangles in a leaf node
             */
            BVH(LOD* lod, bool highQuality = true, u32 minLeafTriangles = 4, u32 maxLeafTriangles = 16);

            /**
             * @brief Construct a new BVH
             * 
             * @param lod a pointer to the level of detail that holds the actual triangle data
             * @param nodes the node list of the BVH
             * @param triangleIndexIdx the triangle index index list of the bvh
             */
            BVH(LOD* lod, const std::vector<Node>& nodes, const std::vector<u64>& triangleIndexIdx)
             : m_nodes(nodes), m_triangleIndexIdx(triangleIndexIdx), m_lod(lod)
            {}

            /**
             * @brief Get the Node Count
             * 
             * @return `size_t` the amount of nodes the BVH has
             */
            inline size_t getNodeCount() const noexcept
            {return m_nodes.size();}

            /**
             * @brief Get the Node
             * 
             * @param i the index of the node to get
             * @return `const Node&` a constant reference to the node
             */
            inline const Node& getNode(size_t i) const noexcept
            {return m_nodes[i];}

            /**
             * @brief Get the Nodes
             * 
             * @return `const std::vector<Node>&` a constant reference to the node vector
             */
            inline const std::vector<Node>& getNodes() const noexcept
            {return m_nodes;}

            /**
             * @brief Get the Triangle Index Index Count
             * 
             * @return `size_t` the amount of triangle index indices
             */
            inline size_t getTriangleIndexIndexCount() const noexcept
            {return m_triangleIndexIdx.size();}

            /**
             * @brief Get the Triangle Index Index
             * 
             * @param i the index of the triangle index index to get
             * @return `u64` the triangle index index at the position
             */
            inline u64 getTriangleIndexIndex(size_t i) const noexcept
            {return m_triangleIndexIdx[i];}

            /**
             * @brief Get the Triangle Index Indices
             * 
             * @return `const std::vector<u64>&` a constant reference to the triangle index indices
             */
            inline const std::vector<u64>& getTriangleIndexIndices() const noexcept
            {return m_triangleIndexIdx;}

            /**
             * @brief Set the Reference level of detail
             * 
             * @param lod the level of detail to reference
             */
            inline void setReferenceLOD(LOD* lod)
            {m_lod = lod;}

        protected:

            /**
             * @brief store all the nodes
             */
            std::vector<Node> m_nodes;
            /**
             * @brief store indices into the index buffer pointing to the starting index of the referenced triangle
             */
            std::vector<u64> m_triangleIndexIdx;

            /**
             * @brief store a pointer to the level of detail that stores the vertices and indices referenced by the BVH
             */
            LOD* m_lod = nullptr;

        };

        /**
         * @brief define a class that owns the vertex and index data of a single level of detail
         * 
         * This owns the data, but not the layout
         */
        class LOD : public BaseClass {
        public:

            /**
             * @brief a class responsible for storing all the vertices
             */
            class Vertices {
            public:

                /**
                 * @brief Construct a new vertex storage
                 * 
                 * @param vertexData a pointer to the vertex data
                 * @param vertexCount the amount of vertices to store
                 * @param layout a constant reference to the layout of the vertices
                 */
                Vertices(void* vertexData, u64 vertexCount, const VertexLayout& layout) 
                 : m_layout(&layout)
                {
                    //validate the layout
                    if (m_layout->getAttributeCount() == 0) {
                        //clean up
                        m_layout = nullptr;
                        //throw
                        throw GLGE::Exception("Failed to create a LOD: invalid layout", "GLGE::Mesh::LOD::LOD");
                    }
                    //if the count is 0, stop
                    if (vertexCount == 0) {return;}
                    //allocate the vertex data, make sure the data is alignable
                    //the computed size must be aligned (aligned size * count always returns aligned-up size)
                    void* dat = GLGE_ALIGNED_ALLOC(VertexAttribute::getVertexAlignment(), vertexCount*layout.getSize());
                    //sanity check
                    if (!dat) {
                        //clean up
                        m_layout = nullptr;
                        //throw
                        throw GLGE::Exception("Memory allocation failed", "GLGE::Mesh::LOD::LOD");
                    }
                    //copy the data over
                    if (vertexData)
                    {memcpy(dat, vertexData, vertexCount*layout.getSize());}
                    //store the data
                    m_vertexData = dat;
                    m_vertexCount = vertexCount;
                }

                /**
                 * @brief Construct a new vertex storage
                 * 
                 * @param other the vertex storage to move from
                 */
                Vertices(Vertices&& other)
                 : m_layout(other.m_layout), m_vertexCount(other.m_vertexCount), m_vertexData(other.m_vertexData)
                {
                    //invalidate other
                    other.m_vertexData = nullptr;
                    other.m_layout = nullptr;
                    other.m_vertexCount = 0;
                }

                //vertices cannot be copied
                Vertices(const Vertices&) = delete;
                Vertices& operator=(const Vertices&) = delete;

                /**
                 * @brief Destroy the Vertices
                 */
                ~Vertices() 
                //free on nullptr is defined
                {GLGE_ALIGNED_FREE(m_vertexData);}

                /**
                 * @brief move operator
                 * 
                 * @param other the vertex storage to move from
                 * @return `Vertices&` a reference to this
                 */
                Vertices& operator=(Vertices&& other) {
                    //prevent move to self
                    if (this == &other) {return *this;}

                    //clean up self
                    GLGE_ALIGNED_FREE(m_vertexData);
                    //no need to write nulls into everything, it will now be overriden

                    //move the data over
                    m_layout = other.m_layout;
                    m_vertexCount = other.m_vertexCount;
                    m_vertexData = other.m_vertexData;

                    //invalidate other
                    other.m_vertexData = nullptr;
                    other.m_layout = nullptr;
                    other.m_vertexCount = 0;
                    
                    //return a reference to this
                    return *this;
                }

                /**
                 * @brief Get the Vertex Count
                 * 
                 * @return `u64` the amount of vertices
                 */
                inline u64 getCount() const noexcept
                {return m_vertexCount;}

                /**
                 * @brief Get a specific vertex
                 * 
                 * @warning no bounds checking is performed. Accessing `idx` with a value higher than `m_vertexCount-1` results in undefined behaviour. 
                 * 
                 * @param idx the index of the vertex to query
                 * @return `Vertex` the vertex at that position
                 */
                inline Mesh::Vertex get(u64 idx) const noexcept
                {return Mesh::Vertex(
                    reinterpret_cast<void*>(reinterpret_cast<u8*>(m_vertexData) + idx*m_layout->getSize()), 
                    *m_layout, 
                    reinterpret_cast<void*>(reinterpret_cast<u8*>(m_vertexData) + m_vertexCount*m_layout->getSize())
                );}

                /**
                 * @brief Get a specific vertex
                 * 
                 * @warning no bounds checking is performed. Accessing `idx` with a value higher than `m_vertexCount-1` results in undefined behaviour. 
                 * 
                 * @param idx the index of the vertex to query
                 * @return `Vertex` the vertex at that position
                 */
                inline Mesh::Vertex operator[](u64 idx) const noexcept
                {return get(idx);}

                /**
                 * @brief get a vertex pointing to the beginning of the vertices
                 * 
                 * This may also be the end
                 * 
                 * @return `Vertex` a vertex pointing to the beginning
                 */
                inline Vertex begin() const noexcept
                {return get(0);}

                /**
                 * @brief get a vertex pointing to the end
                 * 
                 * @return `Vertex` a vertex pointing to the end
                 */
                inline Vertex end() const noexcept
                //this is formally UB, but because this is end, it should NOT be dereferenced, so it is safe. 
                {return get(m_vertexCount);}

                /**
                 * @brief get the first valid vertex
                 * 
                 * @return `Vertex` the first valid vertex
                 */
                inline Vertex front() const noexcept
                {return get(0);}

                /**
                 * @brief get the last valid vertex
                 * 
                 * @return `Vertex` the last valid vertex
                 */
                inline Vertex back() const noexcept
                {return get(m_vertexCount-1);}

                /**
                 * @brief get the raw data
                 * 
                 * @return `const void*` a pointer to the constant raw data
                 */
                inline const void* data() const noexcept
                {return m_vertexData;}

                /**
                 * @brief get the raw data
                 * 
                 * @return `void*` a pointer to the raw data
                 */
                inline void* data() noexcept
                {return m_vertexData;}

                /**
                 * @brief Set the Layout
                 * 
                 * @warning layout compatibility is NOT checked. Using incompatible layouts may result in data corruption. 
                 * 
                 * @param layout a pointer to the new layout
                 */
                inline void setLayout(const VertexLayout* layout)
                {m_layout = layout;}

                /**
                 * @brief Get the Layout
                 * 
                 * @return `const VertexLayout&` a constant reference to the layout
                 */
                inline const VertexLayout& getLayout() const noexcept
                {return *m_layout;}

            protected:

                /**
                 * @brief store the layout of the vertices
                 * 
                 * This references the layout of the mesh the LOD belongs to
                 */
                const VertexLayout* m_layout = nullptr;
                /**
                 * @brief store the raw vertex data
                 */
                void* m_vertexData = nullptr;
                /**
                 * @brief store the amount of vertices
                 */
                u64 m_vertexCount = 0;

            };

            /**
             * @brief a class responsible for storing all the indices
             */
            class Indices {
            public:

                /**
                 * @brief Construct a new Indices
                 * 
                 * @param indices the indices to wrap
                 */
                Indices(const std::vector<Triangle>& indices)
                 : m_indices(indices)
                {}

                //indices cannot be copied
                Indices(const Indices&) = delete;
                Indices& operator=(const Indices&) = delete;

                /**
                 * @brief Construct a new index storage
                 * 
                 * Move constructor
                 */
                Indices(Indices&&) = default;
                /**
                 * @brief move an index storage
                 * 
                 * @return `Indices&` a reference to this after moving
                 */
                Indices& operator=(Indices&&) = default;

                /**
                 * @brief Get the index Count
                 * 
                 * @return `u64` the amount of indices
                 */
                inline u64 getCount() const noexcept
                {return m_indices.size();}

                /**
                 * @brief Get a specific index
                 * 
                 * @param idx the index of the triangle to query
                 * @return `Triangle&` the triangle
                 */
                inline Triangle& get(u64 idx) noexcept
                {return m_indices[idx];}

                /**
                 * @brief Get a specific index
                 * 
                 * @param idx the index of the triangle to query
                 * @return `const Triangle&` the triangle
                 */
                inline const Triangle& get(u64 idx) const noexcept
                {return m_indices[idx];}

                /**
                 * @brief Get a specific index
                 * 
                 * @param idx the index of the triangle to query
                 * @return `const Triangle&` the triangle at that position
                 */
                inline const Triangle& operator[](u64 idx) const noexcept
                {return get(idx);}

                /**
                 * @brief Get a specific index
                 * 
                 * @param idx the index of the triangle to query
                 * @return `Triangle&` the triangle at that position
                 */
                inline Triangle& operator[](u64 idx) noexcept
                {return get(idx);}

                /**
                 * @brief get an iterator pointing to the beginning
                 * 
                 * This may also be the end
                 * 
                 * @return `std::vector<u32>::const_iterator` a iterator pointing to the beginning
                 */
                inline std::vector<Triangle>::const_iterator begin() const noexcept
                {return m_indices.begin();}

                /**
                 * @brief get an iterator pointing to the end
                 * 
                 * @return `std::vector<u32>::const_iterator` an iterator pointing to the end
                 */
                inline std::vector<Triangle>::const_iterator end() const noexcept
                {return m_indices.end();}

                /**
                 * @brief get the first valid triangle
                 * 
                 * @return `Triangle` the first valid triangle
                 */
                inline Triangle front() const noexcept
                {return m_indices.front();}

                /**
                 * @brief get the last valid triangle
                 * 
                 * @return `u32` the last valid triangle
                 */
                inline Triangle back() const noexcept
                {return m_indices.back();}

                /**
                 * @brief get the raw data
                 * 
                 * @return `Triangle*` a pointer to the raw data
                 */
                const Triangle* data() const noexcept
                {return m_indices.data();}

            protected:

                /**
                 * @brief store all the indices
                 */
                std::vector<Triangle> m_indices;

            };

            /**
             * @brief Construct a new level of detail
             * 
             * @warning the LOD does NOT take ownership of the inputted buffer. 
             * 
             * @param vertexData the vertex data for the level of detail
             * @param vertexCount the amount of vertices in the data
             * @param indices a constant reference to the index list
             * @param layout a constant reference to the layout
             * @param error the error the LOD has against the original mesh
             * @param createBVH `true` to create the BVH, `false` to not
             */
            LOD(void* vertexData, u64 vertexCount, const std::vector<Triangle>& indices, const VertexLayout& layout, float error, bool createBVH)
             : m_vertices(vertexData, vertexCount, layout), m_indices(indices), m_error(error)
            {
                //create the BVH
                if (createBVH)
                {m_bvh = BVH(this, true, 4, 16);}
            }

            /**
             * @brief Construct a new LOD
             * 
             * This treats the data as root data, meaning that the error is 0. 
             * 
             * @param verts the vertices to create from
             * @param ind the indices to create from
             * @param createBVH `true` to create the BVH, `false` to not
             */
            LOD(Vertices&& verts, Indices&& ind, bool createBVH)
             : m_vertices(std::forward<Vertices>(verts)), m_indices(std::forward<Indices>(ind)), m_error(0.f)
            {
                //create the BVH
                if (createBVH)
                {m_bvh = BVH(this, true, 4, 16);}
            }

            /**
             * @brief Construct a new LOD
             * 
             * This creates a LOD by simplifying another LOD. This also creates a high quality BVH. 
             * 
             * @param from a pointer to the LOD to create from
             * @param targetError the maximum target error
             */
            LOD(LOD* from, float targetError);

            //LODs cannot be copied
            LOD(const LOD&) = delete;
            LOD& operator=(const LOD&) = delete;

            /**
             * @brief Construct a new LOD
             * 
             * @param other the LOD to move from
             */
            LOD(LOD&& other) 
             : m_vertices(std::move(other.m_vertices)), m_indices(std::move(other.m_indices)), m_bvh(std::move(other.m_bvh))
            {
                //move the rest of the data from other
                m_error = other.m_error;
                //invalidate other (only need to invalidate error, rest invalidated itself, default error is 0.f)
                other.m_error = 0.f;

                //update the BVH
                m_bvh.setReferenceLOD(this);
            }

            /**
             * @brief move operator
             * 
             * @param other the LOD to move from
             * @return `LOD&` a reference to this
             */
            LOD& operator=(LOD&& other) {
                //prevent move to self
                if (this == &other) {return *this;}

                //move the data over
                m_vertices = std::move(other.m_vertices);
                m_indices  = std::move(other.m_indices);
                m_bvh      = std::move(m_bvh);
                m_error = other.m_error;

                //invalidate other
                other.m_error = 0.f;

                //update the BVH
                m_bvh.setReferenceLOD(this);

                //return a reference to self
                return *this;
            }

            /**
             * @brief Destroy the LOD
             */
            ~LOD() = default;

            /**
             * @brief Get the Vertex Count
             * 
             * @return `u64` the amount of vertices
             */
            inline u64 getVertexCount() const noexcept
            {return m_vertices.getCount();}

            /**
             * @brief Get a specific vertex
             * 
             * @param idx the index of the vertex to query
             * @return `Vertex` the vertex at that position
             */
            inline Mesh::Vertex getVertex(u64 idx) const noexcept
            {return m_vertices.get(idx);}

            /**
             * @brief Get the Index Count
             * 
             * @return `u64` the amount of stored triangles
             */
            inline u64 getIndexCount() const noexcept
            {return static_cast<u64>(m_indices.getCount());}

            /**
             * @brief Get a specific triangle
             * 
             * @param idx the index of the triangle to get
             * @return `Triangle` the triangle
             */
            inline Triangle getIndex(u64 idx) const noexcept
            {return m_indices.get(idx);}

            /**
             * @brief access the vertices
             * 
             * @return `Vertices&` a reference to the vertices
             */
            inline Vertices& vertices() noexcept
            {return m_vertices;}

            /**
             * @brief access the vertices
             * 
             * @return `const Vertices&` a constant reference to the vertices
             */
            inline const Vertices& vertices() const noexcept
            {return m_vertices;}

            /**
             * @brief access the indices
             * 
             * @return `Indices&` a reference to the indices
             */
            inline Indices& indices() noexcept
            {return m_indices;}

            /**
             * @brief access the indices
             * 
             * @return `const Indices&` a constant reference to the indices
             */
            inline const Indices& indices() const noexcept
            {return m_indices;}

            /**
             * @brief Get the error of the lod compared to the original mesh
             * 
             * @return `float` the error against the original mesh
             */
            inline float getError() const noexcept
            {return m_error;}

            /**
             * @brief set the bounding volume hierarchy for the lod
             * 
             * @param bvh the BVH to move from
             */
            inline void setBVH(BVH&& bvh)
            {m_bvh = bvh;}

            /**
             * @brief get the bounding volume hierarchy
             * 
             * @return `const BVH&` a constant reference to the bounding volume hierarchy of the level of detail
             */
            inline const BVH& getBVH() const noexcept
            {return m_bvh;}

        protected:

            /**
             * @brief store all the vertices
             */
            Vertices m_vertices;
            /**
             * @brief store all the indices
             */
            Indices m_indices;

            /**
             * @brief store the bounding volume hierarchy of this level of detail
             * 
             * A bvh describes spacial location data, so every LOD has a different BVH
             */
            BVH m_bvh;

            /**
             * @brief store the error of the LOD relative to the original mesh
             */
            float m_error = 0.f;

        };

        /**
         * @brief Construct a new Mesh
         * 
         * @param verts the vertices for the original mesh
         * @param ind the indices for the original mesh
         * @param errors the target errors for all LODs. The LOD count will be equal to `errors.size() + 1`. `errors.size() == 0` is valid. 
         * 
         * Requirements:
         * - `errors` must be strictly increasing. Meaning: `errors[n] > errors[n - 1]` must always evaluate to `true`.
         * - `errors` must only contain values in the non-inclusive range 0 to 1. 
         * - `errors` may not contain non-finite values. 
         * - A mesh with zero vertices must also have zero indices.
         * 
         * Info:
         * - A mesh with zero vertices and zero indices is treated as an empty mesh.
         * - This function automatically creates the BVHs for all LODs. 
         */
        Mesh(LOD::Vertices&& verts, LOD::Indices&& ind, const std::vector<float>& errors = {}) 
         : Mesh(verts.getLayout())
        {
            //make sure that the errors are strictly incrementing
            float lastErr = 0.f;
            for (float err : errors) {
                //check if this is a sane number
                if (!std::isfinite(err))
                {throw GLGE::Exception("Failed to create a mesh: found a non-finite number in the LOD target error list", "GLGE::Mesh::Mesh");}
                //sanity check the range (range is 0[, ]1)
                if (err <= 0.f || err >= 1.f)
                {throw GLGE::Exception("Failed to create a mesh: found an out-of-range value in the LOD target error list", "GLGE::Mesh::Mesh");}
                //validate increasing
                if (err <= lastErr)
                {throw GLGE::Exception("Failed to create a mesh: the requested errors were not strictly increasing", "GLGE::Mesh::Mesh");}
                lastErr = err;
            }

            //set the layout pointer
            //the location has changed, but because it was copied from verts, it is guaranteed to be compatible
            verts.setLayout(&m_layout);

            //compute the LOD count (it is 1 more than the input, input are simplified LOD errors)
            size_t lodCount = errors.size() + 1;
            //prepare the LOD storage
            m_lod.reserve(lodCount);

            //store the original LOD
            m_lod.emplace_back(std::move(verts), std::move(ind), true);

            //iterate over all of them and create the LODs
            for (float err : errors) 
            {m_lod.emplace_back(&m_lod.front(), err);}
        }

        /**
         * @brief Construct a new Mesh
         * 
         * @param targetLODCount the target LOD count
         * @param layout the layout for the mesh
         */
        Mesh(size_t targetLODCount, const VertexLayout& layout)
         : Mesh(layout)
        {m_lod.reserve(targetLODCount);}

        /**
         * @brief create a new level of detail
         * 
         * @param vertexData the data for the vertex data
         * @param vertexCount the amount of vertices
         * @param indices a vector containing the indices
         * @param err the error of the LOD compared to the original mesh
         * @param createBVH `true` to create the BVH, `false` to not
         */
        inline void addLOD(void* vertexData, u64 vertexCount, const std::vector<Triangle>& indices, float err, bool createBVH) {
            //create the LOD
            m_lod.emplace_back(vertexData, vertexCount, indices, m_layout, err, createBVH);
        }

        /**
         * @brief add a new LOD
         * 
         * @param lod a rvalue reference to the lod to move into the mesh
         */
        inline void addLOD(LOD&& lod) {
            //set the layout
            lod.vertices().setLayout(&m_layout);
            //store the mesh
            m_lod.emplace_back(std::move(lod));
        }

        /**
         * @brief get the amount of levels of details the mesh has
         * 
         * @return `size_t` the amount of levels of details the mesh has
         */
        inline size_t getLODCount() const noexcept
        {return m_lod.size();}

        /**
         * @brief get a specific level of detail
         * 
         * @param idx the index of the level of detail to get
         * @return `const LOD&` a constant reference to the level of detail
         */
        inline const LOD& getLOD(size_t idx) const noexcept
        {return m_lod[idx];}

        /**
         * @brief get a specific level of detail
         * 
         * @param idx the index of the level of detail to get
         * @return `LOD&` a reference to the level of detail
         */
        inline LOD& getLOD(size_t idx) noexcept
        {return m_lod[idx];}

        /**
         * @brief get a specific level of detail
         * 
         * @param idx the index of the level of detail to get
         * @return `const LOD&` a constant reference to the level of detail
         */
        inline const LOD& operator[](size_t idx) const noexcept
        {return getLOD(idx);}

        /**
         * @brief a function to check if the mesh is empty
         * 
         * @return `true` if the mesh is empty, `false` if not
         */
        inline bool isEmpty() const noexcept 
        {return (m_lod.size() == 0) ? true : (m_lod[0].indices().getCount() == 0);}

        /**
         * @brief Get the Layout
         * 
         * @return `const VertexLayout&` a constant reference to the vertex layout
         */
        inline const VertexLayout& getLayout() const noexcept
        {return m_layout;}

    protected:

        /**
         * @brief Construct a new Mesh
         * 
         * @param layout a constant reference to the vertex layout
         */
        Mesh(const VertexLayout& layout)
         : m_layout(layout)
        {
            //if the layout has no attributes, reject it
            if (m_layout.getAttributeCount() == 0)
            {throw GLGE::Exception("Tried to create a mesh with an invalid vertex layout", "GLGE::Mesh::Mesh");}
        }

        /**
         * @brief store the vertex layout of the mesh
         * 
         * The mesh owns the canonical layout of a vertex. 
         */
        VertexLayout m_layout;

        /**
         * @brief store the levels of details of the mesh
         */
        std::vector<LOD> m_lod;

    };

}

#endif