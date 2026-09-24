/**
 * @file DebugRenderer.h
 * @author DM8AT
 * @brief define a default implementation for a debug draw data provider
 * @version 0.1
 * @date 2026-09-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_DEBUG_RENDERER_

//add the debug draw data provider
#include "DebugDrawDataProvider.h"

//add transforms
#include "GLGE/Core/Transform.h"
//add the ECS
#include "GLGE/Core/Object.h"
//add core meshes
#include "GLGE/Core/Mesh.h"

//add core AABBs
#include "GLGE/Core/AABB.h"

namespace GLGE::Graphic {


    /**
     * @brief Define a structure that is responsible to record debug render commands
     * 
     * This makes multi-submission to different targets easy. 
     * 
     * @note This does NO GPU-Work
     */
    class DebugRenderer : public DebugDrawDataProvider {
    public:

        /**
         * @brief Construct a new Debug Renderer
         */
        DebugRenderer() = default;

        /**
         * @brief Destroy the Debug Renderer
         */
        ~DebugRenderer() = default;

        /**
         * @brief clear the internal draw command list
         * 
         * This does NOT effect any submitted work to debug renderers, even if they did not finish recording
         */
        void reset();

        /**
         * @brief render an axis-aligned bounding box (AABB)
         * 
         * @param aabb the axis aligned bounding box to draw
         * @param transform the transformation of the AABB
         * @param style the style to draw it in
         */
        void drawAABB(const AABB& aabb, const Transform& transform, const Style& style);

        /**
         * @brief render an axis-aligned bounding box (AABB)
         * 
         * @param aabb the axis aligned bounding box to draw
         * @param at an object to draw it at (requires either `Transform`, `Transform2D` or `WorldTransform` component)
         * @param world a reference to the world in which the object lives
         * @param style the style to draw it in
         */
        void drawAABB(const AABB& aabb, Object at, World& world, const Style& style);

        /**
         * @brief draw a box
         * 
         * In box-local space 0,0,0 is the front bottom left corner of the box. 
         * In box-local space the box will span from 0,0,0 to `extent`. 
         * 
         * @param extent the extent of the box to draw
         * @param transform the transform to draw the box at
         * @param style the style to draw it in
         */
        void drawBox(const vec3& extent, const Transform& transform, const Style& style);

        /**
         * @brief draw a box
         * 
         * In box-local space 0,0,0 is the front bottom left corner of the box. 
         * In box-local space the box will span from 0,0,0 to `extent`. 
         * 
         * @param extent the extent of the box to draw
         * @param at an object to draw it at (requires either `Transform`, `Transform2D` or `WorldTransform` component)
         * @param world a reference to the world in which the object lives
         * @param style the style to draw it in
         */
        void drawBox(const vec3& extent, Object at, World& world, const Style& style);

        /**
         * @brief draw a single dot
         * 
         * This implicitly sets the draw style to `VERTICES`. 
         * 
         * @param position the position to draw the dot at
         * @param style the style to draw in
         */
        void drawDot(const vec3& position, const Style& style);

        /**
         * @brief draw a sphere
         * 
         * This creates a sphere mesh on the fly. The created sphere mesh is a UV-Sphere. 
         * 
         * @param radius the radius of the sphere
         * @param transform the transform for the sphere
         * @param style the style to draw in
         */
        void drawSphere(float radius, const Transform& transform, const Style& style);

        /**
         * @brief draw a sphere
         * 
         * This creates a sphere mesh on the fly. The created sphere mesh is a UV-Sphere. 
         * 
         * @param radius the radius of the sphere
         * @param at an object to draw it at (requires either `Transform`, `Transform2D` or `WorldTransform` component)
         * @param world a reference to the world in which the object lives
         * @param style the style to draw in
         */
        void drawSphere(float radius, Object at, World& world, const Style& style);

        /**
         * @brief draw a line
         * 
         * This implicitly sets the draw style to `WIREFRAME`
         * 
         * @param a the first end point of the line
         * @param b the second end point of the line
         * @param style the style to draw in
         */
        void drawLine(const vec3& a, const vec3& b, const Style& style);

        /**
         * @brief draw a mesh
         * 
         * @note in contrast to the main renderer render pipeline, this does NO persistend GPU VBO / VAO upload. 
         * @note this only uses the position argument of the vertex layout. 
         * @warning this requires the mesh vertex layout to contain a position argument using the default `GLGE::VertexAttribute::Position` type. 
         * 
         * @param mesh the CPU mesh to use
         * @param lod the level of detail index of the mesh
         * @param transform the transformation to apply to the mesh
         * @param style the style to draw in
         */
        void drawMesh(const GLGE::Mesh& mesh, u8 lod, const Transform& transform, const Style& style);

        /**
         * @brief draw a mesh
         * 
         * @note in contrast to the main renderer render pipeline, this does NO persistend GPU VBO / VAO upload. 
         * @note this only uses the position argument of the vertex layout. 
         * @warning this requires the mesh vertex layout to contain a position argument using the default `GLGE::VertexAttribute::Position` type. 
         * 
         * @param mesh the CPU mesh to use
         * @param lod the level of detail index of the mesh
         * @param at an object to draw it at (requires either `Transform`, `Transform2D` or `WorldTransform` component)
         * @param world a reference to the world in which the object lives
         * @param style the style to draw in
         */
        void drawMesh(const GLGE::Mesh& mesh, u8 lod, Object at, World& world, const Style& style);

    };

}

#endif