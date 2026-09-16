/**
 * @file Collider.h
 * @author DM8AT
 * @brief define the backend of a collider
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_PHYSICS_BACKEND_COLLIDER_
#define _GLGE_PHYSICS_BACKEND_COLLIDER_

//add reference counting
#include "Core/Reference.h"

//use the backend namespace
namespace GLGE::Physic::Backend {

    /**
     * @brief store the collider abstraction
     * 
     * A collider is NOT mutable after creation. 
     */
    class Collider : public Referable {
    public:

        /**
         * @brief store the collider type
         */
        enum class Type : u32 {
            /**
             * @brief store a sphere collider
             */
            SPHERE = 0,
            /**
             * @brief store a box collider
             */
            BOX = 1,
            /**
             * @brief store a capsule collider
             */
            CAPSULE = 2,
            /**
             * @brief store a full mesh collider
             */
            MESH = 3
        };

        /**
         * @brief Construct a new Collider
         * 
         * @param type the used type
         */
        Collider(Type type) : m_type(type) {}

        /**
         * @brief Destroy the Collider
         */
        virtual ~Collider() {}

        /**
         * @brief Get the Type of collider
         * 
         * @return `Type` the type of collider this instance is
         */
        inline Type getType() const noexcept
        {return m_type;}

    private:

        /**
         * @brief store the type
         */
        Type m_type;

    };

    /**
     * @brief define what a sphere collider is
     */
    class SphereCollider : public Collider {
    public:

        /**
         * @brief Construct a new Sphere Collider
         * 
         * @param radius the radius of the sphere, must be positive
         */
        SphereCollider(float radius)
         : Collider(Type::SPHERE), m_radius(glm::abs(radius))
        {}

        /**
         * @brief Destroy the Sphere Collider
         * 
         */
        virtual ~SphereCollider() override {}

        /**
         * @brief Get the Radius
         * 
         * @return `float` the radius
         */
        inline float getRadius() const noexcept
        {return m_radius;}

    protected:

        /**
         * @brief store the radius
         */
        float m_radius = 1.f;

    };

    /**
     * @brief store what a box collider is
     */
    class BoxCollider : public Collider {
    public:

        /**
         * @brief Construct a new Box Collider
         * 
         * @param halfExtent the half extent
         */
        BoxCollider(const vec3& halfExtent)
         : Collider(Type::BOX), m_halfExtent(halfExtent)
        {}

        /**
         * @brief Get the Half Extent
         * 
         * @return `const vec3&` the half extent
         */
        inline const vec3& getHalfExtent() const noexcept
        {return m_halfExtent;}

    protected:

        /**
         * @brief store the half extent
         */
        vec3 m_halfExtent;

    };

}

#endif