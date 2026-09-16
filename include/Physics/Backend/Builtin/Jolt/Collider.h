/**
 * @file Collider.h
 * @author DM8AT
 * @brief define the collider overloads for Jolt
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_PHYSICS_BACKEND_JOLT_COLLIDER_
#define _GLGE_PHYSICS_BACKEND_JOLT_COLLIDER_

//add collider
#include "Physics/Backend/Collider.h"

//add jolt
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

//use the backend namespace
namespace GLGE::Physic::Backend::Jolt {

    /**
     * @brief define the sphere collider overload
     */
    class SphereCollider : public Backend::SphereCollider {
    public:

        /**
         * @brief Construct a new Sphere Collider
         * 
         * @param radius the radius of the sphere collider
         */
        SphereCollider(float radius);

        /**
         * @brief Destroy the Sphere Collider
         */
        virtual ~SphereCollider() override {}

        /**
         * @brief Get the Sphere
         * 
         * @return `const JPH::Ref<JPH::SphereShape>&` a reference to the sphere
         */
        inline const JPH::Ref<JPH::SphereShape>& getShape() noexcept
        {return m_sphere;}

    protected:

        /**
         * @brief store the sphere
         */
        JPH::Ref<JPH::SphereShape> m_sphere;

    };

    /**
     * @brief store what a box collider is
     */
    class BoxCollider : public Backend::BoxCollider {
    public:

        /**
         * @brief Construct a new Box Collider
         * 
         * @param halfExtent the half extent
         */
        BoxCollider(const vec3& halfExtent);

        /**
         * @brief Get the Shape
         * 
         * @return `const JPH::Ref<JPH::BoxShape>&` a reference to the box
         */
        inline const JPH::Ref<JPH::BoxShape>& getShape() noexcept
        {return m_box;}

    protected:

        /**
         * @brief store the box shape
         */
        JPH::Ref<JPH::BoxShape> m_box;

    };

}

#endif