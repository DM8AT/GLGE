/**
 * @file World.h
 * @author DM8AT
 * @brief define the backend of a physics world
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_PHYSICS_BACKEND_WORLD_
#define _GLGE_PHYSICS_BACKEND_WORLD_

//add instances
#include "Instance.h"
//add simulated entities
#include "SimulatedEntity.h"
//add the ECS
#include "Core/Object.h"

//add references
#include "Core/Reference.h"

//use the backend namespace
namespace GLGE::Physic::Backend {

    /**
     * @brief store the backend implementation for a world instance
     * 
     * A physics world is used as a frame of reference to know with what another entity may collide with. 
     */
    class World : public Referable {
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
        World(Instance* instance, GLGE::World& world, const vec3& gravity, u64 maxBodies, u64 tempAllocatorSize)
         : m_instance(instance), m_world(&world), m_gravity(gravity), cm_maxBodies(glm::clamp<u64>(maxBodies, 1, 1E8)), cm_tempAllocSize(glm::clamp<u64>(tempAllocatorSize, 1024, 128ull*1024ull*1024ull*1024ull))
        {}

        //worlds cannot be copied nor moved
        World(World&&) = delete;
        World(const World&) = delete;
        World& operator=(World&&) = delete;
        World& operator=(const World&) = delete;

        /**
         * @brief Destroy the World
         */
        virtual ~World() {}

        /**
         * @brief Create an Entity
         * 
         * @param collider a reference to the backend collider the entity should use
         * @param type the body type of the entity
         * @param mass the mass of the entity
         * @param friction the friction the entity experiences
         * @param restitution the restitution of the entity
         * @param highQualityCollision `true` to use high quality collisions, `false` to use the default collisions
         * @return `Reference<SimulatedEntity>` a reference to the new simulated entity
         */
        virtual Reference<SimulatedEntity> createEntity(const Reference<Collider>& collider, SimulatedEntity::BodyType type, float mass, float friction, float restitution, bool highQualityCollision) = 0;

        /**
         * @brief a function that is called every tick from the instance
         */
        virtual void onUpdate() = 0;

        /**
         * @brief a function to sync physics and the ECS
         */
        virtual void update() = 0;

    protected:

        /**
         * @brief a pointer to the wrapped ECS world
         */
        GLGE::World* m_world = nullptr;
        /**
         * @brief store a pointer to the instance the world belongs to
         */
        Instance* m_instance = nullptr;

        /**
         * @brief store the gravity vector
         */
        vec3 m_gravity = {0, -9.81, 0};

        /**
         * @brief define the hard upper limit for the maximum body count
         */
        const u32 cm_maxBodies;

        /**
         * @brief define the size of the temporary allocator
         */
        const u32 cm_tempAllocSize;

    };

}

#endif