/**
 * @file MeshPool.cpp
 * @author DM8AT
 * @brief implement the OpenGL 
 * @version 0.1
 * @date 2026-03-25
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the OpenGL mesh pool implementation
#include "Graphic/Backend/Builtin/Graphics/OpenGL/MeshPool.h"

//the mapper
#include "__Mapper.h"

/**
 * @brief a helper function to align to a specific byte size
 * 
 * @param v the value to align
 * @param a the alignment to align to
 * @return `size_t` the up-aligned value
 */
static size_t __alignUp(size_t v, size_t a) 
{return (v + a - 1) / a * a;};

/**
 * @brief internal helper: choose smallest section that fits
 * 
 * @param list the list of sections to search in
 * @param requiredBytes the size of the section that is required
 * @param alignment the required alignment of the allocation
 */
static size_t __findBestFit(const std::vector<GLGE::Graphic::Backend::Graphic::MeshPool::LODInfo::Section>& list, size_t requiredBytes, size_t alignment) {
    //store the best index
    size_t bestSize = SIZE_MAX;
    size_t bestIndex = SIZE_MAX;
    //iterate and search in the free list
    for (size_t i = 0; i < list.size(); ++i) {
        //get the current section (utility, likely to compile / optimize away)
        const GLGE::Graphic::Backend::Graphic::MeshPool::LODInfo::Section& b = list[i];

        //get the EXTENT of the section (e.g. begin / end "iterators")
        size_t begin = b.offset;
        size_t end   = b.offset + b.size * b.count;

        //align the section to the required alignment
        size_t aligned = __alignUp(begin, alignment);
        //sanity check that space is left
        if (aligned >= end) continue;
        //compute the actually usable space
        size_t usable = end - aligned;
        //update based on the USABLE space
        if (usable >= requiredBytes && usable < bestSize) {
            bestSize = usable;
            bestIndex = i;
        }
    }
    //return the index of the best fitting region
    return bestIndex;
}

/**
 * @brief internal helper: allocate from a free-list or expand buffer
 */
