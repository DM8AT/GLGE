/**
 * @file RayCasting.h
 * @author DM8AT
 * @brief define all structures to enable casting a ray into a scene
 * @version 0.1
 * @date 2026-08-25
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_RAY_CASTING_
#define _GLGE_RAY_CASTING_

//add meshes
#include "Mesh.h"
//add objects
#include "Object.h"

//use the library namespace
namespace GLGE {

    /**
     * @brief a component to attach meshes to objects
     */
    struct MeshComponent {
        /**
         * @brief store a pointer to the mesh that belongs to the object
         */
        GLGE::Mesh* mesh = nullptr;
    };

    /**
     * @brief define a ray
     */
    struct Ray {
        /**
         * @brief store the origin of the ray
         */
        vec3 origin;
        /**
         * @brief store the direction of the ray
         */
        vec3 direction;
        /**
         * @brief define the minimum distance at which the ray can register hits
         */
        f32 tMin = 0.f;
        /**
         * @brief define the maximum distance at which the ray can register hits
         */
        f32 tMax = std::numeric_limits<f32>::infinity();
    };

    /**
     * @brief define a structure to store a hit
     */
    struct Hit {
        /**
         * @brief define the distance from the ray origin that the hit occurred
         */
        float distance = 0.f;
        /**
         * @brief define the world-space position the hit occurred in
         */
        vec3 position;
        /**
         * @brief store which object was hit
         */
        GLGE::Object obj;
        /**
         * @brief store a pointer to the hit mesh
         */
        GLGE::Mesh* mesh = nullptr;
        /**
         * @brief store the index of the LOD that was hit
         */
        u32 lod;
        /**
         * @brief store the triangle that was hit
         */
        Triangle triangle;
        /**
         * @brief store the barycentric coordinates in the hit triangle
         * 
         * This allows for attribute interpolation of the vertices elements
         */
        vec3 barycentric;
    };

    /**
     * @brief use the system namespace for systems
     */
    namespace System {

        /**
         * @brief cast a ray through the world
         * 
         * @param world the world to cast the ray through
         * @param ray the ray to cast
         * @return `std::optional<Hit>` if a mesh was hit, a filled hit structure is returned. If nothing was hit, the optional is empty. 
         */
        std::optional<Hit> CastRay(GLGE::World& world, const Ray& ray);

    }

}

#endif