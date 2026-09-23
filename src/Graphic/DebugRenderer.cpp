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

void GLGE::Graphic::DebugRenderer::drawAABB(const AABB& aabb, const vec3& pos, const Style& style) {
    //transform the AABB to world
    AABB worldAABB(aabb.getMin() + pos, aabb.getMax() + pos);

    //depending on the render mode there are 3 totally different visualizations possible
    switch (style.renderMode)
    {
    case Style::RenderMode::SOLID: {
            //solid adds all 8 points and creates 12 triangles from them
            //this can be done with only 8 points since the points have no face-specific information

            //create the command record
            CommandRecord cm {};
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

            //create the command record
            CommandRecord cm {};
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

            //create the command record
            CommandRecord cm {};
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
    m_points.reserve(m_points.size() + 8);
    m_points.push_back(vec3 {worldAABB.getMin().x, worldAABB.getMin().y, worldAABB.getMin().z});
    m_points.push_back(vec3 {worldAABB.getMin().x, worldAABB.getMin().y, worldAABB.getMax().z});
    m_points.push_back(vec3 {worldAABB.getMin().x, worldAABB.getMax().y, worldAABB.getMin().z});
    m_points.push_back(vec3 {worldAABB.getMin().x, worldAABB.getMax().y, worldAABB.getMax().z});
    m_points.push_back(vec3 {worldAABB.getMax().x, worldAABB.getMin().y, worldAABB.getMin().z});
    m_points.push_back(vec3 {worldAABB.getMax().x, worldAABB.getMin().y, worldAABB.getMax().z});
    m_points.push_back(vec3 {worldAABB.getMax().x, worldAABB.getMax().y, worldAABB.getMin().z});
    m_points.push_back(vec3 {worldAABB.getMax().x, worldAABB.getMax().y, worldAABB.getMax().z});
}

void GLGE::Graphic::DebugRenderer::drawBox(const Transform& transform, const vec3& extent, const Style& style) {
    //regardless of render mode there need to be the the 8 vertices of the box. For that, create them in this scope and transform them in this scope
    //the box is created as centered around (0,0,0) and with the specified extent. 
    vec3 half = extent / vec3 {2.f};
    vec3 verts[] = {
        vec3 {-half.x, -half.y, -half.z},
        vec3 {-half.x, -half.y,  half.z},
        vec3 {-half.x,  half.y, -half.z},
        vec3 {-half.x,  half.y,  half.z},
        vec3 { half.x, -half.y, -half.z},
        vec3 { half.x, -half.y,  half.z},
        vec3 { half.x,  half.y, -half.z},
        vec3 { half.x,  half.y,  half.z}
    };

    //create the transformation matrix from the transform
    glm::mat4 transf = glm::translate(glm::mat4(transform.rot) * glm::scale(glm::mat4 {}, transform.scale), transform.pos);

    //apply the transformation
    for (size_t i = 0; i < (sizeof(verts) / sizeof(*verts)); ++i) {
        vec4 inter = transf * vec4(verts[i], 1);
        verts[i] = vec3{inter.x, inter.y, inter.z};
    }

    //now, depending on the render mode, different connections of the points are required
    switch (style.renderMode) {
    case Style::RenderMode::SOLID: {

        }
        break;
    case Style::RenderMode::WIREFRAME: {

        }
        break;
    case Style::RenderMode::VERTICES: {

        }
        break;
    
    default:
        //invalid
        std::unreachable();
        break;
    }

    //in any valid case add all 8 vertices
    m_points.reserve(m_points.size() + 8);
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
}

void GLGE::Graphic::DebugRenderer::drawSphere(const vec3& position, float radius, const Style& style) {

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
}

void GLGE::Graphic::DebugRenderer::drawMesh(const GLGE::Mesh& mesh, u8 lod, const Transform& transform, const Style& style) {

}