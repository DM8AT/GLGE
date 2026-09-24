/**
 * @file DebugVisualizer.cpp
 * @author DM8AT
 * @brief implement the debug visualizer
 * @version 0.1
 * @date 2026-09-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the debug renderer
#include "GLGE/Graphic/DebugRenderer.h"

/**
 * @brief a helper to apply a transform
 * 
 * @param input a list of positions to transform
 * @param vertexCount the amount of vertices in the position list
 * @param transform the transfrom to apply
 * @param output a pointer to a list to write to
 * 
 * @note input and ouput may be the same
 */
static void __helper_applyTransform(const GLGE::vec3* input, size_t vertexCount, const GLGE::Transform& transform, GLGE::vec3* output) {
    //create the transformation matrix from the transform
    GLGE::WorldTransform wt {};
    wt.pos = transform.pos;
    wt.rot = transform.rot;
    wt.scale = transform.scale;
    glm::mat4 transf = wt;

    //apply the transformation
    for (size_t i = 0; i < vertexCount; ++i) {
        GLGE::vec4 inter = transf * GLGE::vec4(input[i], 1);
        output[i] = GLGE::vec3{inter.x, inter.y, inter.z};
    }
}

static GLGE::vec3 __helper_toVec3(const void* data, GLGE::Mesh::VertexAttribute::Type type) {
    //alias for quick access with type
    using Type = GLGE::Mesh::VertexAttribute::Type;
    //switch over the specified type
    //Behaviour: 
    //1. For scalars assume that y and z are 0
    //2. For 2D values assume that z is zero
    //3. For 3D values carry them over
    //4. For 4D values discard the w component
    switch (type) {
    case Type::Unused:
        //invalid
        std::unreachable();

    //scalars
    case Type::Int:   return glm::vec3(static_cast<float>(*static_cast<const GLGE::i32*>(data)), 0.f, 0.f);
    case Type::UInt:  return glm::vec3(static_cast<float>(*static_cast<const GLGE::u32*>(data)), 0.f, 0.f);
    case Type::Float: return glm::vec3(*static_cast<const float*>(data), 0.f, 0.f);

    //float vectors
    case Type::vec2: {
        const auto* v = static_cast<const float*>(data);
        return GLGE::vec3(v[0], v[1], 0.0f);
    }
    case Type::vec3: {
        const auto* v = static_cast<const float*>(data);
        return GLGE::vec3(v[0], v[1], v[2]);
    }
    case Type::vec4: {
        const auto* v = static_cast<const float*>(data);
        return GLGE::vec3(v[0], v[1], v[2]); //discard w
    }

    //signed integer vectors
    case Type::ivec2: {
        const auto* v = static_cast<const GLGE::i32*>(data);
        return GLGE::vec3(static_cast<float>(v[0]), static_cast<float>(v[1]), 0.f);
    }
    case Type::ivec3: {
        const auto* v = static_cast<const GLGE::i32*>(data);
        return GLGE::vec3(static_cast<float>(v[0]), static_cast<float>(v[1]), static_cast<float>(v[2]));
    }
    case Type::ivec4: {
        const auto* v = static_cast<const GLGE::i32*>(data);
        return GLGE::vec3(static_cast<float>(v[0]), static_cast<float>(v[1]), static_cast<float>(v[2]));
    }

    //unsigned integer vectors
    case Type::uvec2: {
        const auto* v = static_cast<const GLGE::u32*>(data);
        return GLGE::vec3(static_cast<float>(v[0]), static_cast<float>(v[1]), 0.f);
    }
    case Type::uvec3: {
        const auto* v = static_cast<const GLGE::u32*>(data);
        return GLGE::vec3(static_cast<float>(v[0]), static_cast<float>(v[1]), static_cast<float>(v[2]));
    }
    case Type::uvec4: {
        const auto* v = static_cast<const GLGE::u32*>(data);
        return GLGE::vec3(static_cast<float>(v[0]), static_cast<float>(v[1]), static_cast<float>(v[2]));
    }

    //unsigned normalized 8-bit values
    case Type::unorm_u8x1: {
        const auto* v = static_cast<const GLGE::u8*>(data);
        return glm::vec3(static_cast<float>(v[0]) / 255.f, 0.f,0.f);
    }
    case Type::unorm_u8x2: {
        const auto* v = static_cast<const GLGE::u8*>(data);
        return glm::vec3(static_cast<float>(v[0]) / 255.f, static_cast<float>(v[1]) / 255.f, 0.f);
    }
    case Type::unorm_u8x4: {
        const auto* v = static_cast<const GLGE::u8*>(data);
        return glm::vec3(static_cast<float>(v[0]) / 255.f, static_cast<float>(v[1]) / 255.f, static_cast<float>(v[2]) / 255.f);
    }

    //signed normalized 8-bit values
    case Type::snorm_u8x1: {
        const auto* v = static_cast<const GLGE::i32*>(data);
        return glm::vec3(std::max(-1.f, static_cast<float>(v[0]) / 127.f), 0.f, 0.f);
    }
    case Type::snorm_u8x2: {
        const auto* v = static_cast<const GLGE::i32*>(data);
        return glm::vec3(std::max(-1.f, static_cast<float>(v[0]) / 127.f), std::max(-1.f, static_cast<float>(v[1]) / 127.f), 0.f);
    }
    case Type::snorm_u8x4: {
        const auto* v = static_cast<const GLGE::i32*>(data);
        return glm::vec3(std::max(-1.f, static_cast<float>(v[0]) / 127.f), std::max(-1.f, static_cast<float>(v[1]) / 127.f), std::max(-1.f, static_cast<float>(v[2]) / 127.f));
    }

    //poison so I do not forget to update this
    default: std::unreachable();
    }
}

