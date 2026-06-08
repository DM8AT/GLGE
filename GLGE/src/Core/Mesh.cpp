/**
 * @file Mesh.cpp
 * @author DM8AT
 * @brief implement the mesh class functions
 * @version 0.1
 * @date 2026-06-05
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add meshes
#include "Core/Mesh.h"

//add mesh optimizer
#include <meshoptimizer.h>
//add embree
#include <embree4/rtcore.h>

struct _Vertex {GLGE::f32 x, y, z;};

struct _Triangle {GLGE::u32 a, b, c;};

struct TempNode {
    GLGE::AABB bounds;
    TempNode* children[GLGE::Mesh::BVH::MaxChildCount] {};
    GLGE::u8 childCount = 0;
    std::vector<GLGE::u64> triangles;
};

struct BuildContext {
    std::vector<TempNode*> allocatedNodes;
    std::mutex nodeMtx;
};

inline static GLGE::vec3 getPositionFromVertex(GLGE::Mesh::Vertex vert, GLGE::Mesh::Type type) {
    switch (type) {
    case GLGE::Mesh::Type::Int:        return GLGE::vec3(*vert->get<GLGE::i32, GLGE::VertexAttribute::Position>(), 0, 0);
    case GLGE::Mesh::Type::UInt:       return GLGE::vec3(*vert->get<GLGE::u32, GLGE::VertexAttribute::Position>(), 0, 0);
    case GLGE::Mesh::Type::Float:      return GLGE::vec3(*vert->get<GLGE::f32, GLGE::VertexAttribute::Position>(), 0, 0);
    case GLGE::Mesh::Type::vec2:       return GLGE::vec3(*vert->get<GLGE::vec2, GLGE::VertexAttribute::Position>(), 0);
    case GLGE::Mesh::Type::vec3:       return GLGE::vec3(*vert->get<GLGE::vec3, GLGE::VertexAttribute::Position>());
    case GLGE::Mesh::Type::vec4:       return GLGE::vec3(*vert->get<GLGE::vec4, GLGE::VertexAttribute::Position>());
    case GLGE::Mesh::Type::ivec2:      return GLGE::vec3(*vert->get<GLGE::ivec2, GLGE::VertexAttribute::Position>(), 0);
    case GLGE::Mesh::Type::ivec3:      return GLGE::vec3(*vert->get<GLGE::ivec3, GLGE::VertexAttribute::Position>());
    case GLGE::Mesh::Type::ivec4:      return GLGE::vec3(*vert->get<GLGE::ivec4, GLGE::VertexAttribute::Position>());
    case GLGE::Mesh::Type::uvec2:      return GLGE::vec3(*vert->get<GLGE::uvec2, GLGE::VertexAttribute::Position>(), 0);
    case GLGE::Mesh::Type::uvec3:      return GLGE::vec3(*vert->get<GLGE::uvec3, GLGE::VertexAttribute::Position>());
    case GLGE::Mesh::Type::uvec4:      return GLGE::vec3(*vert->get<GLGE::uvec4, GLGE::VertexAttribute::Position>());
    case GLGE::Mesh::Type::unorm_u8x1: return GLGE::vec3(*vert->get<GLGE::u8, GLGE::VertexAttribute::Position>() / GLGE::f32(0xff), 0, 0);
    case GLGE::Mesh::Type::unorm_u8x2: return GLGE::vec3(GLGE::vec2(*vert->get<GLGE::u8vec2_p, GLGE::VertexAttribute::Position>()) / GLGE::vec2(0xff), 0);
    case GLGE::Mesh::Type::unorm_u8x4: return GLGE::vec3(GLGE::vec3(*vert->get<GLGE::u8vec4_p, GLGE::VertexAttribute::Position>()) / GLGE::vec3(0xff));
    case GLGE::Mesh::Type::snorm_u8x1: return GLGE::vec3(*vert->get<GLGE::i8, GLGE::VertexAttribute::Position>() / GLGE::f32(0x7f), 0, 0);
    case GLGE::Mesh::Type::snorm_u8x2: return GLGE::vec3(GLGE::vec2(*vert->get<GLGE::i8vec2_p, GLGE::VertexAttribute::Position>()) / GLGE::vec2(0x7f), 0);
    case GLGE::Mesh::Type::snorm_u8x4: return GLGE::vec3(GLGE::vec3(*vert->get<GLGE::i8vec4_p, GLGE::VertexAttribute::Position>()) / GLGE::vec3(0x7f));

    default: return GLGE::vec3(0);
    }
}

GLGE::u32 flattenNode(TempNode* node, std::vector<GLGE::Mesh::BVH::Node>& nodes, std::vector<GLGE::u64>& triangleIdxIdx) {
    //get the node ID
    GLGE::u32 nodeId = static_cast<GLGE::u32>(nodes.size());

    //create the node
    nodes.emplace_back();

    //store the AABB
    nodes[nodeId].aabb = node->bounds;

    //store the triangles
    if (node->triangles.size() > 0) {
        nodes[nodeId].data.leaf.firstTriangleIdxIdx = triangleIdxIdx.size();
        nodes[nodeId].data.leaf.triangleCount = node->triangles.size();
        triangleIdxIdx.insert(triangleIdxIdx.end(), node->triangles.begin(), node->triangles.end());
    }

    //store the children
    nodes[nodeId].childCount = node->childCount;
    for (GLGE::u8 i = 0; i < nodes[nodeId].childCount; ++i) 
    {nodes[nodeId].data.childrenIds[i] = flattenNode(node->children[i], nodes, triangleIdxIdx);}

    //return the ID
    return nodeId;
}

GLGE::Mesh::BVH::BVH(LOD* lod, bool highQuality, u32 minLeafTriangles, u32 maxLeafTriangles) 
 : m_lod(lod)
{
    //get the embree device
    RTCDevice dev = reinterpret_cast<RTCDevice>(lod->getInstance()->getEmbreeDevice());

    #if 0
    //create the geometry data
    RTCGeometry geom = rtcNewGeometry(dev, RTC_GEOMETRY_TYPE_TRIANGLE);
    //write the vertex data
    _Vertex* vb = reinterpret_cast<_Vertex*>(rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(_Vertex), lod->getVertexCount()));
    GLGE::Mesh::Type type = lod->getVertex(0).getLayout().getAttribute<GLGE::VertexAttribute::Position>().type;
    size_t i = 0;
    for (const auto& vert : lod->vertices()) {
        GLGE::vec3 pos = getPositionFromVertex(vert, type);
        vb[i++] = _Vertex {
            .x = pos.x,
            .y = pos.y,
            .z = pos.z
        };
    }
    //write the index data
    _Triangle* tb = reinterpret_cast<_Triangle*>(rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(_Triangle), lod->getIndexCount()));
    i = 0;
    for (const auto& tri : lod->indices()) {
        tb[i++] = _Triangle{
            .a = tri.a,
            .b = tri.b,
            .c = tri.c
        };
    }
    #endif

    //get all vertex positions
    std::vector<vec3> positions(lod->vertices().getCount());
    GLGE::Mesh::Type type = lod->getVertex(0).getLayout().getAttribute<GLGE::VertexAttribute::Position>().type;
    for (size_t i = 0; i < positions.size(); ++i)
    {positions[i] = getPositionFromVertex(lod->vertices().get(i), type);}

    std::vector<RTCBuildPrimitive> primitives(lod->indices().getCount());
    for (u64 i = 0; i < lod->indices().getCount(); ++i) {
        const auto& triangle = lod->indices().get(i);
        GLGE::vec3 pos[3] {
            positions[triangle.a],
            positions[triangle.b],
            positions[triangle.c]
        };
        primitives[i] = RTCBuildPrimitive {
            .lower_x = glm::min(pos[0].x, pos[1].x, pos[2].x),
            .lower_y = glm::min(pos[0].y, pos[1].y, pos[2].y),
            .lower_z = glm::min(pos[0].z, pos[1].z, pos[2].z),
            .geomID = 0,
            .upper_x = glm::max(pos[0].x, pos[1].x, pos[2].x),
            .upper_y = glm::max(pos[0].y, pos[1].y, pos[2].y),
            .upper_z = glm::max(pos[0].z, pos[1].z, pos[2].z),
            .primID = static_cast<unsigned int>(i)
        };
    }

    BuildContext ctx;

    //create the BVH
    RTCBVH bvh = rtcNewBVH(dev);
    if (!bvh) 
    {throw GLGE::Exception(rtcGetDeviceLastErrorMessage(dev), "GLGE::Mesh::BVH::BVH");}
    //store the BVH build arguments
    RTCBuildArguments args = rtcDefaultBuildArguments();
    args.buildQuality = highQuality ? RTC_BUILD_QUALITY_HIGH : RTC_BUILD_QUALITY_MEDIUM;
    args.bvh = bvh;
    args.primitives = reinterpret_cast<RTCBuildPrimitive*>(primitives.data());
    args.primitiveCount = primitives.size();
    args.primitiveArrayCapacity = primitives.capacity();
    args.userPtr = &ctx;
    args.maxBranchingFactor = 4;
    args.maxDepth = 32;
    args.minLeafSize = minLeafTriangles;
    args.maxLeafSize = maxLeafTriangles;
    args.createNode = [](RTCThreadLocalAllocator allocator, unsigned int childCount, void *userPtr) -> void* {
        //get the context
        auto* ctx = reinterpret_cast<BuildContext*>(userPtr);
        //create the new node
        TempNode* node = new TempNode();
        node->childCount = static_cast<u8>(childCount);
        //store in the context
        {
        std::unique_lock lock(ctx->nodeMtx);
        ctx->allocatedNodes.push_back(node);
        }

        //return the node
        return node;
    };
    args.setNodeChildren = [](void* nodePtr, void** children, unsigned int childCount, void* userPtr) -> void {
        //get the node
        TempNode* node = reinterpret_cast<TempNode*>(nodePtr);
        //update the child count
        node->childCount = static_cast<u8>(childCount);
        //fill in the child nodes
        for (unsigned int i = 0; i < childCount; ++i)
        {node->children[i] = static_cast<TempNode*>(children[i]);}
    };
    args.setNodeBounds = [](void* nodePtr, const RTCBounds** bounds, unsigned int childCount, void* userPtr) -> void {
        //get the node
        auto* node = static_cast<TempNode*>(nodePtr);

        //store the minimum and maximum
        vec3 lower(FLT_MAX);
        vec3 upper(-FLT_MAX);
        //update the bounds
        for (unsigned int i = 0; i < childCount; ++i) {
            lower.x = glm::min(lower.x, bounds[i]->lower_x);
            lower.y = glm::min(lower.y, bounds[i]->lower_y);
            lower.z = glm::min(lower.z, bounds[i]->lower_z);

            upper.x = glm::max(upper.x, bounds[i]->upper_x);
            upper.y = glm::max(upper.y, bounds[i]->upper_y);
            upper.z = glm::max(upper.z, bounds[i]->upper_z);
        }
        //store the bounds
        node->bounds = AABB(lower, upper);
    };
    args.createLeaf = [](RTCThreadLocalAllocator allocator, const RTCBuildPrimitive* prims, size_t primitiveCount, void* userPtr) -> void* {
        //get the context
        auto* ctx = static_cast<BuildContext*>(userPtr);

        //create the leaf node
        TempNode* leaf = new TempNode();
        //store the leaf node
        {
        std::unique_lock lock(ctx->nodeMtx);
        ctx->allocatedNodes.push_back(leaf);
        }
        //make space for all primitives
        leaf->triangles.reserve(primitiveCount);

        //store the minimum and maximum
        vec3 lower(FLT_MAX);
        vec3 upper(-FLT_MAX);
        //iterate over all primitives
        for (unsigned int i = 0; i < primitiveCount; ++i) {
            //update the bounds
            lower.x = glm::min(lower.x, prims[i].lower_x);
            lower.y = glm::min(lower.y, prims[i].lower_y);
            lower.z = glm::min(lower.z, prims[i].lower_z);

            upper.x = glm::max(upper.x, prims[i].upper_x);
            upper.y = glm::max(upper.y, prims[i].upper_y);
            upper.z = glm::max(upper.z, prims[i].upper_z);

            //store the primitive
            leaf->triangles.push_back(prims[i].primID);
        }
        //store the bounds
        leaf->bounds = AABB(lower, upper);

        //return the leaf
        return leaf;
    };

    //build the BVH
    TempNode* root = reinterpret_cast<TempNode*>(rtcBuildBVH(&args));
    if (!root) 
    {throw GLGE::Exception(rtcGetDeviceLastErrorMessage(dev), "GLGE::Mesh::BVH::BVH");}

    //now, the tree must be flattened into storage
    m_nodes.reserve(ctx.allocatedNodes.size());
    flattenNode(root, m_nodes, m_triangleIndexIdx); //no need to store root, root is always 0

    //clean the temporary tree
    for (TempNode* tmpNode : ctx.allocatedNodes)
    {delete tmpNode;}

    //release the BVH
    rtcReleaseBVH(bvh);
}

GLGE::Mesh::LOD::LOD(LOD* from, float targetError) 
 : m_vertices(nullptr, 0, from->getVertex(0).getLayout()), m_indices({})
{
    //get all vertex positions
    std::vector<vec3> positions(from->vertices().getCount());
    GLGE::Mesh::Type type = from->getVertex(0).getLayout().getAttribute<GLGE::VertexAttribute::Position>().type;
    for (size_t i = 0; i < positions.size(); ++i)
    {positions[i] = getPositionFromVertex(from->vertices().get(i), type);}

    //flatten the indices
    std::vector<u32> indices;
    indices.reserve(from->indices().getCount()*3);
    for (const auto& tri : from->indices()) {
        indices.push_back(tri.a);
        indices.push_back(tri.b);
        indices.push_back(tri.c);
    }

    //store the actual error
    float actualError = 0.f;
    //store the simplified mesh
    std::vector<u32> simplified(indices.size());
    size_t targetIdxCount = simplified.size();
    size_t newIdxCount = 0;

    //downsample
    do {
        //reduce target size
        targetIdxCount = std::max<size_t>(targetIdxCount * (0.75f), 3);

        //simplify the mesh
        newIdxCount = meshopt_simplify(
            simplified.data(), 
            indices.data(), 
            indices.size(), 
            reinterpret_cast<const float*>(positions.data()), 
            positions.size(), 
            sizeof(vec3), 
            targetIdxCount,
            targetError, 
            0, 
            &actualError
        );
    } while (newIdxCount == targetIdxCount && targetIdxCount > 3);

    //shrink the index buffer
    simplified.resize(newIdxCount);

    //update the vertices
    std::vector<u32> remap(from->getVertexCount());
    size_t newVertCount = meshopt_generateVertexRemap(remap.data(), simplified.data(), simplified.size(), from->vertices().data(), from->vertices().getCount(), from->vertices().get(0).getLayout().getSize());

    //create the new vertex storage
    Vertices newVerts(nullptr, newVertCount, from->vertices().getLayout());
    //remap the data
    meshopt_remapVertexBuffer(newVerts.data(), from->vertices().data(), from->vertices().getCount(), from->vertices().getLayout().getSize(), remap.data());
    //remap the index data
    meshopt_remapIndexBuffer(simplified.data(), simplified.data(), newIdxCount, remap.data());

    //optimize the mesh
    meshopt_optimizeVertexCache(simplified.data(), simplified.data(), newIdxCount, newVertCount);
    meshopt_optimizeVertexFetch(newVerts.data(), simplified.data(), newIdxCount, newVerts.data(), newVertCount, m_vertices.get(0).getLayout().getSize());

    //create the triangle index buffer
    std::vector<Triangle> triangles;
    triangles.resize(simplified.size()/3);
    for (size_t i = 0; i < (simplified.size()/3); ++i) {
        triangles[i].a = simplified[i*3 + 0];
        triangles[i].b = simplified[i*3 + 1];
        triangles[i].c = simplified[i*3 + 2];
    }

    //store the new vertex and index buffers
    m_vertices = std::move(newVerts);
    m_indices = Indices(triangles);

    //create the BVHs
    m_bvh = BVH(this, true, 1, 16);

    //store the actual error
    m_error = actualError;
}