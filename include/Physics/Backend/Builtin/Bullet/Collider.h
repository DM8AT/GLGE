/**
 * @file Collider.h
 * @author DM8AT
 * @brief define the collider classes for bullet
 * @version 0.1
 * @date 2026-06-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_PHYSICS_BACKEND_BULLET_COLLIDER_
#define _GLGE_PHYSICS_BACKEND_BULLET_COLLIDER_

//add the collider glue layer
#include "Physics/Backend/Collider.h"
//add bullet
#include <btBulletDynamicsCommon.h>
//add smart pointers (here ok)
#include <memory>

//use the backend namespace
namespace GLGE::Physic::Backend::Bullet {

    /**
     * @brief define the sphere collider backend
     */
    class SphereCollider : public Backend::SphereCollider {
    public:

        /**
         * @brief Construct a new Sphere Collider
         * 
         * @param radius the radius for the sphere
         */
        SphereCollider(float radius)
         : Backend::SphereCollider(radius),
           m_shape(std::make_unique<btSphereShape>(radius))
        {
            //reduce collision margin
            m_shape->setMargin(0.001f);
        }

        /**
         * @brief Get the Shape
         * 
         * @return `btCollisionShape*` a pointer to the shape
         */
        inline btCollisionShape* getShape() const noexcept
        {return m_shape.get();}

    protected:

        /**
         * @brief store the collision shape
         */
        std::unique_ptr<btSphereShape> m_shape;

    };

    /**
     * @brief define the box collider backend
     */
    class BoxCollider : public Backend::BoxCollider {
    public:

        /**
         * @brief Construct a new Box Collider
         * 
         * @param halfExtent the half extent
         */
        BoxCollider(const vec3& halfExtent)
         : Backend::BoxCollider(halfExtent),
           m_shape(std::make_unique<btBoxShape>(btVector3(halfExtent.x, halfExtent.y, halfExtent.z)))
        {
            //reduce collision margin
            m_shape->setMargin(0.001f);
        }

        /**
         * @brief Get the Shape
         * 
         * @return `btCollisionShape*` a pointer to the shape
         */
        inline btCollisionShape* getShape() const noexcept
        {return m_shape.get();}

    protected:

        /**
         * @brief store the shape
         */
        std::unique_ptr<btBoxShape> m_shape;

    };

}

#endif