GLGE::u32 cast_u32_safe(size_t size) {
    //in debug add a sanity check
    #if GLGE_DEBUG
    if (size > std::numeric_limits<GLGE::u32>::max()) {throw GLGE::Exception("Invalid cast: size_t value out of range for u32", "GLGE::Graphic::DebugVisualizer::InternalHelper::cast_u32_safe");}
    #endif
    //just use static cast
    return static_cast<GLGE::u32>(size);
}

void GLGE::Graphic::DebugRenderer::reset() {
    //clean the buffers
    m_points.clear();
    m_connections.clear();
    m_records.clear();
}

void GLGE::Graphic::DebugRenderer::drawAABB(const AABB& aabb, const Transform& transf, const Style& style) {
    //extract the vertices from the AABB
    vec3 verts[] = {
        vec3 {aabb.getMin().x, aabb.getMin().y, aabb.getMin().z},
        vec3 {aabb.getMin().x, aabb.getMin().y, aabb.getMax().z},
        vec3 {aabb.getMin().x, aabb.getMax().y, aabb.getMin().z},
        vec3 {aabb.getMin().x, aabb.getMax().y, aabb.getMax().z},
        vec3 {aabb.getMax().x, aabb.getMin().y, aabb.getMin().z},
        vec3 {aabb.getMax().x, aabb.getMin().y, aabb.getMax().z},
        vec3 {aabb.getMax().x, aabb.getMax().y, aabb.getMin().z},
        vec3 {aabb.getMax().x, aabb.getMax().y, aabb.getMax().z}
    };
    
    //transform AABB to world
    __helper_applyTransform(verts, sizeof(verts)/sizeof(*verts), transf, verts);

    //create the command record
    CommandRecord cm {};
    cm.style = style;

    //depending on the render mode there are 3 totally different visualizations possible
    switch (style.renderMode)
    {
    case Style::RenderMode::SOLID: {
            //solid adds all 8 points and creates 12 triangles from them
            //this can be done with only 8 points since the points have no face-specific information

            //fill the command record
            cm.indexStart = cast_u32_safe(m_connections.size());
            cm.indexCount = (3 * 12);
            cm.pointStart = cast_u32_safe(m_points.size());
            cm.pointCount = 8;

            //add the 12 triangles
            m_connections.reserve(m_connections.size() + (3 * 12));
            //Bottom
            m_connections.push_back(0); m_connections.push_back(4); m_connections.push_back(5);
            m_connections.push_back(0); m_connections.push_back(5); m_connections.push_back(1);
            //Top
            m_connections.push_back(2); m_connections.push_back(3); m_connections.push_back(7);
            m_connections.push_back(2); m_connections.push_back(7); m_connections.push_back(6);

            //Left
            m_connections.push_back(0); m_connections.push_back(1); m_connections.push_back(3);
            m_connections.push_back(0); m_connections.push_back(3); m_connections.push_back(2);
            //Right
            m_connections.push_back(4); m_connections.push_back(6); m_connections.push_back(7);
            m_connections.push_back(4); m_connections.push_back(7); m_connections.push_back(5);

            //Front
            m_connections.push_back(0); m_connections.push_back(2); m_connections.push_back(6);
            m_connections.push_back(0); m_connections.push_back(6); m_connections.push_back(4);
            //Back
            m_connections.push_back(1); m_connections.push_back(5); m_connections.push_back(7);
            m_connections.push_back(1); m_connections.push_back(7); m_connections.push_back(3);
        }
        break;
    case Style::RenderMode::WIREFRAME: {
            //add the 8 edge points and connect them with 12 lines to form a wireframe

            //fill the command record
            cm.indexStart = cast_u32_safe(m_connections.size());
            cm.indexCount = (2 * 12);
            cm.pointStart = cast_u32_safe(m_points.size());
            cm.pointCount = 8;

            //add the line connections
            m_connections.reserve(m_connections.size() + (2 * 12));
            //Bottom
            m_connections.push_back(0); m_connections.push_back(4);
            m_connections.push_back(4); m_connections.push_back(5);
            m_connections.push_back(5); m_connections.push_back(1);
            m_connections.push_back(1); m_connections.push_back(0);
            //Top
            m_connections.push_back(2); m_connections.push_back(6);
            m_connections.push_back(6); m_connections.push_back(7);
            m_connections.push_back(7); m_connections.push_back(3);
            m_connections.push_back(3); m_connections.push_back(2);
            //Vertical
            m_connections.push_back(0); m_connections.push_back(2);
            m_connections.push_back(4); m_connections.push_back(6);
            m_connections.push_back(5); m_connections.push_back(7);
            m_connections.push_back(1); m_connections.push_back(3);
        }
        break;
    case Style::RenderMode::VERTICES: {
            //just add 8 points at the vertices of the AABB

            //fill the command record
            cm.indexStart = std::numeric_limits<u32>::max();
            cm.indexCount = 0;
            cm.pointStart = cast_u32_safe(m_points.size());
            cm.pointCount = 8;
            cm.style = style;
        }
        break;
    
    default:
        //invalid
        std::unreachable();
        break;
    }
    
    //in any valid case add all 8 corners
    m_points.insert(m_points.end(), verts, verts + (sizeof(verts) / sizeof(*verts)));

    //add the command record
    m_records.push_back(cm);
}

