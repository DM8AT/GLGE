/**
 * @file MeshPool.cpp
 * @author DM8AT
 * @brief implement the vulkan mesh pool
 * @version 0.1
 * @date 2026-05-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the Vulkan mesh pool implementation
#include "Graphic/Backend/Builtin/Graphics/Vulkan/MeshPool.h"
//add frontend instances
#include "Graphic/Instance.h"
//add vulkan instances
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"

//add shared stuff
#include "Shared.h"

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
static GLGE::Graphic::Backend::Graphic::MeshPool::LODInfo::Section __allocateSection(std::vector<GLGE::Graphic::Backend::Graphic::MeshPool::LODInfo::Section>& freeList, 
    size_t elementCount, size_t elementSize)
{
    //alias to not constantly type out the full section name
    using Section = GLGE::Graphic::Backend::Graphic::MeshPool::LODInfo::Section;

    //try to find the best fitting section
    const size_t requiredBytes = elementCount * elementSize;
    size_t bestIndex = __findBestFit(freeList, requiredBytes, elementSize);

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

        //return the final section
        return out;
    }

    //caller must handle expansion
    return {};
}

/**
 * @brief upload a data into a region of the buffer
 * 
 * @param inst a pointer to the backend instance to use
 * @param dst the buffer to write to
 * @param data the data to write
 * @param size the size of the data to write
 * @param offset the offset to write to
 */
static void __uploadBufferRegion(GLGE::Graphic::Backend::Graphic::Vulkan::Instance* inst, VkBuffer dst, const void* data, size_t size, size_t offset) {
    //create the staging buffer
    StagingBuffer buff = __createStagingBuffer(reinterpret_cast<VmaAllocator>(inst->getAllocator()), size, inst->getTransferQueue().familyIdx);
    //write to the staging buffer
    memcpy(buff.mapped, data, size);

    //transfer queue ownership of the buffer to the transfer queue family
    //this must run on the graphics queue
    VkCommandBuffer init_transfer = __beginSingleTimeCommands(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkCommandPool>(inst->getGraphicsQueue().singleUsePool));
    //record the barrier
    VkBufferMemoryBarrier init_barrier {};
    init_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    init_barrier.srcAccessMask = 0;
    init_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    init_barrier.srcQueueFamilyIndex = inst->getGraphicsQueue().familyIdx;
    init_barrier.dstQueueFamilyIndex = inst->getTransferQueue().familyIdx;
    init_barrier.buffer = dst;
    init_barrier.offset = offset;
    init_barrier.size = size;
    vkCmdPipelineBarrier(init_transfer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 1, &init_barrier, 0, nullptr);
    
    //submit to graphics
    __endSingleTimeCommands(reinterpret_cast<VkDevice>(inst->getDevice()), inst->getGraphicsQueue(), reinterpret_cast<VkCommandPool>(inst->getGraphicsQueue().singleUsePool), init_transfer);

    //start a single time command buffer to copy the data over using an async upload
    VkCommandBuffer cmdBuff = __beginSingleTimeCommands(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkCommandPool>(inst->getTransferQueue().singleUsePool));

    //get buffer ownership using memory barrier
    vkCmdPipelineBarrier(cmdBuff, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 1, &init_barrier, 0, nullptr);

    //copy data
    VkBufferCopy region {};
    region.srcOffset = 0;
    region.dstOffset = offset;
    region.size = size;
    vkCmdCopyBuffer(cmdBuff, buff.buffer, dst, 1, &region);

    //transfer ownership back to graphics
    VkBufferMemoryBarrier finalize_barrier {};
    finalize_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    finalize_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    finalize_barrier.dstAccessMask = 0;
    finalize_barrier.srcQueueFamilyIndex = inst->getTransferQueue().familyIdx;
    finalize_barrier.dstQueueFamilyIndex = inst->getGraphicsQueue().familyIdx;
    finalize_barrier.buffer = dst;
    finalize_barrier.offset = offset;
    finalize_barrier.size = size;
    vkCmdPipelineBarrier(cmdBuff, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 1, &finalize_barrier, 0, nullptr);

    //submit the command buffer
    __endSingleTimeCommands(reinterpret_cast<VkDevice>(inst->getDevice()), inst->getTransferQueue(), reinterpret_cast<VkCommandPool>(inst->getTransferQueue().singleUsePool), cmdBuff);

    //graphics queue must re-obtain ownership for safe future usage
    //use another single time command buffer to obtain it
    VkCommandBuffer finalize_transfer = __beginSingleTimeCommands(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkCommandPool>(inst->getGraphicsQueue().singleUsePool));
    vkCmdPipelineBarrier(finalize_transfer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 1, &finalize_barrier, 0, nullptr);
    __endSingleTimeCommands(reinterpret_cast<VkDevice>(inst->getDevice()), inst->getGraphicsQueue(), reinterpret_cast<VkCommandPool>(inst->getGraphicsQueue().singleUsePool), finalize_transfer);

    //clean up
    __destroyStagingBuffer(reinterpret_cast<VmaAllocator>(inst->getAllocator()), buff);
}

GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::MeshPool(GLGE::Graphic::Instance* instance) 
 : GLGE::Graphic::Backend::Graphic::MeshPool(instance)
{
    //get the instance
    auto* inst = dynamic_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(instance->getGraphicBackendInstance().get());

    //pre-define some initial sizes
    m_vboSize = 1 << 20; //1 MB pre-cache
    m_iboSize = 512 * 1024; //5 KB pre-cache

    //get the graphics queue
    u32 graphicsQueue = inst->getGraphicsQueue().familyIdx;

    //create the vbo
    VkBufferCreateInfo vboCreate {};
    vboCreate.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vboCreate.size = m_vboSize;
    vboCreate.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    vboCreate.queueFamilyIndexCount = 1;
    vboCreate.pQueueFamilyIndices = &graphicsQueue;
    vboCreate.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VmaAllocationCreateInfo vboAllocCreate {};
    vboAllocCreate.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    VkBuffer vbo;
    VmaAllocation vboAlloc;
    if (vmaCreateBuffer(reinterpret_cast<VmaAllocator>(inst->getAllocator()), &vboCreate, &vboAllocCreate, &vbo, &vboAlloc, nullptr) != VK_SUCCESS)
    {throw Exception("Failed to allocate the vertex buffer object", "GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::MeshPool");}
    //store the vbo
    m_vbo = reinterpret_cast<void*>(vbo);
    m_vboAlloc = reinterpret_cast<void*>(vboAlloc);

    //create the ibo
    VkBufferCreateInfo iboCreate = vboCreate;
    iboCreate.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VmaAllocationCreateInfo iboAllocCreate = vboAllocCreate;
    VkBuffer ibo;
    VmaAllocation iboAlloc;
    if (vmaCreateBuffer(reinterpret_cast<VmaAllocator>(inst->getAllocator()), &iboCreate, &iboAllocCreate, &ibo, &iboAlloc, nullptr) != VK_SUCCESS)
    {throw Exception("Failed to allocate the index buffer object", "GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::MeshPool");}
    //store the ibo
    m_ibo = reinterpret_cast<void*>(ibo);
    m_iboAlloc = reinterpret_cast<void*>(iboAlloc);

    //add the free list sections
    m_vertexFreeList.push_back(LODInfo::Section{
        .count = 1,
        .offset = 0,
        .size = m_vboSize
    });
    m_indexFreeList.push_back(LODInfo::Section{
        .count = 1,
        .offset = 0,
        .size = m_iboSize
    });
}

GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::~MeshPool() {
    //get the instance
    auto* inst = dynamic_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance->getGraphicBackendInstance().get());

    //clean up
    vmaDestroyBuffer(reinterpret_cast<VmaAllocator>(inst->getAllocator()), reinterpret_cast<VkBuffer>(m_vbo), reinterpret_cast<VmaAllocation>(m_vboAlloc));
    vmaDestroyBuffer(reinterpret_cast<VmaAllocator>(inst->getAllocator()), reinterpret_cast<VkBuffer>(m_ibo), reinterpret_cast<VmaAllocation>(m_iboAlloc));
}

