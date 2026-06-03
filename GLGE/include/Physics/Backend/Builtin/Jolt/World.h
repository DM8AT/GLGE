/**
 * @file World.h
 * @author DM8AT
 * @brief define the world overload for jolt
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_PHYSICS_BACKEND_JOLT_WORLD_
#define _GLGE_PHYSICS_BACKEND_JOLT_WORLD_

//add worlds
#include "Physics/Backend/World.h"

//add internals
#include "Jolt_Internal_LayerSetup.h"

//use the backend namespace
namespace GLGE::Physic::Backend::Jolt {

    //simulated entities are defined somewhere else
    class SimulatedEntity;

    /**
     * @brief define the world overload for jolt
     */
    class World : public Backend::World {
    public:

        /**
         * @brief Construct a new World
         * 
         * @param instance a pointer to the instance the world belongs to
         * @param world a reference to the world wrapped by the physic world
         * @param gravity the gravity vector
         * @param maxBodies the maximum amount of allowed bodies
         * @param tempAllocatorSize the size in bytes of the temporary allocator
         */
        World(Backend::Instance* instance, GLGE::World& world, const vec3& gravity, u64 maxBodies, u64 tempAllocatorSize);

        /**
         * @brief Destroy the World
         */
        virtual ~World() override;

        /**
         * @brief Create an Entity
         * 
         * @param collider a reference to the backend collider the entity should use
         * @param type the body type of the entity
         * @param mass the mass of the entity
         * @param friction the friction the entity experiences
         * @param restitution the restitution of the entity
         * @return `Reference<SimulatedEntity>` a reference to the new simulated entity
         * @param highQualityCollision `true` to use high quality collisions, `false` to use the default collisions
         */
        virtual Reference<Backend::SimulatedEntity> createEntity(const Reference<Backend::Collider>& collider, Backend::SimulatedEntity::BodyType type, float mass, float friction, float restitution, bool highQualityCollision) override;

        /**
         * @brief a function that is called every tick from the instance
         */
        virtual void onUpdate() override;

        /**
         * @brief a function to sync physics and the ECS
         */
        virtual void update() override;

        /**
         * @brief Get the Physics System
         * 
         * @return `JPH::PhysicsSystem&` a modifiable reference to the physics system
         */
        inline JPH::PhysicsSystem& getPhysicsSystem() noexcept
        {return m_physicsSystem;}

        /**
         * @brief recreate a specific entity
         * 
         * @param entity a pointer to the entity to re-create
         */
        void recreate(Jolt::SimulatedEntity* entity);

    protected:

        /**
         * @brief Create a Jolt Body
         * 
         * @param collider the collider to use
         * @param type the body type
         * @param mass the mass
         * @param friction the friction value
         * @param restitution the restitution value
         * @param hcCollision `true` to use high quality collisions, `false` to use the default collisions
         * @return `JPH::BodyID` the body ID
         */
        JPH::BodyID createJoltBody(const Reference<Backend::Collider>& collider, Backend::SimulatedEntity::BodyType type, float mass, float friction, float restitution, bool hcCollision);

        //to change these settings, inherit from the world and just change them before calling the constructor

        /**
         * @brief define the maximum amount of allowed physics body pairs
         */
        const u32 cm_maxBodyPairs;
        /**
         * @brief define the maximum amount of contact constrains
         */
        const u32 cm_maxContactConstraints;

        /**
         * @brief store the amount of entities discoverd on the last iteration
         * 
         * This is used for fast pre-resizing of the entity discovery table
         */
        u64 m_entityCountCache = 1024;

        /**
         * @brief store the physics system
         */
        JPH::PhysicsSystem m_physicsSystem;
        /**
         * @brief store a temporary allocator
         */
        JPH::TempAllocatorImpl m_tempAllocator;
        /**
         * @brief store the job system
         */
        JPH::JobSystemThreadPool m_jobSystem;
        /**
         * @brief store the broad phase layer interface
         */
        Internal::BPLayerInterfaceImpl m_broadPhaseLayerInterface;
        /**
         * @brief store the filter for objects against the broad phase layer
         */
        Internal::ObjectVsBroadPhaseLayerFilterImpl m_objectVsBroadPhaseLayerFilter;
        /**
         * @brief store the filter for object pairs
         */
        Internal::ObjectLayerPairFilterImpl m_objectLayerPairFilter;

        /**
         * @brief store if there is currently a cache
         */
        std::atomic_bool m_hasCache {false};
        /**
         * @brief store a mutex to make adding bodies thread-safe
         */
        std::mutex m_pendingBodyMtx;
        /**
         * @brief store the body IDs for all pending bodies
         */
        std::vector<JPH::BodyID> m_pendingIds;

    };

}

#endif