void GLGE::Graphic::DebugRenderer::drawAABB(const AABB& aabb, Object at, World& world, const Style& style) {
    //extract the transform
    Transform transf;

    //highest priority: world transform
    WorldTransform* wt = world.get<WorldTransform>(at);
    if (wt) {
        //found -> just use it
        transf = *wt;
    } else {
        //second highest priority: normal transform
        Transform* tf = world.get<Transform>(at);

        if (tf) {
            //found -> use it
            transf = *tf;
        } else {
            //lowest priority: transform 2D
            Transform2D* tf2d = world.get<Transform2D>(at);

            if (tf2d) {
                //extent to third dimension
                transf.pos = vec3(tf2d->pos, 0);
                transf.rot = Quaternion(vec3(0,0,tf2d->angle));
                transf.scale = vec3(tf2d->scale, 1);
            } else {
                throw GLGE::Exception("Cannot use object without transform as positioner object", "GLGE::Graphic::DebugRenderer::drawAABB");
            }
        }
    }

    //just use the function with transform
    drawAABB(aabb, transf, style);
}

void GLGE::Graphic::DebugRenderer::drawBox(const vec3& extent, const Transform& transform, const Style& style) {
    //just use the AABB command

    //for that, an AABB is required. To create one, compute the half extent and use negative half extent as the min and positive half extent as the max. 
    vec3 half = extent / GLGE::vec3(2);
    drawAABB(AABB(-half, half), transform, style);
}