GLGE::u64 GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::allocate(const void* vertices, size_t vertexSize, size_t vertexCount, const u32* indices, size_t indexCount, const LODInfo* lod, u8 LODCount, const VertexAttribute* attributes, u64 attributeCount) {
    //get the instance
    auto* inst = dynamic_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance->getGraphicBackendInstance().get());

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
    {throw Exception("Specified too many attributes for a single vertex", "GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::allocate");}

    //validate that the size is sane (the inputted vertex size must be at least the sum of all specified attributes)
    u64 attrSize = 0;
    for (u64 i = 0; i < attributeCount; ++i) 
    {attrSize += VertexAttribute::getSizeOfFormat(attributes[i].getFormat());}
    if (attrSize > vertexSize)
    {throw Exception("The sum of all attributes is too large for the specified vertex size", "GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::allocate");}

    //store the vertex attributes
    for (u64 i = 0; i < attributeCount; ++i) 
    {meta.attributes[i] = attributes[i];}
    meta.attributeCount = attributeCount;

    //compute required bytes
    size_t vertexBytes = vertexSize  * vertexCount;
    size_t indexBytes  = sizeof(u32) * indexCount;

    //look for the smallest free sections that would fit the buffer
    meta.vertex = __allocateSection(m_vertexFreeList, vertexCount, vertexSize);
    meta.index  = __allocateSection(m_indexFreeList,  indexCount,  sizeof(u32));

    //for now hard fail if allocation failed
    if (meta.vertex.count == 0 || meta.index.count == 0)
    {throw Exception("Failed to allocate - the mesh pool ran out of memory", "GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::allocate");}

    //GPU upload
    __uploadBufferRegion(inst, reinterpret_cast<VkBuffer>(m_vbo), vertices, meta.vertex.size*meta.vertex.count, meta.vertex.offset);
    __uploadBufferRegion(inst, reinterpret_cast<VkBuffer>(m_ibo), indices,  meta.index.size*meta.index.count,   meta.index.offset);

    //return the mesh id
    return id;
}

GLGE::u8 GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::getLODCount(u64 meshId) const {
    //access the LOD count and return it, return 0 on failure
    return (meshId < m_slots.size()) ? (m_slots[meshId].alive ? m_slots[meshId].data.data.lodCount : 0) : 0;
}

void GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::destroy(u64 meshId) {
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

GLGE::Graphic::Backend::Graphic::MeshPool::LODInfo GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::getLODInfo(u64 meshId, u8 lod) const {
    //return the LOD info
    return m_slots[meshId].data.data.lod[lod];
}

GLGE::u64 GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::getVertexCount(u64 meshId) const {
    //return the total vertex count
    return m_slots[meshId].data.data.vertex.count;
}

const void* GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::getVertexData(u64 meshId) const {
    //return a pointer to the vertex data
    return reinterpret_cast<u8*>(m_vboCopy) + m_slots[meshId].data.data.vertex.offset;
}

GLGE::u64 GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::getVertexSize(u64 meshId) const {
    //return the vertex size in bytes
    return m_slots[meshId].data.data.vertex.size;
}


GLGE::u64 GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::getIndexCount(u64 meshId) const {
    //return the total index count
    return m_slots[meshId].data.data.index.count;
}

const void* GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::getIndexData(u64 meshId) const {
    //return a pointer to the index data
    return reinterpret_cast<u8*>(m_iboCopy) + m_slots[meshId].data.data.index.offset;
}

GLGE::u8 GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::getIndexTypeSize(u64 meshId) const {
    //return the size of the index type
    return m_slots[meshId].data.data.index.size;
}

const GLGE::Graphic::Backend::Graphic::MeshPool::LODInfo::Section& GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::getIndexSection(u64 meshId) const {
    //return the section
    return m_slots[meshId].data.data.index;
}

const GLGE::Graphic::Backend::Graphic::MeshPool::LODInfo::Section& GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::getVertexSection(u64 meshId) const {
    //return the section
    return m_slots[meshId].data.data.vertex;
}

GLGE::u64 GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::getVertexAttributeCount(u64 meshId) const 
{return m_slots[meshId].data.data.attributeCount;}

GLGE::Graphic::VertexAttribute GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool::getVertexAttribute(u64 meshId, u64 attributeId) const 
{return m_slots[meshId].data.data.attributes[attributeId];}