/**
 * @file Collider.h
 * @author DM8AT
 * @brief define the frontend collider
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_PHYSICS_COLLIDER_
#define _GLGE_PHYSICS_COLLIDER_

//add the collider backend
#include "Backend/Collider.h"
//add base classes
#include "Core/BaseClass.h"
//add instances
#include "Instance.h"

//use the physics namespace
namespace GLGE::Physic {

    /**
     * @brief a class that can store ANY collider type
     */
    class Collider : public BaseClass {
    public:

        /**
         * @brief define the type of collider
         */
        using Type = Backend::Collider::Type;

        /**
         * @brief Construct a new Collider
         * 
         * @param collider the backend to wrap
         */
        Collider(const Reference<Backend::Collider>& collider)
         : m_collider(collider)
        {}

        /**
         * @brief Construct a new Collider
         * 
         * The Type will be a sphere collider
         * 
         * @param radius the radius for the collider
         */
        Collider(float radius)
         : BaseClass(), m_collider(getInstance()->getExtension<GLGE::Physic::Instance>()->getDescription()->createSphereCollider(radius).get())
        {}

        /**
         * @brief Construct a new Collider
         * 
         * The Type will be a box collider
         * 
         * @param extent the extent of the box collider
         */
        Collider(const vec3& extent)
         : BaseClass(), m_collider(getInstance()->getExtension<GLGE::Physic::Instance>()->getDescription()->createBoxCollider(extent * 0.5f).get())
        {}

        /**
         * @brief Get the Type
         * 
         * @return `Type` the type of the wrapped collider
         */
        inline Type getType() const noexcept
        {return m_collider->getType();}

        /**
         * @brief Get the Backend
         * 
         * @return `Reference<Backend::Collider>` a reference to the backend
         */
        inline Reference<Backend::Collider> getBackend() const noexcept
        {return m_collider;}

    protected:

        /**
         * @brief store the abstract collider
         */
        Reference<Backend::Collider> m_collider;

    };

    /**
     * @brief define a class for a sphere collider
     */
    class SphereCollider : public Collider {
    public:

        /**
         * @brief Construct a new Sphere Collider
         * 
         * @param radius the radius for the sphere collider
         */
        SphereCollider(float radius)
         : Collider(radius)
        {}

        /**
         * @brief Construct a new Sphere Collider
         * 
         * Upcast from a collider
         * 
         * @param collider the collider to create from
         * 
         * @throws `GLGE::Exception` if the type of the collider is not `Type::SPHERE`. 
         */
        SphereCollider(const Collider& collider)
         : Collider(collider)
        {if (getType() != Type::SPHERE) {throw GLGE::Exception("Invalid collider type for a sphere collider", "SphereCollider::SphereCollider");}}
    
    };

    /**
     * @brief define a class for a box collider
     */
    class BoxCollider : public Collider {
    public:

        /**
         * @brief Construct a new Box Collider
         * 
         * @param extent the extent for the box collider
         */
        BoxCollider(const vec3& extent)
         : Collider(extent)
        {}

        /**
         * @brief Construct a new Sphere Collider
         * 
         * Upcast from a collider
         * 
         * @param collider the collider to create from
         * 
         * @throws `GLGE::Exception` if the type of the collider is not `Type::BOX`. 
         */
        BoxCollider(const Collider& collider)
         : Collider(collider)
        {if (getType() != Type::BOX) {throw GLGE::Exception("Invalid collider type for a box collider", "SphereCollider::SphereCollider");}}
    
    };

}

#endif