void GLGE::Graphic::DebugRenderer::drawBox(const vec3& extent, Object at, World& world, const Style& style) {
    //extract the transform
    Transform transf;

    //highest priority: world transform
    WorldTransform* wt = world.get<WorldTransform>(at);
    if (wt) {
        //found -> just use it
        transf = *wt;
    } else {
        //second highest priority: normal transform
        Transform* tf = world.get<Transform>(at);

        if (tf) {
            //found -> use it
            transf = *tf;
        } else {
            //lowest priority: transform 2D
            Transform2D* tf2d = world.get<Transform2D>(at);

            if (tf2d) {
                //extent to third dimension
                transf.pos = vec3(tf2d->pos, 0);
                transf.rot = Quaternion(vec3(0,0,tf2d->angle));
                transf.scale = vec3(tf2d->scale, 1);
            } else {
                throw GLGE::Exception("Cannot use object without transform as positioner object", "GLGE::Graphic::DebugRenderer::drawBox");
            }
        }
    }

    //just use the function with transform
    drawBox(extent, transf, style);
}

void GLGE::Graphic::DebugRenderer::drawDot(const vec3& position, const Style& style_in) {
    //copy the input and set the draw mode to vertices (only valid draw mode for this operation)
    Style style = style_in;
    style.renderMode = Style::RenderMode::VERTICES;

    //create the command record
    CommandRecord cm {};
    cm.style = style;
    cm.indexStart = std::numeric_limits<u32>::max();
    cm.indexCount = 0;
    cm.pointStart = cast_u32_safe(m_points.size());
    cm.pointCount = 1;

    //add the point
    m_points.push_back(position);

    //add the command record
    m_records.push_back(cm);
}

