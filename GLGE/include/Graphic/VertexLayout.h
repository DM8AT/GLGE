/**
 * @file VertexLayout.h
 * @author DM8AT
 * @brief define how vertex layouts are mapped to graphics
 * @version 0.1
 * @date 2026-06-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_VERTEX_LAYOUT_
#define _GLGE_GRAPHIC_VERTEX_LAYOUT_

//add meshes
#include "Core/Mesh.h"
//add the geometry pool system
#include "Backend/Graphics/GeometryPool.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief a class that defines the layout of a vertex
     */
    class VertexLayout {
    public:

        /**
         * @brief define what a type is
         * 
         * A type defines how the data is stored
         */
        using Type = GLGE::Mesh::Type;

        /**
         * @brief store information about a single attribute
         */
        struct Attribute {
            /**
             * @brief define the usage hash
             */
            u64 usage;
            /**
             * @brief define the byte offset into the stream
             */
            u32 streamOffset;
            /**
             * @brief define the offset in the source
             */
            u32 sourceOffset;
            /**
             * @brief define the index in the source
             */
            u32 sourceIdx;
            /**
             * @brief define the type of the attribute in storage
             */
            Type type;
            /**
             * @brief define the ID of the stream the attribute lives in
             */
            u8 streamId;
        };

        /**
         * @brief Construct a new Vertex Layout
         * 
         * @tparam Ts the types of the usages to map
         * @param layout the vertex layout to map from
         * @param mapping the mappings from usages to streams
         */
        template <typename... Ts>
        VertexLayout(const GLGE::Mesh::VertexLayout& layout, std::pair<Ts, u64>... mapping) 
         : VertexLayout(layout, std::vector<std::pair<u64, u64>>{{getTypeHash64<Ts>(), mapping.second}... })
        {}

        /**
         * @brief Construct a new Vertex Layout
         * 
         * @param layout the CPU side vertex layout to create from
         * @param mapping the mappings from attribute IDs to streams
         * 
         * The mapping order is important. The first entry in a stream will have offset 0, the next one will packed tightly behind that, and the next one behind that. 
         * The packing respects attribute padding. 
         */
        VertexLayout(const GLGE::Mesh::VertexLayout& layout, const std::vector<std::pair<u64, u64>>& mapping) 
         : m_layout(layout)
        {
            //early out on empty mapping (CPU only layout)
            if (mapping.empty())
            {return;}

            //store the current offset for each stream
            std::array<u32, Backend::Graphic::GeometryPool::MAX_STREAM_COUNT> offsets {};
            offsets.fill(0);

            //iterate over all mappings
            for (const auto& [usage, streamId64] : mapping) {
                //check if the stream ID is valid
                if (streamId64 >= Backend::Graphic::GeometryPool::MAX_STREAM_COUNT) 
                {throw GLGE::Exception("Invalid stream ID", "GLGE::Graphic::VertexLayout::VertexLayout");}

                //compress the ID down (safety check done, so safe)
                const u8 streamId = static_cast<u8>(streamId64);

                //store the source index
                const u64 sourceIdx = layout.getIdxOfUsage(usage);
                if (sourceIdx == UINT64_MAX)
                {throw GLGE::Exception("Mapping reference to an attribute of usage that is not found", "GLGE::Graphic::VertexLayout::VertexLayout");}

                //sanity check the source index
                if (sourceIdx > layout.getAttributeCount()) 
                {throw GLGE::Exception("Mapping references an out of range attribute", "GLGE::Graphic::VertexLayout::VertexLayout");}
                // if (layout[sourceIdx].usage != getTypeHash64<GLGE::VertexAttribute::Undefined>())
                // {throw GLGE::Exception("Mapping references an undefined attribute", "GLGE::Graphic::VertexLayout::VertexLayout");}

                //get the source attachment
                const auto& srcAttr = layout[sourceIdx];

                //get information about the type
                auto typeInfo = GLGE::Mesh::VertexLayout::VertexAttribute::getTypeInfo(srcAttr.type);

                //align up the offset and store it
                offsets[streamId] = alignUp(offsets[streamId], static_cast<u32>(typeInfo.alignment));

                //construct the attribute
                Attribute attr;
                attr.type         = srcAttr.type;
                attr.streamId     = streamId;
                attr.streamOffset = static_cast<u8>(offsets[streamId]);
                attr.sourceOffset = static_cast<u32>(srcAttr.offset);
                attr.sourceIdx    = static_cast<u16>(sourceIdx);
                attr.usage        = usage;

                //store the attribute
                m_attributes.push_back(attr);

                //add the size to the stream offset (to push format)
                offsets[streamId] += static_cast<u32>(typeInfo.size);
                //update the stream count
                m_streamCount = std::max<u8>(m_streamCount, static_cast<u8>(streamId + 1));
            }

            //store the strides for all the streams
            for (u8 i = 0; i < m_streamCount; ++i) 
            {m_streamStrides[i] = offsets[i];}
        }

        /**
         * @brief Construct a new Vertex Layout
         * 
         * Copy constructor
         */
        VertexLayout(const VertexLayout&) = default;
        /**
         * @brief Construct a new Vertex Layout
         * 
         * Move constructor
         */
        VertexLayout(VertexLayout&&) = default;

        /**
         * @brief Copy assignment operator
         * 
         * @return `VertexLayout&` a reference to the layout after copying
         */
        VertexLayout& operator=(const VertexLayout&) = default;
        /**
         * @brief Move assignment operator
         * 
         * @return `VertexLayout&` a reference the layout after moving
         */
        VertexLayout& operator=(VertexLayout&&) = default;

        /**
         * @brief Get the Attribute Count
         * 
         * @return `size_t` the amount of attributes in the layout
         */
        inline size_t getAttributeCount() const noexcept
        {return m_attributes.size();}

        /**
         * @brief Get the Attribute
         * 
         * @param idx the index of the attribute
         * @return `const Attribute&` a constant reference to the attribute
         */
        inline const Attribute& getAttribute(size_t idx) const noexcept
        {return m_attributes[idx];}

        /**
         * @brief Get the Attribute
         * 
         * @param idx the index of the attribute
         * @return `const Attribute&` a constant reference to the attribute
         */
        inline const Attribute& operator[](size_t idx) const noexcept
        {return m_attributes[idx];}

        /**
         * @brief check if the layout has an attribute of a specific usage
         * 
         * @param usage the usage to check for
         * @return `true` if an attribute of the usage exists, `false` if not
         */
        inline bool hasUsage(u64 usage) const noexcept
        {return getIdxOfUsage(usage) != UINT64_MAX;}

        /**
         * @brief Get the index of an attribute with a specific usage
         * 
         * @param usage the hash of the usage to check for
         * @return `u64` the index of the attribute or `UINT64_MAX` the the usage does not exist
         */
        inline u64 getIdxOfUsage(u64 usage) const noexcept {
            for (size_t i = 0; i < m_attributes.size(); ++i) {
                if (m_attributes[i].usage == usage)
                {return i;}
            }
            return UINT64_MAX;
        }

        /**
         * @brief check if the layout has an attribute with a specific usage
         * 
         * @tparam T the type identifier of the usage
         * @return `true` if the usage was found, `false` if not
         */
        template<typename T>
        inline bool hasUsage() const noexcept
        {return hasUsage(getTypeHash64<T>());}

        /**
         * @brief Get the index of a specific usage
         * 
         * @tparam T the type identifier of the usage
         * @return `u64` the index of the attribute with that usage or `UINT64_MAX` if the usage was not found
         */
        template<typename T>
        inline u64 getIdxOfUsage() const noexcept
        {return getIdxOfUsage(getTypeHash64<T>());}

        /**
         * @brief Get the Attribute with a specific usage
         * 
         * @tparam T the type identifier of the usage
         * @return `const Attribute&` a constant reference to the attribute with the usage
         */
        template<typename T>
        inline const Attribute& getAttribute() const
        {return m_attributes[getIdxOfUsage<T>()];}

        /**
         * @brief Get the Stream Count
         * 
         * @return `u8` the amount of used streams
         */
        inline u8 getStreamCount() const noexcept
        {return m_streamCount;}

        /**
         * @brief Get the stream stride
         * 
         * @param stream the stream ID to query the stride for
         * @return `u32` the stride of the specific stream
         */
        inline u32 getStreamStride(u8 stream) const noexcept
        {return m_streamStrides[stream];}

        /**
         * @brief Get the Layout
         * 
         * @return `const GLGE::Mesh::VertexLayout&` a constant reference to the layout this layout derives from
         */
        inline const GLGE::Mesh::VertexLayout& getLayout() const noexcept
        {return m_layout;}

    protected:

        /**
         * @brief a helper to align a value to a specific alignment
         * 
         * @param value the value to align
         * @param alignment the alignment value to align to (must be a power of 2)
         * @return `constexpr u32` the aligned value
         */
        static constexpr u32 alignUp(u32 value, u32 alignment) noexcept 
        {return (value + alignment - 1) & ~(alignment - 1);}

        /**
         * @brief store the attributes
         */
        std::vector<Attribute> m_attributes;
        /**
         * @brief store the strides of all the streams
         */
        std::array<u32, Backend::Graphic::GeometryPool::MAX_STREAM_COUNT> m_streamStrides{};
        /**
         * @brief store the amount of streams
         */
        u8 m_streamCount = 0;

        /**
         * @brief store the layout this layout derives from
         */
        GLGE::Mesh::VertexLayout m_layout;

    };

}

#endif