static GLGE::Graphic::Backend::Graphic::MeshPool::LODInfo::Section __allocateSection(std::vector<GLGE::Graphic::Backend::Graphic::MeshPool::LODInfo::Section>& freeList, const void* data,
                size_t elementCount, size_t elementSize, size_t& currentBufferSize, GLGE::u32& bufferObject, GLGE::u32& lastBuffer, bool isVBO, void** buffer)
{
    //alias to not constantly type out the full section name
    using Section = GLGE::Graphic::Backend::Graphic::MeshPool::LODInfo::Section;

    //try to find the best fitting section
    const size_t requiredBytes = elementCount * elementSize;
    size_t bestIndex = __findBestFit(freeList, requiredBytes, elementSize);

    //allocate a temporary staging buffer
    GLGE::u32 staging = 0;
    glCreateBuffers(1, &staging);
    glNamedBufferData(staging, requiredBytes, data, GL_STATIC_DRAW);

    //section to return
    Section out{};

    //no best fit found
    if (bestIndex != SIZE_MAX) {
        //allocate inside free block
        Section block = freeList[bestIndex];

        //store the block
        out.offset = block.offset;
        out.count  = elementCount;
        out.size   = elementSize;

        //aliasing for the block front / end "iterator"
        size_t blockBegin = block.offset;
        size_t blockEnd   = block.offset + block.size * block.count;

        //align the data
        size_t alignedOffset = __alignUp(blockBegin, elementSize);
        size_t allocationEnd = alignedOffset + requiredBytes;

        //update if data is left free after the allocation
        if (allocationEnd <= blockEnd) {
            //store the block data
            out.offset = alignedOffset;
            out.count  = elementCount;
            out.size   = elementSize;

            //remove old block
            freeList.erase(freeList.begin() + bestIndex);

            //prefix (alignment gap)
            if (alignedOffset > blockBegin) {
                Section prefix;
                prefix.offset = blockBegin;
                prefix.size   = 1;
                prefix.count  = alignedOffset - blockBegin;
                freeList.push_back(prefix);
            }

            //suffix (remaining memory)
            if (allocationEnd < blockEnd) {
                Section suffix;
                suffix.offset = allocationEnd;
                suffix.size   = 1;
                suffix.count  = blockEnd - allocationEnd;
                freeList.push_back(suffix);
            }
        }


        //fill in the new data
        glCopyNamedBufferSubData(staging, bufferObject, 0, out.offset, requiredBytes);
        memcpy(reinterpret_cast<GLGE::u8*>(*buffer) + out.offset, data, requiredBytes);

        //clean the staging buffer
        glDeleteBuffers(1, &staging);
        //return the final section
        return out;
    }

    //no free block fits, so extend the buffer
    out.offset = currentBufferSize;
    out.count  = elementCount;
    out.size   = elementSize;
    size_t oldSize = currentBufferSize;
    currentBufferSize += requiredBytes;

    //create a new buffer
    GLGE::u32 buff = 0;
    glCreateBuffers(1, &buff);
    //in debug name the buffers
    #if GLGE_DEBUG
    if (isVBO) {
        std::string name = std::string("Vertex Buffer");
        glObjectLabel(GL_BUFFER, buff, name.length(), name.c_str());
    } else {
        std::string name = std::string("Index Buffer");
        glObjectLabel(GL_BUFFER, buff, name.length(), name.c_str());
    }
    #endif
    //allocate the new GPU buffer
    glNamedBufferData(buff, currentBufferSize, nullptr, GL_STATIC_DRAW);
    void* newShadow = malloc(currentBufferSize);

    //fill the new buffer
    if (bufferObject) {
        //carry the data over
        glCopyNamedBufferSubData(bufferObject, buff, 0, 0, oldSize);
        memcpy(newShadow, *buffer, oldSize);
        free(*buffer);
        *buffer = newShadow;
    }
    //write in the new data
    glCopyNamedBufferSubData(staging, buff, 0, out.offset, requiredBytes);
    memcpy(reinterpret_cast<GLGE::u8*>(*buffer) + out.offset, data, requiredBytes);

    //clean the staging buffer
    glDeleteBuffers(1, &staging);
    //if an old buffer exists, delete it
    if (lastBuffer) {glDeleteBuffers(1, &lastBuffer);}
    //update buffer storage and orphan the last buffer
    lastBuffer = bufferObject;
    glNamedBufferData(lastBuffer, 0, nullptr, GL_STATIC_DRAW);
    bufferObject = buff;

    //in debug update the old buffer name
    #if GLGE_DEBUG
    if (isVBO) {
        std::string name = std::string("Old Vertex Buffer");
        glObjectLabel(GL_BUFFER, lastBuffer, name.length(), name.c_str());
    } else {
        std::string name = std::string("Old Index Buffer");
        glObjectLabel(GL_BUFFER, lastBuffer, name.length(), name.c_str());
    }
    #endif

    //return the section
    return out;
}

GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool::MeshPool() {
    //create the VBO and IBO
    glCreateBuffers(1, &m_vbo);
    glCreateBuffers(1, &m_ibo);
    //in debug name the buffers
    #if GLGE_DEBUG
    {
    std::string name = std::string("Vertex Buffer");
    glObjectLabel(GL_BUFFER, m_vbo, name.length(), name.c_str());
    }
    {
    std::string name = std::string("Index Buffer");
    glObjectLabel(GL_BUFFER, m_ibo, name.length(), name.c_str());
    }
    #endif
}

GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool::~MeshPool() {
    //delete the VBO and IBO
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ibo);
}