void GLGE::Graphic::DebugRenderer::drawSphere(float radius, const Transform& transform, const Style& style) {
    //UV sphere resolution
    constexpr size_t SEGMENTS = 32;
    constexpr size_t RINGS = 16;
    constexpr float PI = 3.14159265358979323846f;

    //The sphere consists of:
    // - one vertex at the north pole
    // - RINGS - 1 intermediate latitude rings
    // - one vertex at the south pole
    constexpr size_t INTERMEDIATE_RINGS = RINGS - 1;
    constexpr size_t VERTEX_COUNT = 2 + (INTERMEDIATE_RINGS * SEGMENTS);

    //generate the sphere vertices
    vec3 verts[VERTEX_COUNT];
    size_t vertexIndex = 0;

    //north pole
    verts[vertexIndex++] = vec3(0.0f, radius, 0.0f);

    //intermediate latitude rings
    for (size_t ring = 1; ring < RINGS; ++ring) {
        //theta goes from 0 at the north pole to PI at the south pole
        const float theta = PI * static_cast<float>(ring) / static_cast<float>(RINGS);

        const float sinTheta = std::sin(theta);
        const float cosTheta = std::cos(theta);

        for (size_t segment = 0; segment < SEGMENTS; ++segment) {
            //phi goes around the sphere
            const float phi = 2.0f * PI * static_cast<float>(segment) / static_cast<float>(SEGMENTS);

            const float sinPhi = std::sin(phi);
            const float cosPhi = std::cos(phi);

            verts[vertexIndex++] = vec3(radius * sinTheta * cosPhi, radius * cosTheta, radius * sinTheta * sinPhi);
        }
    }

    //south pole
    const size_t southPole = vertexIndex;
    verts[vertexIndex++] = vec3(0.0f, -radius, 0.0f);

    //transform the generated sphere into world space
    __helper_applyTransform(verts, VERTEX_COUNT, transform, verts);

    //create the command record
    CommandRecord cm {};
    cm.style = style;
    cm.pointStart = cast_u32_safe(m_points.size());
    cm.pointCount = cast_u32_safe(VERTEX_COUNT);

    switch (style.renderMode) {
    case Style::RenderMode::SOLID: {
        //Every pair of neighboring latitude rings forms SEGMENTS quads, each split into two triangles.
        //
        //There are RINGS latitude intervals:
        //   north pole -> first ring
        //   intermediate rings
        //   last ring -> south pole
        const size_t triangleCount = 2 * SEGMENTS * RINGS;

        cm.indexStart = cast_u32_safe(m_connections.size());
        cm.indexCount = cast_u32_safe(triangleCount * 3);

        m_connections.reserve(m_connections.size() + triangleCount * 3);

        //North pole -> first intermediate ring
        for (size_t segment = 0; segment < SEGMENTS; ++segment) {
            const size_t nextSegment = (segment + 1) % SEGMENTS;

            const size_t current = 1 + segment;
            const size_t next = 1 + nextSegment;

            m_connections.push_back(0);
            m_connections.push_back(static_cast<u32>(next));
            m_connections.push_back(static_cast<u32>(current));
        }

        //Intermediate rings
        for (size_t ring = 0; ring < INTERMEDIATE_RINGS - 1; ++ring) {
            const size_t currentRingStart = 1 + ring * SEGMENTS;
            const size_t nextRingStart = currentRingStart + SEGMENTS;

            for (size_t segment = 0; segment < SEGMENTS; ++segment) {
                const size_t nextSegment = (segment + 1) % SEGMENTS;

                const size_t a = currentRingStart + segment;
                const size_t b = currentRingStart + nextSegment;
                const size_t c = nextRingStart + segment;
                const size_t d = nextRingStart + nextSegment;

                //first triangle
                m_connections.push_back(static_cast<u32>(a));
                m_connections.push_back(static_cast<u32>(b));
                m_connections.push_back(static_cast<u32>(c));
                //second triangle
                m_connections.push_back(static_cast<u32>(b));
                m_connections.push_back(static_cast<u32>(d));
                m_connections.push_back(static_cast<u32>(c));
            }
        }

        //last intermediate ring -> south pole
        const size_t lastRingStart = 1 + (INTERMEDIATE_RINGS - 1) * SEGMENTS;

        for (size_t segment = 0; segment < SEGMENTS; ++segment) {
            const size_t nextSegment = (segment + 1) % SEGMENTS;

            const size_t current = lastRingStart + segment;
            const size_t next = lastRingStart + nextSegment;

            m_connections.push_back(static_cast<u32>(current));
            m_connections.push_back(static_cast<u32>(next));
            m_connections.push_back(static_cast<u32>(southPole));
        }
    }
    break;

    case Style::RenderMode::WIREFRAME: {
        //horizontal edges: one complete loop for every intermediate latitude ring
        const size_t horizontalEdges = INTERMEDIATE_RINGS * SEGMENTS;

        //vertical edges:
        //SEGMENTS lines from north to the first ring, between all
        //neighboring rings, and from the last ring to the south pole
        const size_t verticalEdges = RINGS * SEGMENTS;

        const size_t edgeCount = horizontalEdges + verticalEdges;

        cm.indexStart = cast_u32_safe(m_connections.size());
        cm.indexCount = cast_u32_safe(edgeCount * 2);

        m_connections.reserve(m_connections.size() + edgeCount * 2);

        //horizontal rings
        for (size_t ring = 0; ring < INTERMEDIATE_RINGS; ++ring) {
            const size_t ringStart = 1 + ring * SEGMENTS;

            for (size_t segment = 0; segment < SEGMENTS; ++segment) {
                const size_t nextSegment = (segment + 1) % SEGMENTS;

                m_connections.push_back(static_cast<u32>(ringStart + segment));
                m_connections.push_back(static_cast<u32>(ringStart + nextSegment));
            }
        }

        //vertical edges
        //north pole -> first ring
        for (size_t segment = 0; segment < SEGMENTS; ++segment) {
            m_connections.push_back(0);
            m_connections.push_back(static_cast<u32>(1 + segment));
        }

        //between intermediate rings
        for (size_t ring = 0; ring < INTERMEDIATE_RINGS - 1; ++ring) {
            const size_t currentRingStart = 1 + ring * SEGMENTS;
            const size_t nextRingStart = currentRingStart + SEGMENTS;

            for (size_t segment = 0; segment < SEGMENTS; ++segment) {
                m_connections.push_back(static_cast<u32>(currentRingStart + segment));
                m_connections.push_back(static_cast<u32>(nextRingStart + segment));
            }
        }

        //last ring -> south pole
        const size_t lastRingStart = 1 + (INTERMEDIATE_RINGS - 1) * SEGMENTS;

        for (size_t segment = 0; segment < SEGMENTS; ++segment) {
            m_connections.push_back(static_cast<u32>(lastRingStart + segment));
            m_connections.push_back(static_cast<u32>(southPole));
        }
    }
    break;

    case Style::RenderMode::VERTICES: {
        //no connections are required
        cm.indexStart = std::numeric_limits<u32>::max();
        cm.indexCount = 0;
    }
    break;

    default:
        std::unreachable();
    }

    //add all generated vertices
    m_points.insert(m_points.end(), verts, verts + VERTEX_COUNT);
    //add the command record
    m_records.push_back(cm);
}

