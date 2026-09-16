/**
 * @file SimulatedEntity.h
 * @author DM8AT
 * @brief define a class that is responsible for holding the state of a physics actor, but the backend implementation
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_PHYSICS_BACKEND_JOLT_SIMULATED_ENTITY_
#define _GLGE_PHYSICS_BACKEND_JOLT_SIMULATED_ENTITY_

//add simulated entities
#include "Physics/Backend/SimulatedEntity.h"

//add jolt
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>

//use the physics backend namespace
namespace GLGE::Physic::Backend::Jolt {

    /**
     * @brief define the backend abstraction 
     */
    class SimulatedEntity : public Backend::SimulatedEntity {
    public:

        /**
         * @brief Construct a new Simulated Entity
         * 
         * @param id the ID of the body
         * @param world a pointer to the world that owns the simulated entity
         * @param collider the collider the simulated entity uses
         * @param type the type of simulated entity this is
         * @param mass the mass of the entity
         * @param friction the friction the entity experiences
         * @param restitution the restitution of the entity
         * @param highQualityCollision `true` to use high quality collisions, `false` to use the default collisions
         */
        SimulatedEntity(JPH::BodyID id, Backend::World* world, const Reference<Backend::Collider>& collider, BodyType type, float mass, float friction, float restitution, bool highQualityCollision)
         : Backend::SimulatedEntity(world, collider, type, mass, friction, restitution, highQualityCollision), m_body(id)
        {}

        /**
         * @brief Destroy the Simulated Entity
         */
        virtual ~SimulatedEntity() override {}

        /**
         * @brief Set the Linear Velocity
         * 
         * @param velocity the new linear velocity
         */
        virtual void setLinearVelocity(const vec3& velocity) override;
        /**
         * @brief Get the Linear Velocity
         * 
         * @return `vec3` the current linear velocity
         */
        virtual vec3 getLinearVelocity() const override;

        /**
         * @brief Set the Angular Velocity
         * 
         * @param velocity the angular velocity
         */
        virtual void setAngularVelocity(const vec3& velocity) override;
        /**
         * @brief Get the Angular Velocity
         * 
         * @return `vec3` the current angular velocity
         */
        virtual vec3 getAngularVelocity() const override;

        /**
         * @brief apply a force to the object
         * 
         * Forces are applied at the object's center of mass. 
         * 
         * If the body type is static, this does nothing. 
         * 
         * @param force the force vector to apply to the object
         */
        virtual void applyForce(const vec3& force) override;
        /**
         * @brief apply an impulse to the object
         * 
         * The impulse is applied at the object's center of mass. 
         * 
         * If the body type is static, this does nothing. 
         * 
         * @param impulse the impulse to apply
         */
        virtual void applyImpulse(const vec3& impulse) override;
        /**
         * @brief apply torque to the object
         * 
         * Torque is applied to the object's center of mass. 
         * 
         * If the body type is static, this does nothing. 
         * 
         * @param torque the torque vector to apply
         */
        virtual void applyTorque(const vec3& torque) override;

        /**
         * @brief check if the object is sleeping
         * 
         * A static entity is always considered awake. 
         * 
         * @return `true` if the object is sleeping, `false` if not
         */
        virtual bool isSleeping() const override;
        /**
         * @brief wake the object up
         * 
         * If the body type is static, this does nothing. 
         */
        virtual void wakeUp() override;
        /**
         * @brief make the object sleep
         * 
         * If the body type is static, this does nothing. 
         */
        virtual void sleep() override;

        /**
         * @brief Get the Body
         * 
         * @return `JPH::BodyID` the body ID
         */
        inline JPH::BodyID getBody() const noexcept
        {return m_body;}

        /**
         * @brief Set the Body
         * 
         * @param body the body
         */
        void setBody(JPH::BodyID body) noexcept
        {m_body = body;}

    protected:

        //world is friend
        friend class World;

        /**
         * @brief a function called by the world on an update to clear dirty states
         */
        virtual void flushState() override;

        /**
         * @brief store the jolt body ID
         */
        JPH::BodyID m_body;
        /**
         * @brief store if this entity was ticked
         */
        bool m_hadTick = false;

    };

}

#endif