GLGE::u64 GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool::allocate(const void* vertices, size_t vertexSize, size_t vertexCount, const u32* indices, size_t indexCount, const LODInfo* lod, u8 LODCount, const VertexAttribute* attributes, u64 attributeCount) {
    //get the new entity ID
    u64 id = UINT64_MAX;
    //check if free list entries exist and get the next ID correctly
    if (m_freeTop == SIZE_MAX) {
        id = m_slots.size();
        m_slots.emplace_back();
    } else {
        id = m_freeTop;
        m_freeTop = m_slots[id].data.freeList;
    }
    //store the metadata for the LODs
    m_slots[id].alive = true;
    new (&m_slots[id].data.data) MetaData();
    m_slots[id].data.data.lodCount = LODCount;
    MetaData& meta = m_slots[id].data.data;
    for (size_t i = 0; i < LODCount; ++i) 
    {meta.lod[i] = lod[i];}

    //sanity check the attribute size
    if (attributeCount > VertexLayout::MAX_ATTRIBUTE_COUNT)
    {throw Exception("Specified too many attributes for a single vertex", "GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool::allocate");}

    //validate that the size is sane (the inputted vertex size must be at least the sum of all specified attributes)
    u64 attrSize = 0;
    for (u64 i = 0; i < attributeCount; ++i) 
    {attrSize += VertexAttribute::getSizeOfFormat(attributes[i].getFormat());}
    if (attrSize > vertexSize)
    {throw Exception("The sum of all attributes is too large for the specified vertex size", "GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool::allocate");}

    //store the vertex attributes
    for (u64 i = 0; i < attributeCount; ++i) 
    {meta.attributes[i] = attributes[i];}
    meta.attributeCount = attributeCount;

    //compute required bytes
    size_t vertexBytes = vertexSize  * vertexCount;
    size_t indexBytes  = sizeof(u32) * indexCount;

    //look for the smallest free sections that would fit the buffer
    meta.vertex = __allocateSection(m_vertexFreeList, vertices, vertexCount, vertexSize,  m_vboSize, m_vbo, m_lastVBO, true,  &m_vboCopy);
    meta.index  = __allocateSection(m_indexFreeList,  indices,  indexCount,  sizeof(u32), m_iboSize, m_ibo, m_lastIBO, false, &m_iboCopy);

    //return the identifier
    return id;
}

GLGE::u8 GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool::getLODCount(u64 meshId) const {
    //access the LOD count and return it, return 0 on failure
    return (meshId < m_slots.size()) ? (m_slots[meshId].alive ? m_slots[meshId].data.data.lodCount : 0) : 0;
}

void GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool::destroy(u64 meshId) {
    //protect unused slots
    if (m_slots.size() < meshId) {return;}
    if (!m_slots[meshId].alive) {return;}

    //access the mesh data
    MetaData& meta = m_slots[meshId].data.data;

    //return the section to the free list
    m_vertexFreeList.push_back(meta.vertex);
    m_indexFreeList.push_back(meta.index);

    //push to the index free list
    m_slots[meshId].alive = false;
    m_slots[meshId].data.freeList = m_freeTop;
    m_freeTop = meshId;
}

GLGE::Graphic::Backend::Graphic::MeshPool::LODInfo GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool::getLODInfo(u64 meshId, u8 lod) const {
    //return the LOD info
    return m_slots[meshId].data.data.lod[lod];
}

GLGE::u64 GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool::getVertexCount(u64 meshId) const {
    //return the total vertex count
    return m_slots[meshId].data.data.vertex.count;
}

const void* GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool::getVertexData(u64 meshId) const {
    //return a pointer to the vertex data
    return reinterpret_cast<u8*>(m_vboCopy) + m_slots[meshId].data.data.vertex.offset;
}

GLGE::u64 GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool::getVertexSize(u64 meshId) const {
    //return the vertex size in bytes
    return m_slots[meshId].data.data.vertex.size;
}


GLGE::u64 GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool::getIndexCount(u64 meshId) const {
    //return the total index count
    return m_slots[meshId].data.data.index.count;
}

const void* GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool::getIndexData(u64 meshId) const {
    //return a pointer to the index data
    return reinterpret_cast<u8*>(m_iboCopy) + m_slots[meshId].data.data.index.offset;
}

GLGE::u8 GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool::getIndexTypeSize(u64 meshId) const {
    //return the size of the index type
    return m_slots[meshId].data.data.index.size;
}

const GLGE::Graphic::Backend::Graphic::MeshPool::LODInfo::Section& GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool::getIndexSection(u64 meshId) const {
    //return the section
    return m_slots[meshId].data.data.index;
}

const GLGE::Graphic::Backend::Graphic::MeshPool::LODInfo::Section& GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool::getVertexSection(u64 meshId) const {
    //return the section
    return m_slots[meshId].data.data.vertex;
}

GLGE::u64 GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool::getVertexAttributeCount(u64 meshId) const 
{return m_slots[meshId].data.data.attributeCount;}

GLGE::Graphic::VertexAttribute GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool::getVertexAttribute(u64 meshId, u64 attributeId) const 
{return m_slots[meshId].data.data.attributes[attributeId];}