void GLGE::Graphic::DebugRenderer::drawSphere(float radius, Object at, World& world, const Style& style) {
    //extract the transform
    Transform transf;

    //highest priority: world transform
    WorldTransform* wt = world.get<WorldTransform>(at);
    if (wt) {
        //found -> just use it
        transf = *wt;
    } else {
        //second highest priority: normal transform
        Transform* tf = world.get<Transform>(at);

        if (tf) {
            //found -> use it
            transf = *tf;
        } else {
            //lowest priority: transform 2D
            Transform2D* tf2d = world.get<Transform2D>(at);

            if (tf2d) {
                //extent to third dimension
                transf.pos = vec3(tf2d->pos, 0);
                transf.rot = Quaternion(vec3(0,0,tf2d->angle));
                transf.scale = vec3(tf2d->scale, 1);
            } else {
                throw GLGE::Exception("Cannot use object without transform as positioner object", "GLGE::Graphic::DebugRenderer::drawBox");
            }
        }
    }

    //just use the function with transform
    drawSphere(radius, transf, style);
}

void GLGE::Graphic::DebugRenderer::drawLine(const vec3& a, const vec3& b, const Style& style_in) {
    //copy the input and set the draw mode to wireframe (only valid draw mode for this operation)
    Style style = style_in;
    style.renderMode = Style::RenderMode::WIREFRAME;

    //create the command record
    CommandRecord cm {};
    cm.style = style;
    cm.indexStart = cast_u32_safe(m_connections.size());
    cm.indexCount = 2;
    cm.pointStart = cast_u32_safe(m_points.size());
    cm.pointCount = 1;

    //add both points
    m_points.reserve(m_points.size() + 2);
    m_points.push_back(a);
    m_points.push_back(b);

    //add the connection indices
    m_connections.reserve(m_connections.size() + 2);
    m_connections.push_back(0);
    m_connections.push_back(1);

    //add the command record
    m_records.push_back(cm);
}

