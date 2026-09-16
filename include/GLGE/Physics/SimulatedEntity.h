/**
 * @file SimulatedEntity.h
 * @author DM8AT
 * @brief define a class that is responsible for holding the state of a physics actor
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_PHYSICS_SIMULATED_ENTITY_
#define _GLGE_PHYSICS_SIMULATED_ENTITY_

//add the simulated entity backend
#include "Backend/SimulatedEntity.h"
//add colliders
#include "Collider.h"

//use the physics namespace
namespace GLGE::Physic {

    /**
     * @brief define a class that defines the physics properties of an entity
     * 
     * This is wrapped in a rigid body. It can be used in multiple worlds simultaneously. This simulated entity
     * may only be referenced by ONE entity per world. 
     */
    class SimulatedEntity {
    public:

        /**
         * @brief store the filter mask type
         */
        using Filter = Backend::SimulatedEntity::Filter;

        /**
         * @brief store the types of the body
         * 
         */
        using BodyType = Backend::SimulatedEntity::BodyType;

        /**
         * @brief Construct a new Simulated Entity
         * 
         * Initially invalid
         */
        SimulatedEntity() = default;

        /**
         * @brief Destroy the Simulated Entity
         */
        ~SimulatedEntity() = default;

        /**
         * @brief Get the Transform
         * 
         * @return `const WorldTransform&` a constant reference to the stored transform
         */
        inline const WorldTransform& getTransform() const noexcept
        {return m_entity->getTransform();}

        /**
         * @brief Set the Collider
         * 
         * @param collider a reference to the backend collider to set to
         */
        inline void setCollider(Physic::Collider& collider) const
        {m_entity->setCollider(collider.getBackend());}
        /**
         * @brief Get the Collider
         * 
         * @return `Collider` the collider
         */
        inline Collider getCollider() const noexcept
        {return Collider(m_entity->getCollider());}

        /**
         * @brief Set the Mass
         * 
         * The mass must be positive. If `mass < 0.f` is true, the absolute mass will be used. 
         * 
         * @param mass the new mass of the entity
         */
        inline void setMass(float mass) const
        {m_entity->setMass(mass);}
        /**
         * @brief Get the Mass
         * 
         * @return `float` the mass of the entity
         */
        inline float getMass() const noexcept
        {return m_entity->getMass();}

        /**
         * @brief Set the Friction
         * 
         * The friction parameter is constrained to a range form 0 to 1. Everything outside the range is clamped to fit into it. 
         * 
         * @param friction the friction of the object
         */
        inline void setFriction(float friction) const
        {m_entity->setFriction(friction);}
        /**
         * @brief Get the Friction
         * 
         * @return `float` the friction of the object
         */
        inline float getFriction() const noexcept
        {return m_entity->getFriction();}

        /**
         * @brief Set the Restitution
         * 
         * The restitution parameter is constrained to a range from 0 to 1. Everything outside the range is clamped to fit into it. 
         * 
         * @param restitution the restitution of the object
         */
        inline void setRestitution(float restitution) const
        {m_entity->setRestitution(restitution);}
        /**
         * @brief Get the Restitution
         * 
         * @return `float` the restitution of the object
         */
        inline float getRestitution() const noexcept
        {return m_entity->getRestitution();}

        /**
         * @brief Set the Filter
         * 
         * @param filter the new filter state of the object
         */
        inline void setFilter(const Filter& filter) const
        {m_entity->setFilter(filter);}
        /**
         * @brief Get the Filter
         * 
         * @return `const Filter&` a constant reference to the filter state of the object
         */
        inline const Filter& getFilter() const noexcept
        {return m_entity->getFilter();}

        /**
         * @brief Set the Body Type
         * 
         * @param type the body type for the object
         */
        inline void setBodyType(BodyType type) const
        {m_entity->setBodyType(type);}
        /**
         * @brief Get the Body Type
         * 
         * @return `BodyType` the body type of the object
         */
        inline BodyType getBodyType() const noexcept
        {return m_entity->getBodyType();}

        /**
         * @brief Set the User Data
         * 
         * @param userData the new user data pointer
         */
        void setUserData(void* userData) const
        {m_entity->setUserData(userData);}
        /**
         * @brief Get the User Data
         * 
         * @return `void*` the user data pointer
         */
        inline void* getUserData() const noexcept
        {return m_entity->getUserData();}

        /**
         * @brief Set the Linear Velocity
         * 
         * @param velocity the new linear velocity
         */
        inline void setLinearVelocity(const vec3& velocity) const
        {m_entity->setLinearVelocity(velocity);}
        /**
         * @brief Get the Linear Velocity
         * 
         * @return `vec3` the current linear velocity
         */
        inline vec3 getLinearVelocity() const 
        {return m_entity->getLinearVelocity();}

        /**
         * @brief Set the Angular Velocity
         * 
         * @param velocity the angular velocity
         */
        inline void setAngularVelocity(const vec3& velocity) const
        {m_entity->setAngularVelocity(velocity);}
        /**
         * @brief Get the Angular Velocity
         * 
         * @return `vec3` the current angular velocity
         */
        inline vec3 getAngularVelocity() const 
        {return m_entity->getAngularVelocity();}

        /**
         * @brief apply a force to the object
         * 
         * Forces are applied at the object's center of mass. 
         * 
         * If the body type is static, this does nothing. 
         * 
         * @param force the force vector to apply to the object
         */
        inline void applyForce(const vec3& force) const
        {m_entity->applyForce(force);}
        /**
         * @brief apply an impulse to the object
         * 
         * The impulse is applied at the object's center of mass. 
         * 
         * If the body type is static, this does nothing. 
         * 
         * @param impulse the impulse to apply
         */
        inline void applyImpulse(const vec3& impulse) const
        {m_entity->applyImpulse(impulse);}
        /**
         * @brief apply torque to the object
         * 
         * Torque is applied to the object's center of mass. 
         * 
         * If the body type is static, this does nothing. 
         * 
         * @param torque the torque vector to apply
         */
        inline void applyTorque(const vec3& torque) const
        {m_entity->applyTorque(torque);}

        /**
         * @brief check if the object is sleeping
         * 
         * A static entity is always considered awake. 
         * 
         * @return `true` if the object is sleeping, `false` if not
         */
        inline bool isSleeping() const 
        {return m_entity->isSleeping();}
        /**
         * @brief wake the object up
         * 
         * If the body type is static, this does nothing. 
         */
        inline void wakeUp() const
        {m_entity->wakeUp();}
        /**
         * @brief make the object sleep
         * 
         * If the body type is static, this does nothing. 
         */
        inline void sleep() const
        {m_entity->sleep();}

        /**
         * @brief Get the Backend
         * 
         * @return `const Reference<Backend::SimulatedEntity>&` the backend
         */
        inline const Reference<Backend::SimulatedEntity>& getBackend() const noexcept
        {return m_entity;}

        /**
         * @brief check if the simulated entity is valid
         * 
         * @return `true` if this is a valid simulated entity, `false` if not
         */
        inline bool isValid() const noexcept
        {return m_entity.get() != nullptr;}

        /**
         * @brief check if the simulated entity is valid
         * 
         * @return `true` if this is a valid simulated entity, `false` if not
         */
        inline operator bool() const noexcept
        {return isValid();}

    protected:

        //friends are worlds
        friend class World;

        /**
         * @brief define the wrapped simulated entity
         */
        Reference<Backend::SimulatedEntity> m_entity;

    };

}

#endif