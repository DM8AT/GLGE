/**
 * @file MeshManager.cpp
 * @author DM8AT
 * @brief implement the mesh manager class
 * @version 0.1
 * @date 2026-08-20
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "Graphic/Backend/Graphics/MeshManager.h"
#include "Graphic/Instance.h"

//for resizable arrays
#include <vector>

//ok in impl file
using namespace GLGE::Graphic::Backend::Graphic;

MeshManager::MeshManager(GLGE::Graphic::Instance* instance)
    : m_gPool(instance), m_inst(instance) {}

void MeshManager::ensureMetaBufferCapacity(size_t requiredMeshSlots, size_t requiredLodSlots) {
    //check for buffer creation
    if (!m_meshMetaBuff.get()) {
        //start with a sane minimum
        size_t initialMeshSize = std::max<size_t>(1024, requiredMeshSlots) * sizeof(MeshMeta);
        size_t initialLodSize  = std::max<size_t>(4096, requiredLodSlots) * sizeof(LODMeta);

        //crate the actual buffers using the description
        m_meshMetaBuff = m_inst->getGraphicDescription()->createBuffer(
            Buffer::Type::STORAGE, nullptr, initialMeshSize,
            Buffer::Usage::STREAMING_UPLOAD, m_inst->getGraphicBackendInstance().get()
        );
        m_lodBuff = m_inst->getGraphicDescription()->createBuffer(
            Buffer::Type::STORAGE, nullptr, initialLodSize,
            Buffer::Usage::STREAMING_UPLOAD, m_inst->getGraphicBackendInstance().get()
        );
        return;
    }

    //dynamic expansion without full rebuild
    if (requiredMeshSlots * sizeof(MeshMeta) > m_meshMetaBuff->getSize()) 
    {m_meshMetaBuff->resize(m_meshMetaBuff->getSize() * 2);}
    if (requiredLodSlots * sizeof(LODMeta) > m_lodBuff->getSize()) 
    {m_lodBuff->resize(m_lodBuff->getSize() * 2);}
}

void MeshManager::updateMeshMetaGPU(u32 slot, const MeshMeta& meta) {
    //write the buffer and then sync CPU to GPU
    m_meshMetaBuff->write(&meta, sizeof(MeshMeta), slot * sizeof(MeshMeta));
    m_meshMetaBuff->sync(false);
}

void MeshManager::updateLODMetaGPU(u32 lodOffset, const LODMeta* lods, u32 count) {
    //write the buffer and then sync CPU to GPU
    m_lodBuff->write(lods, count * sizeof(LODMeta), lodOffset * sizeof(LODMeta));
    m_lodBuff->sync(false);
}

MeshHandle MeshManager::createMesh(const GLGE::Mesh& mesh, const GLGE::Graphic::VertexLayout& layout) {
    //extract layout attributes
    std::vector<GeometryPool::AttributeIdentifier> identifiers;
    identifiers.reserve(layout.getAttributeCount());
    for (size_t i = 0; i < layout.getAttributeCount(); ++i) {
        const auto& attr = layout.getAttribute(i);
        identifiers.emplace_back(attr.usage, static_cast<u8>(attr.type), attr.streamId);
    }
    //get archetype ID
    auto& archetype = m_gPool.acquireArchetype(identifiers);
    u32 archetypeId = archetype.getId();

    //deduplication check
    MeshIdentifier identifier{mesh.getUUID(), archetypeId};
    auto it = m_handleMap.find(identifier);
    if (it != m_handleMap.end()) {
        MeshHandle existingHandle = it->second;
        u32 idx = existingHandle & MESH_HANDLE_INDEX_MASK;
        m_entries[idx].refCount++;
        //duplicate -> early out
        return existingHandle;
    }

    //allocate geometry in bulk
    u32 lodCount = static_cast<u32>(mesh.getLODCount());
    std::vector<LODMeta> lodMetas(lodCount);
    std::vector<GeometryPool::Allocation> lodAllocations(lodCount);
    for (u32 i = 0; i < lodCount; ++i) {
        const auto& lod = mesh.getLOD(i);
        auto alloc = m_gPool.allocate(lod.getVertexCount(), lod.getIndexCount(), identifiers);
        lodAllocations[i] = alloc;

        //load metadata
        lodMetas[i].error        = lod.getError();
        lodMetas[i].indexCount   = alloc.indexCount;
        lodMetas[i].indexOffset  = alloc.indexOffset;
        lodMetas[i].vertexCount  = alloc.vertexCount;
        lodMetas[i].vertexOffset = alloc.vertexOffset;

        //bulk write indices
        GeometryPool::AttributeIdentifier indexIdent(getTypeHash64<GeometryPool::Index>(), static_cast<u8>(GLGE::Mesh::Type::UInt), 0);
        m_gPool.write(alloc, indexIdent, lod.indices().data(), 0, lod.getIndexCount(), 0);

        //split vertex data into streams on CPU, upload full stream to GPU
        for (size_t k = 0; k < layout.getStreamCount(); ++k) {
            //compute the stream size
            size_t elSize = 0;
            for (const auto& attr : identifiers) {
                if (attr.stream == k) {
                    const auto& a = layout.getAttribute(layout.getIdxOfUsage(attr.usage));
                    size_t cap = a.streamOffset + GLGE::Mesh::VertexAttribute::getTypeInfo(a.type).size;
                    elSize = std::max<size_t>(elSize, cap);
                }
            }
            size_t count = lod.vertices().getCount();
            size_t totalSize = count * elSize;
            auto ptr = std::make_unique<u8[]>(totalSize);

            //write to data
            for (size_t k = 0; k < lod.vertices().getCount(); ++k) {
                for (size_t j = 0; j < identifiers.size(); ++j) {
                    const auto& a = layout.getAttribute(layout.getIdxOfUsage(identifiers[j].usage));
                    //write the data
                    memcpy(ptr.get() + (k * elSize) + a.streamOffset, lod.vertices().get(k).get(a.usage), GLGE::Mesh::VertexAttribute::getTypeInfo(a.type).size);
                }
            }

            //write the filled stream to the GPU
            m_gPool.write(alloc, identifiers[k], ptr.get(), 0, lod.vertices().getCount(), 0);
        }
    }

    //reserve slot index
    u32 slotIndex = 0;
    u8 version = 0;
    if (m_freeTop == UINT32_MAX) {
        slotIndex = static_cast<u32>(m_entries.size());
        if (slotIndex >= MESH_HANDLE_INDEX_MASK) 
        {throw GLGE::Exception("Mesh handle index overflow", "MeshManager::createMesh");}
        m_entries.emplace_back();
    } else {
        slotIndex = m_freeTop;
        version = m_entries[slotIndex].version;
        m_freeTop = m_entries[slotIndex].nextFree;
    }

    //update holding
    u32 currentLodOffset = m_nextLodFreeSlot;
    m_nextLodFreeSlot += lodCount;

    //make sure that the meta buffers are large enough
    ensureMetaBufferCapacity(m_entries.size(), m_nextLodFreeSlot);

    //populate entry
    MeshEntry& entry = m_entries[slotIndex];
    entry.refCount = 1;
    entry.nextFree = UINT32_MAX;
    entry.lodOffset = currentLodOffset;
    entry.lodCount = lodCount;
    entry.archetypeId = archetypeId;
    entry.alive = true;
    entry.version = version;
    entry.identifier = identifier;
    entry.lodAllocations = std::move(lodAllocations);

    //update GPU meta buffers incrementally
    AABB aabb = mesh.getLOD(0).getBVH().getNode(0).aabb;
    MeshMeta meta {
        .lodOffset = currentLodOffset,
        .lodCount = lodCount,
        .lowerBound = {aabb.getMin().x, aabb.getMin().y, aabb.getMin().z},
        .upperBound = {aabb.getMax().x, aabb.getMax().y, aabb.getMax().z}
    };

    //update the meta buffers
    updateMeshMetaGPU(slotIndex, meta);
    updateLODMetaGPU(currentLodOffset, lodMetas.data(), lodCount);

    //crate the handle
    MeshHandle handle = (static_cast<u32>(version) << MESH_HANDLE_VERSION_SHIFT) | (slotIndex & MESH_HANDLE_INDEX_MASK);
    m_handleMap[identifier] = handle;

    //return the handle
    return handle;
}

void MeshManager::copyHandle(MeshHandle handle) {
    //get the index of the mesh and the meshes' version
    u32 idx = handle & MESH_HANDLE_INDEX_MASK;
    u8 version = static_cast<u8>((handle & ~MESH_HANDLE_INDEX_MASK) >> MESH_HANDLE_VERSION_SHIFT);

    //sanity check the handle
    if (idx >= m_entries.size()) return;
    MeshEntry& entry = m_entries[idx];
    if (!entry.alive || entry.version != version) return;

    //valid -> increase reference count
    ++m_entries[idx].refCount;
}

void MeshManager::freeMesh(MeshHandle handle) {
    //get the index of the mesh and the meshes' version
    u32 idx = handle & MESH_HANDLE_INDEX_MASK;
    u8 version = static_cast<u8>((handle & ~MESH_HANDLE_INDEX_MASK) >> MESH_HANDLE_VERSION_SHIFT);

    //sanity check the handle
    if (idx >= m_entries.size()) return;
    MeshEntry& entry = m_entries[idx];
    if (!entry.alive || entry.version != version) return;

    //decrease refcount, check if it is 0
    if (--entry.refCount == 0) {
        //free pool geometry
        for (auto& alloc : entry.lodAllocations) 
        {m_gPool.free(alloc);}
        entry.lodAllocations.clear();

        //remove from handle map
        m_handleMap.erase(entry.identifier);

        //release entry
        entry.alive = false;
        entry.version = (entry.version) & ~MESH_HANDLE_INDEX_MASK;
        entry.nextFree = m_freeTop;
        m_freeTop = idx;
    }
}