void GLGE::Graphic::DebugRenderer::drawMesh(const GLGE::Mesh& mesh, u8 lodIdx, const Transform& transform, const Style& style) {
    //sanity check the layout
    if (!mesh.getLayout().hasUsage<GLGE::VertexAttribute::Position>())
    {throw GLGE::Exception("Cannot draw a mesh that has no position attribute", "GLGE::Graphic::DebugRenderer::drawMesh");}
    //sanity check the lod
    if (mesh.getLODCount() <= lodIdx)
    {throw GLGE::Exception("Cannot access the requested LOD: LOD index out of range", "GLGE::Graphic::DebugRenderer::drawMesh");}

    //get the LOD
    const auto& lod = mesh.getLOD(lodIdx);

    //extract the vertex positions
    std::vector<vec3> verts;
    verts.reserve(lod.vertices().getCount());
    GLGE::Mesh::VertexAttribute::Type posType = mesh.getLayout().getAttribute<GLGE::VertexAttribute::Position>().type;
    for (const auto& vertex : lod.vertices()) 
    {verts.push_back(__helper_toVec3(vertex.get<void, GLGE::VertexAttribute::Position>(), posType));}

    //apply the transform
    __helper_applyTransform(verts.data(), verts.size(), transform, verts.data());

    //create the command record
    CommandRecord cm {};
    cm.style = style;

    //switch over the style to correctly build the connections
    switch (style.renderMode) {
        case Style::RenderMode::SOLID: {
            //fill the command record
            cm.indexStart = m_connections.size();
            cm.indexCount = (3 * lod.indices().getCount());
            cm.pointStart = cast_u32_safe(m_points.size());
            cm.pointCount = verts.size();

            //store the triangles
            m_connections.reserve(m_connections.size() + cm.indexCount);
            for (const auto& tri : lod.indices()) {
                m_connections.push_back(tri.a);
                m_connections.push_back(tri.b);
                m_connections.push_back(tri.c);
            }
        }
        break;

        case Style::RenderMode::WIREFRAME: {
            cm.indexStart = cast_u32_safe(m_connections.size());
            cm.pointStart = cast_u32_safe(m_points.size());
            cm.pointCount = cast_u32_safe(verts.size());

            //helper structure to store an edge
            struct Edge {
                u32 a;
                u32 b;

                bool operator<(const Edge& other) const noexcept 
                {return a < other.a || (a == other.a && b < other.b);}

                bool operator==(const Edge& other) const noexcept 
                {return a == other.a && b == other.b;}
            };

            std::vector<Edge> edges;
            edges.reserve(static_cast<size_t>(lod.indices().getCount()) * 3);

            for (const auto& tri : lod.indices()) {
                //helper to add undirected edges
                const auto addEdge = [&edges](u32 a, u32 b) {
                    //ignore degenerate edges
                    if (a == b) {return;}
                    //canonical representation of an undirected edge
                    if (a > b) {std::swap(a, b);}
                    edges.push_back({a, b});
                };

                //add edges of triangle
                addEdge(tri.a, tri.b);
                addEdge(tri.b, tri.c);
                addEdge(tri.c, tri.a);
            }

            //put identical/shared edges next to each other
            std::sort(edges.begin(), edges.end());

            //remove shared edges
            edges.erase(std::unique(edges.begin(), edges.end()), edges.end());

            //store index count
            cm.indexCount = cast_u32_safe(edges.size() * 2);

            //add connections
            m_connections.reserve(m_connections.size() + cm.indexCount);
            for (const Edge& edge : edges) {
                m_connections.push_back(edge.a);
                m_connections.push_back(edge.b);
            }
        }
        break;

        case Style::RenderMode::VERTICES: {
            //fill the command record
            cm.indexStart = std::numeric_limits<u32>::max();
            cm.indexCount = 0;
            cm.pointStart = cast_u32_safe(m_points.size());
            cm.pointCount = verts.size();
        }
        break;

        default: std::unreachable();
    }

    //in any case add the vertices
    m_points.insert(m_points.end(), verts.begin(), verts.end());
    //add the command record
    m_records.push_back(cm);
}

void GLGE::Graphic::DebugRenderer::drawMesh(const GLGE::Mesh& mesh, u8 lod, Object at, World& world, const Style& style) {
    //extract the transform
    Transform transf;

    //highest priority: world transform
    WorldTransform* wt = world.get<WorldTransform>(at);
    if (wt) {
        //found -> just use it
        transf = *wt;
    } else {
        //second highest priority: normal transform
        Transform* tf = world.get<Transform>(at);

        if (tf) {
            //found -> use it
            transf = *tf;
        } else {
            //lowest priority: transform 2D
            Transform2D* tf2d = world.get<Transform2D>(at);

            if (tf2d) {
                //extent to third dimension
                transf.pos = vec3(tf2d->pos, 0);
                transf.rot = Quaternion(vec3(0,0,tf2d->angle));
                transf.scale = vec3(tf2d->scale, 1);
            } else {
                throw GLGE::Exception("Cannot use object without transform as positioner object", "GLGE::Graphic::DebugRenderer::drawBox");
            }
        }
    }

    //just use the function with transform
    drawMesh(mesh, lod, transf, style);
}