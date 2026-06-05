/**
 * @file World.h
 * @author DM8AT
 * @brief define the world backend overload for bullet
 * @version 0.1
 * @date 2026-06-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_PHYSICS_BACKEND_BULLET_WORLD_
#define _GLGE_PHYSICS_BACKEND_BULLET_WORLD_

//add the world glue layer
#include "Physics/Backend/World.h"
//add bullet
#include <btBulletDynamicsCommon.h>

//use the backend namespace
namespace GLGE::Physic::Backend::Bullet {

    /**
     * @brief define the bullet world backend
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

    protected:

        /**
         * @brief store the collisoin configuration
         */
        std::unique_ptr<btDefaultCollisionConfiguration> m_config;
        /**
         * @brief store the dispatcher
         */
        std::unique_ptr<btCollisionDispatcher> m_dispatcher;
        /**
         * @brief store the broad phase interface
         */
        std::unique_ptr<btBroadphaseInterface> m_broadphase;
        /**
         * @brief store the sequential impulse constraint solver
         */
        std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
        /**
         * @brief store the rigid body world
         */
        std::unique_ptr<btDiscreteDynamicsWorld> m_discWorld;
    
    };

}

#endif