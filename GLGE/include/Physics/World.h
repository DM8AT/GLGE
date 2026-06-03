/**
 * @file World.h
 * @author DM8AT
 * @brief define the frontend of a physics world
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_PHYSICS_WORLD_
#define _GLGE_PHYSICS_WORLD_

//add simulated entities
#include "SimulatedEntity.h"
//add the world backend
#include "Backend/World.h"
//add colliders
#include "Collider.h"

//add instances and base classes
#include "Core/BaseClass.h"
#include "Instance.h"

//use the physics namespace
namespace GLGE::Physic {

    /**
     * @brief define the frontend wrapper for the world calss
     */
    class World : public BaseClass {
    public:

        /**
         * @brief Construct a new World
         * 
         * @param world a reference to the ECS world to wrap
         * @param gravity the vector that defines the direction of gravity
         * @param maxBodies the maximum amount of bodies the physics world may have
         * @param tempAllocatorSize the size of the temporary allocator
         */
        World(GLGE::World& world, const vec3& gravity = vec3{0,-9.81, 0}, u64 maxBodies = 5E4, u64 tempAllocatorSize = 512*1024*1024)
         : BaseClass(), m_world(getInstance()->getExtension<GLGE::Physic::Instance>()->getDescription()->createWorld(
            getInstance()->getExtension<GLGE::Physic::Instance>()->getBackend().get(), world, gravity, maxBodies, tempAllocatorSize
         ))
        {}

        /**
         * @brief update the physic world
         */
        inline void update() const noexcept
        {m_world->update();}

        /**
         * @brief Create a new Entity
         * 
         * @param collider a constant reference to the collider to use
         * @param type the type of the entity
         * @param mass the mass of the entity
         * @param friction the friction of the entity
         * @param restitution the restitution of the entity
         * @param highQualityCollision `true` to use high quality collisions, `false` to use the default collisions
         * @return `SimulatedEntity` the new simulated entity
         */
        inline SimulatedEntity createEntity(const GLGE::Physic::Collider& collider, GLGE::Physic::SimulatedEntity::BodyType type = SimulatedEntity::BodyType::STATIC, float mass = 1, float friction = 0.5, float restitution = 0.f, bool highQualityCollision = false) {
            //setup the entity and create it using the internal world
            SimulatedEntity ent;
            ent.m_entity = m_world->createEntity(collider.getBackend(), type, mass, friction, restitution, highQualityCollision);
            //return the entity
            return ent;
        };

    protected:

        /**
         * @brief store the wrapped backend
         */
        Reference<Backend::World> m_world;

    };

}

#endif