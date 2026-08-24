/**
 * @file GeometryPool.cpp
 * @author DM8AT
 * @brief implement the geometry pool
 * @version 0.1
 * @date 2026-06-08
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add geometry pools
#include "Graphic/Backend/Graphics/GeometryPool.h"
//add exceptions
#include "Core/Exception.h"
//add graphic instances
#include "Graphic/Instance.h"

static constexpr GLGE::u64 alignUp(GLGE::u64 value, GLGE::u64 alignment) noexcept 
{return (value + alignment - 1) & ~(alignment - 1);}

GLGE::Graphic::Backend::Graphic::GeometryPool::Archetype& GLGE::Graphic::Backend::Graphic::GeometryPool::acquireArchetype(const std::vector<AttributeIdentifier>& usages) {
    //create the identifier
    ArchetypeIdentifier identifier{usages};
    //potential insert
    auto it = m_archetypeIdMap.find(identifier);
    if (it == m_archetypeIdMap.end()) {
        //insert into linear map
        size_t idx = m_archetypes.size();
        m_archetypes.emplace_back(std::make_unique<Archetype>(m_inst, usages, static_cast<u32>(m_archetypes.size())));
        //add the ID mapping
        it = m_archetypeIdMap.emplace(identifier, idx).first;
    }
    //return the value
    return *m_archetypes[it->second];
}




GLGE::Graphic::Backend::Graphic::GeometryPool::Region GLGE::Graphic::Backend::Graphic::GeometryPool::Stream::allocate(u64 size, u64 alignment) {
    //sanity check
    if (size == 0)
    {throw GLGE::Exception("Cannot allocate a region with a size of 0", "GLGE::Graphic::Backend::Graphic::GeometryPool::Pool::allocate");}
    if (alignment == 0)
    {throw GLGE::Exception("Alignment must not be zero", "GLGE::Graphic::Backend::Graphic::GeometryPool::Pool::allocate");}

    //look through all free-list entries
    for (size_t i = 0; i < m_free.size(); ++i) {
        //get the region
        Region free = m_free[i];

        //align and compute padding
        u64 alignedStart = alignUp(free.offset, alignment);
        u64 padding = alignedStart - free.offset;

        //check for a fit
        if (free.size < padding + size) 
        {continue;}

        //fits -> store a resulting region
        Region result;
        result.offset = alignedStart;
        result.size = size;

        //compute the allocation end and free end
        u64 allocEnd = alignedStart + size;
        u64 freeEnd  = free.offset + free.size;

        //compute the tail size
        u64 tailSize = freeEnd - allocEnd;

        //remove the original block
        m_free.erase(m_free.begin() + i);
        //conditional re-insert

        //re-insert padding front
        if (padding > 0) {m_free.emplace_back(free.offset, padding);}
        //re-insert tail
        if (tailSize > 0) {m_free.emplace_back(allocEnd,tailSize);}

        //return the result
        return result;
    }

    //no element found -> does not fit -> grow
    u64 oldSize = m_size;

    //do double-growth until allocation size is satisfied
    do {m_size = std::max<u64>(m_size * 2, size);}
    while (alignUp(oldSize, alignment) + size > m_size);

    //resize
    onResize(m_size);

    //add a free range
    m_free.push_back({
        oldSize,
        m_size - oldSize
    });

    //just recurse (this call should be successful)
    return allocate(size, alignment);
}

void GLGE::Graphic::Backend::Graphic::GeometryPool::Stream::free(Region& region) {
    //stop on null region
    if (region.isNull()) {return;}

    //add the free region
    m_free.push_back(region);

    //make sure to clear the region
    region.offset = 0;
    region.size = 0;

    //sort the free list
    std::sort(m_free.begin(), m_free.end(), [](auto& a, auto& b) {return a.offset < b.offset;});

    //iterate over all free elements and check for insertion or merge
    for (size_t i = 0; i + 1 < m_free.size();) {
        //get this and the next element
        Region& a = m_free[i];
        Region& b = m_free[i + 1];

        //check for merging
        if (a.offset + a.size == b.offset) {
            a.size += b.size;
            m_free.erase(m_free.begin() + i + 1);
        }
        else {
            //if not merging, step forward
            ++i;
        }
    }
}

void GLGE::Graphic::Backend::Graphic::GeometryPool::Stream::write(Region region, const void* data, u64 offset, u64 size) {
    //check for null region
    if (region.isNull()) 
    {throw Exception("attempt to write to null region", "GLGE::Graphic::Backend::Graphic::GeometryPool::Pool::write");}

    //sanity check bounds
    if (size > region.size) 
    {throw std::out_of_range("region write overflow");}

    //write
    onWrite(region, data, offset, size);
}



/**
 * @brief store the initial size for pools
 */
inline static constexpr GLGE::u64 INITIAL_SIZE = 1024;

