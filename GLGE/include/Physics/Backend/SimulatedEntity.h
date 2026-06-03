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
#ifndef _GLGE_PHYSICS_BACKEND_SIMULATED_ENTITY_
#define _GLGE_PHYSICS_BACKEND_SIMULATED_ENTITY_

//add transforms
#include "Core/Transform.h"
//add colliders
#include "Collider.h"

//use the physics backend namespace
namespace GLGE::Physic::Backend {

    //worlds are defined somewhere else
    class World;

    /**
     * @brief define the backend abstraction 
     */
    class SimulatedEntity : public Referable {
    public:

        /**
         * @brief define object filtering
         * 
         * @warning TODO
         */
        struct Filter {
            /**
             * @brief store the layers other objects can collide with the object with
             */
            u64 layers = ~0ULL;
            /**
             * @brief store the layers of objects this object can collide with
             */
            u64 mask = ~0ULL;
        };

        /**
         * @brief define the type of a body
         */
        enum class BodyType {
            /**
             * @brief the body is static
             */
            STATIC,
            /**
             * @brief the body is kinematic
             */
            KINEMATIC,
            /**
             * @brief the body is fully dynamic
             */
            DYNAMIC
        };

        /**
         * @brief Construct a new Simulated Entity
         * 
         * @param world a pointer to the world that owns the simulated entity
         * @param collider the collider the simulated entity uses
         * @param type the type of simulated entity this is
         * @param mass the mass of the entity
         * @param friction the friction the entity experiences
         * @param restitution the restitution of the entity
         * @param highQualityCollision `true` to use high quality collisions, `false` to use the default collisions
         */
        SimulatedEntity(World* world, const Reference<Collider>& collider, BodyType type, float mass, float friction, float restitution, bool highQualityCollision)
         : m_world(world), m_collider(collider), m_type(type), m_mass(glm::abs(mass)), m_friction(glm::clamp(friction, 0.f, 1.f)), m_restitution(glm::clamp(restitution, 0.f, 1.f)),
           m_highQualityCollision(highQualityCollision)
        {}

        //object cannot be copied nor moved
        SimulatedEntity(const SimulatedEntity&) = delete;
        SimulatedEntity& operator=(const SimulatedEntity&) = delete;
        SimulatedEntity(SimulatedEntity&&) = delete;
        SimulatedEntity& operator=(SimulatedEntity&&) = delete;

        /**
         * @brief Destroy the Simulated Entity
         */
        virtual ~SimulatedEntity()
        {}

        /**
         * @brief Get the Transform
         * 
         * @return `const WorldTransform&` a constant reference to the stored transform
         */
        inline const WorldTransform& getTransform() const noexcept
        {return m_transf;}

        /**
         * @brief Set the Collider
         * 
         * @param collider a reference to the backend collider to set to
         */
        void setCollider(const Reference<Collider>& collider);
        /**
         * @brief Get the Collider
         * 
         * @return `Reference<Collider>` a reference to the collider
         */
        inline Reference<Collider> getCollider() const noexcept
        {return m_collider;}

        /**
         * @brief Get the World
         * 
         * @return `World*` a pointer to the world that owns the simulated entity
         */
        inline World* getWorld() const noexcept
        {return m_world;}

        /**
         * @brief Set the Mass
         * 
         * The mass must be positive. If `mass < 0.f` is true, the absolute mass will be used. 
         * 
         * @param mass the new mass of the entity
         */
        void setMass(float mass);
        /**
         * @brief Get the Mass
         * 
         * @return `float` the mass of the entity
         */
        inline float getMass() const noexcept
        {return m_mass;}

        /**
         * @brief Set the Friction
         * 
         * The friction parameter is constrained to a range form 0 to 1. Everything outside the range is clamped to fit into it. 
         * 
         * @param friction the friction of the object
         */
        void setFriction(float friction);
        /**
         * @brief Get the Friction
         * 
         * @return `float` the friction of the object
         */
        inline float getFriction() const noexcept
        {return m_friction;}

        /**
         * @brief Set the Restitution
         * 
         * The restitution parameter is constrained to a range from 0 to 1. Everything outside the range is clamped to fit into it. 
         * 
         * @param restitution the restitution of the object
         */
        void setRestitution(float restitution);
        /**
         * @brief Get the Restitution
         * 
         * @return `float` the restitution of the object
         */
        inline float getRestitution() const noexcept
        {return m_restitution;}

        /**
         * @brief Set the Filter
         * 
         * @param filter the new filter state of the object
         */
        void setFilter(const Filter& filter);
        /**
         * @brief Get the Filter
         * 
         * @return `const Filter&` a constant reference to the filter state of the object
         */
        inline const Filter& getFilter() const noexcept
        {return m_filter;}

        /**
         * @brief Set the Body Type
         * 
         * @param type the body type for the object
         */
        void setBodyType(BodyType type);
        /**
         * @brief Get the Body Type
         * 
         * @return `BodyType` the body type of the object
         */
        inline BodyType getBodyType() const noexcept
        {return m_type;}

        /**
         * @brief Set the User Data
         * 
         * @param userData the new user data pointer
         */
        void setUserData(void* userData);
        /**
         * @brief Get the User Data
         * 
         * @return `void*` the user data pointer
         */
        inline void* getUserData() const noexcept
        {return m_userData;}

