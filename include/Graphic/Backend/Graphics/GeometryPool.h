/**
 * @file GeometryPool.h
 * @author DM8AT
 * @brief define the geometry pool glue class
 * @version 0.1
 * @date 2026-06-08
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHICS_GEOMETRY_POOL_
#define _GLGE_GRAPHIC_BACKEND_GRAPHICS_GEOMETRY_POOL_

//add common stuff
#include "Core/Common.h"
//add meshes
#include "Core/Mesh.h"
//for smart pointers (actually used here)
#include <memory>

//instances are defined later
namespace GLGE::Graphic
{class Instance;}

//use the backend namespace
namespace GLGE::Graphic::Backend::Graphic {

    //forward declaration
    class Instance;

    /**
     * @brief store an abstract geometry pool
     */
    class GeometryPool {
    public:

        /**
         * @brief a structure that stores a single archetype allocation
         */
        struct Allocation {
            /**
             * @brief store the amount of vertices the data is offset
             */
            u32 vertexOffset;
            /**
             * @brief store the amount of vertices the data is owning
             */
            u32 vertexCount;
            /**
             * @brief store the amount of indices the data is offset
             */
            u32 indexOffset;
            /**
             * @brief store the amount of indices the data is owning
             */
            u32 indexCount;
            /**
             * @brief store the archetype ID of the allocation
             */
            u32 archetypeId;
        };

        /**
         * @brief a structure used to identify an attribute
         */
        struct AttributeIdentifier {
            /**
             * @brief store the usage hash
             */
            u64 usage = 0;
            /**
             * @brief store the actual storage type
             */
            u8 type = 0;
            /**
             * @brief store the stream ID of the attribute
             */
            u8 stream = 0;

            /**
             * @brief Construct a new Attribute Identifier
             * 
             * @param _usage the usageof the attribute
             * @param _type the type of the attribute
             * @param _stream the stream of the attribute
             */
            AttributeIdentifier(u64 _usage, u8 _type, u8 _stream)
             : usage(_usage), type(_type), stream(_stream)
            {}

            /**
             * @brief Construct a new Pool Identifier
             */
            AttributeIdentifier() = default;

            /**
             * @brief default comparison operator
             * 
             * @return `true` if the pool identifiers are the same, `false` if not
             */
            constexpr bool operator==(const AttributeIdentifier&) const noexcept = default;

            /**
             * @brief a weak-order comparison operator
             * 
             * @param other a constant reference to the pool identifier to compare against
             * @return `true` if this element is less than other, otherwise `false` 
             */
            bool operator<(const AttributeIdentifier& other) const noexcept {
                //stream ID must always be the TOP sorting element. All stream IDs must be continues after sort
                //do NOT break!!!!!
                if (stream != other.stream) {return stream < other.stream;}
                //sort on rest
                if (usage != other.usage) {return usage < other.usage;}
                return type < other.type;
            }
        };

        /**
         * @brief define the maximum amount of attributes a single archetype may have
         */
        inline static constexpr size_t MAX_STREAM_COUNT = 16;

        /**
         * @brief Construct a new Geometry Pool
         */
        GeometryPool(GLGE::Graphic::Instance* instance) 
         : m_inst(instance)
        {}

        /**
         * @brief allocate a new data set
         * 
         * @warning the `usages` list must be duplicate free. 
         * 
         * @param vertexCount the amount of vertices to allocate
         * @param indexCount the amount of indices to allocate
         * @param attributeIdentifiers the identifiers for all pools of the archetypes
         * @return `Allocation` the final allocation
         */
        inline Allocation allocate(u32 vertexCount, u32 indexCount, const std::vector<AttributeIdentifier>& attributeIdentifiers) {
            //get the correct archetype (create if not existent)
            auto& arch = acquireArchetype(attributeIdentifiers);
            //allocate the region
            auto alloc = arch.allocate(vertexCount, indexCount);
            //set the archetype ID
            alloc.archetypeId = m_archetypeIdMap[attributeIdentifiers];
            //return the allocation
            return alloc;
        }

        /**
         * @brief free an existing allocation
         * 
         * @warning the `usages` list must be duplicate free. 
         * 
         * @param allocation a reference to the allocation to free. Will be reset to all zero after free. Parsing elements with a size of 0 is valid and does nothing. 
         */
        inline void free(Allocation& allocation)
        {m_archetypes[allocation.archetypeId]->free(allocation);}

        /**
         * @brief write into a specific usage of an allocation
         * 
         * @warning the `usages` list must be duplicate free. 
         * 
         * @param allocation the allocation to write to
         * @param identifier the identifier of the pool to write to
         * @param data a pointer to the data to write
         * @param offset the offset in elements to write to
         * @param count the amount of elements to write
         * @param localOffset the attribute local offset
         */
        inline void write(const Allocation& allocation, AttributeIdentifier identifier, const void* data, u32 offset, u32 count, u32 localOffset)
        {m_archetypes[allocation.archetypeId]->write(allocation, identifier, data, offset, count, localOffset);}

        /**
         * @brief define an index usage that hooks into the same usage system as vertex attributes
         */
        struct Index {
            /**
             * @brief required for MSVC
             */
            u8 _unused = 0;
        };

        /**
         * @brief store a region reference into a pool
         */
        struct Region {
            /**
             * @brief store the offset in bytes from the region start
             */
            u64 offset = 0;
            /**
             * @brief store the size in bytes of the region
             * 
             * A size of 0 is never returned on an allocation. 
             * 
             * If the size is 0 and the offset is 0, this is interpreted as a null-region. 
             * 
             * Future special cases of size = 0 + offset are possible, e.g. for error return info. 
             */
            u64 size = 0;

            /**
             * @brief create a new region
             * 
             * Initially a region is a null region
             */
            constexpr Region() = default;

            /**
             * @brief create a new region
             * 
             * @param offset the offset from the buffer start in bytes
             * @param size the region size in bytes
             */
            constexpr Region(u64 offset, u64 size)
             : offset(offset), size(size) 
            {}

            //default copy / move

            /**
             * @brief copy a region
             */
            constexpr Region(const Region&) = default;
            /**
             * @brief copy a region
             * 
             * @return `constexpr Region&` a reference to the region after copying
             */
            constexpr Region& operator=(const Region&) = default;
            /**
             * @brief move a region
             */
            constexpr Region(Region&&) = default;
            /**
             * @brief move a region
             * 
             * @return `constexpr Region&` a reference to the region after moving
             */
            constexpr Region& operator=(Region&&) = default;

            /**
             * @brief check if this is a null region
             * 
             * @return `true` if this is a null region, `false` if this is not a null region
             */
            inline constexpr bool isNull() const noexcept
            {return offset == 0 && size == 0;}

            /**
             * @brief check if this is NOT a null region
             * 
             * @return `true` if this is not a null region, `false` if this is a null region
             */
            inline constexpr operator bool() const noexcept
            {return !isNull();}

            /**
             * @brief check if two regions are identical
             * 
             * @return `true` if they are the same, `false` if not
             */
            inline constexpr bool operator==(const Region&) const noexcept = default;
        };

        /**
         * @brief store a single stream
         */
        class Stream {
        public:

            /**
             * @brief Construct a new Stream
             * 
             * @param size the inital size of the Stream
             * @param isIbo `true` if this is an index buffer, `false` for a vertex buffer
             * @param instance a pointer to the instance the stream will belong to
             */
            Stream(u64 size, bool isIbo, GLGE::Graphic::Backend::Graphic::Instance* instance)
             : m_size(size), m_free{Region{0, size}}, m_isIBO(isIbo), m_inst(instance)
            {}

            /**
             * @brief Destroy the Stream
             */
            virtual ~Stream() {}

            /**
             * @brief allocate a specific region
             * 
             * @warning this function may re-allocate the internal data
             * 
             * @warning the alignment may only be powers of 2
             * 
             * @param size the size of the region in bytes
             * @param alignment define the byte alignment for the data
             * @return `Region` a region pointing to the aligned start of the section
             */
            inline Region allocate(u64 size, u64 alignment);

            /**
             * @brief free a specific region
             * 
             * @warning the region must be allocated from this Stream
             * 
             * @param region the region to free. Will be the null-region after free. Parsing the null region is valid and will do nothing. 
             */
            void free(Region& region);

            /**
             * @brief write into a region
             * 
             * @param region the region to write to
             * @param data a pointer to the data to write
             * @param offset the region-internal offset of the data
             * @param size the size of the data
             */
            void write(Region region, const void* data, u64 offset, u64 size);

            /**
             * @brief flush the newly written data to the GPU
             * 
             * Depending on the backend this may be required or not, so make sure to always use it. The backend decides if it is required or not. 
             */
            virtual void onFlush() = 0;

        protected:

            /**
             * @brief a function that is called when the buffer is resized
             * 
             * @param newSize the new size of the Stream
             */
            virtual void onResize(u64 newSize) = 0;

            /**
             * @brief a function that is called on writing
             * 
             * @param region the region to write to
             * @param data a pointer to the data to write
             * @param offset the region-internal offset of the data
             * @param size the size of the data
             */
            virtual void onWrite(Region region, const void* data, u64 offset, u64 size) = 0;

            /**
             * @brief store the free regions
             */
            std::vector<Region> m_free;
            /**
             * @brief store the size of the Stream
             */
            u64 m_size = 0;
            /**
             * @brief store if this is an index buffer
             */
            bool m_isIBO = false;
            
            /**
             * @brief store a pointer to the backend instance the stream belongs to
             */
            GLGE::Graphic::Backend::Graphic::Instance* m_inst;

        };

        /**
         * @brief store information about what is stored in a stream
         */
        struct StreamDataTag {
            /**
             * @brief store the attribute identifiers
             */
            std::vector<AttributeIdentifier> attributes;

            /**
             * @brief store the stride of the stream
             */
            u32 stride = 0;
        };

        /**
         * @brief an archetype is a structure that stores Streams for a single unique set of usages
         * 
         * Archetypes are independent from the order of the elements since they do not matter for alignment. 
         * 
         * This also has the effect of minimizing archetype count, which the backend may use to minimize state changes. 
         */
        class Archetype {
        public:

            /**
             * @brief store an entry in the stream array
             */
            struct StreamEntry {
                /**
                 * @brief store the data tag
                 */
                StreamDataTag tag;
                /**
                 * @brief store the actual stream
                 */
                std::unique_ptr<Stream> stream;
                /**
                 * @brief `true` if in use, `false` if not
                 */
                bool active = false;
            };

            /**
             * @brief Construct a new Archetype
             * 
             * @param instance a pointer to the instance the archetype belongs to
             * @param attributeIdentifiers the identifiers for all Streams of the archetype
             * @param id the index of the archetype
             */
            Archetype(GLGE::Graphic::Instance* instance, const std::vector<AttributeIdentifier>& attributeIdentifiers, u32 id);

            /**
             * @brief allocate a new data set
             * 
             * @param vertexCount the amount of vertices to allocate
             * @param indexCount the amount of indices to allocate
             * @return `Allocation` the final allocation
             */
            Allocation allocate(u32 vertexCount, u32 indexCount);

            /**
             * @brief free an existing allocation
             * 
             * @param allocation a reference to the allocation to free. Will be reset to all zero after free. Parsing elements with a size of 0 is valid and does nothing. 
             */
            void free(Allocation& allocation);

            /**
             * @brief write into a specific usage of an allocation
             * 
             * @param allocation the allocation to write to
             * @param identifier the attribute identifier of the attribute to write to
             * @param data a pointer to the data to write
             * @param offset the offset in elements to write to
             * @param count the amount of elements to write
             * @param localOffset the attribute-local offset
             */
            void write(const Allocation& allocation, AttributeIdentifier identifier, const void* data, u32 offset, u32 count, u32 localOffset);

            /**
             * @brief Get the identifier of the archetype
             * 
             * @return `u32` the identifier for the archetype
             */
            inline u32 getId() const noexcept
            {return m_id;}

            /**
             * @brief access a specific stream
             * 
             * @warning Only modify if you know what you're doing
             * 
             * @param streamId the identifier of the stream to access
             * @return `StreamEntry&` a reference to the requested stream entry
             */
            inline StreamEntry& accessStream(u8 streamId) 
            {return m_vertexPools[streamId];}

            /**
             * @brief access a specific stream
             * 
             * @warning Only modify if you know what you're doing
             * 
             * @param streamId the identifier of the stream to access
             * @return `const StreamEntry&` a constant reference to the requested stream entry
             */
            inline const StreamEntry& accessStream(u8 streamId) const
            {return m_vertexPools[streamId];}

            /**
             * @brief Get the Index Stream
             * 
             * @warning Only modify if you know what you're doing
             * 
             * @return `Stream*` a pointer to the index stream
             */
            inline Stream* getIndexStream() const noexcept
            {return m_indexPool.get();}

        protected:

            /**
             * @brief store all the pools and their usage
             */
            std::array<StreamEntry, MAX_STREAM_COUNT> m_vertexPools;

            /**
             * @brief store the index pool
             */
            std::unique_ptr<Stream> m_indexPool = nullptr;

            /**
             * @brief store a pointer to the graphic instance the geometry pool belongs to
             */
            GLGE::Graphic::Instance* m_inst = nullptr;
            /**
             * @brief store the identifier for the archetype
             */
            u32 m_id;

        };

        /**
         * @brief Get the Archetype
         * 
         * If the archetype is not known, this creates the archetype
         * 
         * @param attributeIdentifiers the identifiers for all pools of the archetypes
         * @return `Archetype&` a reference to the archetype
         */
        Archetype& acquireArchetype(const std::vector<AttributeIdentifier>& attributeIdentifiers);

    protected:

        /**
         * @brief a function to hash a pool identifier
         * 
         * @param identifier the identifier to hash
         * @return `std::size_t` the combined hash
         */
        inline static std::size_t hashAttributeIdentifier(const AttributeIdentifier& identifier) noexcept {
            //a helper to combine a hash
            auto hashCombine = [](std::size_t& seed, std::size_t value) {seed ^= value + 0x9e3779b97f4a7c15ull + (seed << 6) + (seed >> 2);};
            
            //the u64 is allready a hash, no need to hash a hash
            std::size_t hash = static_cast<std::size_t>(identifier.usage);
            //add the other elements
            hashCombine(hash, static_cast<std::size_t>(identifier.type));
            hashCombine(hash, static_cast<std::size_t>(identifier.stream));
            //return the combined hash
            return hash;
        }

        /**
         * @brief identify a specific archetype
         */
        struct ArchetypeIdentifier {
            /**
             * @brief the identifiers of the archetype
             */
            std::vector<AttributeIdentifier> identifiers;

            /**
             * @brief Construct a new Archetype Identifier
             * 
             * @warning `_identifiers` must contain only unique elements
             * 
             * @param _identifiers a list of usages the archetype identifier has
             */
            ArchetypeIdentifier(const std::vector<AttributeIdentifier>& _identifiers)
             : identifiers(_identifiers)
            {std::sort(identifiers.begin(), identifiers.end());}

            /**
             * @brief check if two archetype identifiers are identical
             * 
             * @param other the identifier to compare against
             * @return `true` if they are the same, `false` if not
             */
            inline bool operator==(const ArchetypeIdentifier& other) const noexcept {
                //if the sizes don't match, they cannot match
                if (other.identifiers.size() != identifiers.size()) {return false;}
                //check all pool identifiers to match (should be sorted)
                for (size_t i = 0; i < identifiers.size(); ++i)
                {if (identifiers[i] != other.identifiers[i]) {return false;}}
                //they are identical
                return true;
            }
        };

        /**
         * @brief a struct that acts as a hash functor for archetype identifiers
         */
        struct ArchetypeIdentifierHashFunctor {
            /**
             * @brief a functor to hash an identifier
             * 
             * @param identifier a constant reference to the identifier to hash
             * @return `std::size_t` the hash of the identifier
             */
            inline std::size_t operator()(const ArchetypeIdentifier& identifier) const noexcept {
                //this is here acceptable: All elements are verified to be unique and `hashUsages({0, 1}) == hashUsages({0, 1})` must be satisfied. 
                std::size_t combined = 0;
                for (const auto& el : identifier.identifiers) {
                    std::size_t hash = hashAttributeIdentifier(el);
                    combined ^= std::rotl(hash, (hash % 61));
                }
                return combined;
            }
        };

        /**
         * @brief store the archetypes
         */
        std::vector<std::unique_ptr<Archetype>> m_archetypes;

        /**
         * @brief store a mapping from the archetype identifiers to the archetype IDs
         */
        std::unordered_map<ArchetypeIdentifier, size_t, ArchetypeIdentifierHashFunctor> m_archetypeIdMap;

        /**
         * @brief store a pointer to the graphic instance the geometry pool belongs to
         */
        GLGE::Graphic::Instance* m_inst = nullptr;

    };

}

#endif