GLGE::Graphic::Backend::Graphic::GeometryPool::Archetype::Archetype(GLGE::Graphic::Instance* instance, const std::vector<AttributeIdentifier>& identifiers, u32 id) 
 : m_inst(instance), m_id(id)
{
    //copy identifiers
    std::vector<AttributeIdentifier> ident = identifiers;
    //sort the identifiers
    std::sort(ident.begin(), ident.end());
    //the order of usages is fully irrelevant, no need to sort them
    for (size_t i = 0; i < ident.size();) {
        //store the current usage
        size_t start = i;
        u8 stream = ident[start].stream;
        //sanity check stream
        if (stream >= MAX_STREAM_COUNT) {throw GLGE::Exception("Found an out of bounds stream ID", "GLGE::Graphic::Backend::Graphic::GeometryPool::Archetype::Archetype");}
        //sum up the strides (stride is always the sum of element sizes)
        size_t sum = 0;
        for (; (i < ident.size()) && (ident[i].stream == stream); ++i) 
        {sum += GLGE::Mesh::VertexAttribute::getTypeInfo(static_cast<GLGE::Mesh::Type>(ident[i].type)).size;}
        std::vector<AttributeIdentifier> attrs(ident.begin() + start, ident.begin() + i);
        //store the vertex data tag
        m_vertexPools[stream].tag.attributes = attrs;
        m_vertexPools[stream].tag.stride = sum;
        m_vertexPools[stream].stream = std::unique_ptr<Stream>(m_inst->getGraphicDescription()->createGeometryPoolStream(INITIAL_SIZE, false, m_inst->getGraphicBackendInstance().get()));
        m_vertexPools[stream].active = true;
    }

    //create the index pool
    m_indexPool = std::unique_ptr<Stream>(m_inst->getGraphicDescription()->createGeometryPoolStream(INITIAL_SIZE, true, m_inst->getGraphicBackendInstance().get()));
}

GLGE::Graphic::Backend::Graphic::GeometryPool::Allocation GLGE::Graphic::Backend::Graphic::GeometryPool::Archetype::allocate(u32 vertexCount, u32 indexCount) {
    //store the allocation
    Allocation alloc {};

    //allocate the index stuff (indices are required to be u32 engine wide)
    Region idxRegion = m_indexPool->allocate(indexCount * sizeof(Triangle), 1);
    //store the allocation
    alloc.indexCount = idxRegion.size/sizeof(Triangle);
    alloc.indexOffset = idxRegion.offset/sizeof(Triangle);

    //in debug record the regions
    #if GLGE_DEBUG
    std::vector<Region> regions;
    regions.reserve(m_vertexPools.size());
    #endif

    //vertex allocation
    for (u8 i = 0; i < MAX_STREAM_COUNT; ++i) {
        //get the pool
        auto& [info, pool, active] = m_vertexPools[i];
        if (!active) {continue;}
        //allocate the region
        Region r = pool->allocate(vertexCount*info.stride, 1);
        #if GLGE_DEBUG
        regions.push_back(r);
        #endif
        //store the allocation metadata
        alloc.vertexCount  = r.size / info.stride;
        alloc.vertexOffset = r.offset / info.stride;
    }

    //return the allocation
    return alloc;
}

void GLGE::Graphic::Backend::Graphic::GeometryPool::Archetype::free(Allocation& allocation) {
    //free the vertex allocation
    Region r(allocation.indexOffset * sizeof(Triangle), allocation.indexCount * sizeof(Triangle));
    m_indexPool->free(r);

    //iterate over all pools and free the allocation
    for (u8 i = 0; i < MAX_STREAM_COUNT; ++i) {
        //get the pool
        auto& [info, pool, active] = m_vertexPools[i];
        if (!active) {continue;}
        //construct the region
        Region r(allocation.vertexOffset * info.stride, allocation.vertexCount * info.stride);
        //free the region
        pool->free(r);
    }

    //clean the allocation
    allocation.indexCount   = 0;
    allocation.indexOffset  = 0;
    allocation.vertexCount  = 0;
    allocation.vertexOffset = 0;
    allocation.archetypeId = UINT32_MAX;
}

void GLGE::Graphic::Backend::Graphic::GeometryPool::Archetype::write(const Allocation& allocation, AttributeIdentifier identifier, const void* data, u32 offset, u32 count, u32 localOffset) {
    //specific check for index usage
    if (identifier == AttributeIdentifier(getTypeHash64<Index>(), static_cast<u8>(GLGE::Mesh::Type::UInt), 0)) {
        //use the special-cased index pool
        m_indexPool->write(Region(allocation.indexOffset*sizeof(Triangle), allocation.indexCount*sizeof(Triangle)), data, offset*sizeof(Triangle) + localOffset, count*sizeof(Triangle));
        m_indexPool->onFlush();
        return;
    }

    //iterate over all pools to look for the usage
    for (u8 i = 0; i < MAX_STREAM_COUNT; ++i) {
        //get the pool
        auto& [info, pool, active] = m_vertexPools[i];
        if (!active) {continue;}
        //skip the attribute list does not contain the identifier
        size_t idx = SIZE_MAX;
        for (size_t j = 0; j < info.attributes.size(); ++j) 
        {if (info.attributes[j] == identifier) {idx = j; break;}}
        if (idx == SIZE_MAX) {continue;}
        //get the type information
        auto typeInfo = GLGE::Mesh::VertexLayout::VertexAttribute::getTypeInfo(static_cast<GLGE::Mesh::Type>(info.attributes[idx].type));
        //write
        for (size_t j = 0; j < count; ++j) {
            pool->write(Region((allocation.vertexOffset + offset + j)*info.stride, info.stride), reinterpret_cast<const u8*>(data) + j*info.stride, localOffset, info.stride);
        }
        pool->onFlush();
        //stop
        return;
    }

    //at this point the identifier is unknown
    throw GLGE::Exception("Failed to write: Unknown identifier", "GLGE::Graphic::Backend::Graphic::GeometryPool::Archetype::write");
}