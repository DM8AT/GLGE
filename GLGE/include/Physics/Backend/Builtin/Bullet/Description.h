/**
 * @file Description.h
 * @author DM8AT
 * @brief define the bullet description
 * @version 0.1
 * @date 2026-06-03
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_PHYSICS_BACKEND_BULLET_DESCRIPTION_
#define _GLGE_PHYSICS_BACKEND_BULLET_DESCRIPTION_

//add descriptions
#include "Physics/Backend/Description.h"

//use the physic namespace
namespace GLGE::Physic::Builtin {

        /**
         * @brief define the jolt description
         */
        class Bullet : public Physic::Backend::Description {
        public:

            /**
             * @brief Construct a new Jolt description
             */
            Bullet();

            /**
             * @brief Create an Instance
             * 
             * @param instance a pointer to the frontend instance
             * @return `Reference<Instance>` a reference to the new instance
             */
            virtual Reference<Backend::Instance> createInstance(Physic::Instance* instance) override;

            /**
             * @brief Create a Sphere Collider
             * 
             * @param radius the radius for the sphere collider
             * @return `Reference<SphereCollider>` a reference to the sphere collider
             */
            virtual Reference<Backend::SphereCollider> createSphereCollider(float radius) override;

            /**
             * @brief Create a Box Collider
             * 
             * @param halfExtent the half extent for the box collider
             * @return `Reference<BoxCollider>` a reference to the box collider
             */
            virtual Reference<Backend::BoxCollider> createBoxCollider(const vec3& halfExtent) override;

            /**
             * @brief Create a World
             * 
             * @param instance a pointer to the backend instance referenced by the world
             * @param world a reference to the world to wrap
             * @param gravity the direction that defines the direction of gravity
             * @param maxBodies the maximum amount of allowed bodies
             * @param tempAllocatorSize the size in bytes of the temporary allocator
             * @return `Reference<World>` a reference to the new world
             */
            virtual Reference<Backend::World> createWorld(GLGE::Physic::Backend::Instance *instance, GLGE::World &world, const vec3& gravity, u64 maxBodies, u64 tempAllocatorSize) override;

        };

}

#endif