        /**
         * @brief Set the Linear Velocity
         * 
         * @param velocity the new linear velocity
         */
        virtual void setLinearVelocity(const vec3& velocity) = 0;
        /**
         * @brief Get the Linear Velocity
         * 
         * @return `vec3` the current linear velocity
         */
        virtual vec3 getLinearVelocity() const = 0;

        /**
         * @brief Set the Angular Velocity
         * 
         * @param velocity the angular velocity
         */
        virtual void setAngularVelocity(const vec3& velocity) = 0;
        /**
         * @brief Get the Angular Velocity
         * 
         * @return `vec3` the current angular velocity
         */
        virtual vec3 getAngularVelocity() const = 0;

        /**
         * @brief apply a force to the object
         * 
         * Forces are applied at the object's center of mass. 
         * 
         * If the body type is static, this does nothing. 
         * 
         * @param force the force vector to apply to the object
         */
        virtual void applyForce(const vec3& force) = 0;
        /**
         * @brief apply an impulse to the object
         * 
         * The impulse is applied at the object's center of mass. 
         * 
         * If the body type is static, this does nothing. 
         * 
         * @param impulse the impulse to apply
         */
        virtual void applyImpulse(const vec3& impulse) = 0;
        /**
         * @brief apply torque to the object
         * 
         * Torque is applied to the object's center of mass. 
         * 
         * If the body type is static, this does nothing. 
         * 
         * @param torque the torque vector to apply
         */
        virtual void applyTorque(const vec3& torque) = 0;

        /**
         * @brief check if the object is sleeping
         * 
         * A static entity is always considered awake. 
         * 
         * @return `true` if the object is sleeping, `false` if not
         */
        virtual bool isSleeping() const = 0;
        /**
         * @brief wake the object up
         * 
         * If the body type is static, this does nothing. 
         */
        virtual void wakeUp() = 0;
        /**
         * @brief make the object sleep
         * 
         * If the body type is static, this does nothing. 
         */
        virtual void sleep() = 0;

    protected:

        //the world is a friend
        //this is required because the world handles synchronization with the ECS
        friend class World;

        /**
         * @brief store what is dirty
         */
        enum class DirtyFlags : u8 {
            /**
             * @brief nothing is dirty
             */
            NONE = 0,
            /**
             * @brief store if the transform is dirty
             */
            TRANSFORM  = 0x1,
            /**
             * @brief store if the collider is dirty
             * 
             * This may result in the backend fully re-creating the backend object
             */
            COLLIDER   = 0x2,
            /**
             * @brief store if the properties are dirty
             */
            PROPERTIES = 0x4,
            /**
             * @brief store if the user data changed
             */
            USER_DATA  = 0x8,
        };

        /**
         * @brief Set the Transform
         * 
         * @param transf the new transform of the entity
         */
        void setTransform(const Transform& transf);

        /**
         * @brief a function called by the world on an update to clear dirty states
         */
        virtual void flushState() = 0;

        /**
         * @brief check if something is dirty
         * 
         * @return `true` if something is marked dirty, `false` if not
         */
        inline bool isDirty() const noexcept 
        {return m_dirty != DirtyFlags::NONE;}

        /**
         * @brief check if a specific dirty flag set is dirty
         * 
         * @param flags the flags to check
         * @return `true` if any of the flags is considered dirty, `false` if all are clean
         */
        inline bool isDirty(DirtyFlags flags) const noexcept
        {return (static_cast<u8>(m_dirty) & static_cast<u8>(flags)) > 0;}

        /**
         * @brief Get the Dirty Flags
         * 
         * @return `DirtyFlags` a copy of the dirty flags
         */
        inline DirtyFlags getDirtyFlags() const noexcept
        {return m_dirty;}

        /**
         * @brief clear the dirty flags
         */
        void clearDirtyFlags() noexcept
        {m_dirty = DirtyFlags::NONE;}

        /**
         * @brief store the collider
         */
        Reference<Collider> m_collider;
        /**
         * @brief store the world
         * 
         * The world owns the simulated entity. 
         */
        World* m_world = nullptr;

        /**
         * @brief store the mass of the body
         */
        float m_mass = 1.f;
        /**
         * @brief store the friction of the body
         */
        float m_friction = 0.5f;
        /**
         * @brief store the restitution of the body
         */
        float m_restitution = 0.f;

        /**
         * @brief store the transform
         */
        WorldTransform m_transf;

        /**
         * @brief store the cached transform
         */
        WorldTransform m_cachedTransform;

        /**
         * @brief store the filter state
         */
        Filter m_filter;

        /**
         * @brief store the body type
         */
        BodyType m_type = BodyType::STATIC;

        /**
         * @brief mark as dirty if modified from the frontend
         */
        DirtyFlags m_dirty = DirtyFlags::NONE;

        /**
         * @brief store arbitrary user data
         */
        void* m_userData = nullptr;

        /**
         * @brief store if high quality collisions should be used
         */
        bool m_highQualityCollision = false;

        //the implementation backends define how they store the dynamic state. 
        //this is NOT missing here, it is intentionally not defined. 

    };

}

#endif