/**
 * @file Description.h
 * @author DM8AT
 * @brief define the backend description
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_PHYSICS_BACKEND_DESCRIPTION_
#define _GLGE_PHYSICS_BACKEND_DESCRIPTION_

//add backend classes
#include "Collider.h"
#include "SimulatedEntity.h"
#include "World.h"
#include "Instance.h"

//use the physics namespace
namespace GLGE::Physic {

    /**
     * @brief a namespace for all backend stuff for physics
     */
    namespace Backend {

        /**
         * @brief define the description of a physics backend
         */
        class Description {
        public:

            /**
             * @brief Construct a new Description
             */
            Description() = default;

            /**
             * @brief Destroy the Description
             */
            virtual ~Description() {}

            /**
             * @brief Get the Name
             * 
             * @return `const std::string&` the name of the backend
             */
            const std::string& getName() const noexcept
            {return m_name;}

            /**
             * @brief Get the Version
             * 
             * @return `const Version&` the version of the backend
             */
            const Version& getVersion() const noexcept
            {return m_version;}

            /**
             * @brief Create an Instance
             * 
             * @param instance a pointer to the used frontend instance
             * @return `Reference<Instance>` a reference to the new instance
             */
            virtual Reference<Instance> createInstance(Physic::Instance* instance) = 0;

            /**
             * @brief Create a Sphere Collider
             * 
             * @param radius the radius for the sphere collider
             * @return `Reference<SphereCollider>` a reference to the sphere collider
             */
            virtual Reference<SphereCollider> createSphereCollider(float radius) = 0;

            /**
             * @brief Create a Box Collider
             * 
             * @param halfExtent the half extent for the box collider
             * @return `Reference<BoxCollider>` a reference to the box collider
             */
            virtual Reference<BoxCollider> createBoxCollider(const vec3& halfExtent) = 0;

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
            virtual Reference<Backend::World> createWorld(GLGE::Physic::Backend::Instance *instance, GLGE::World &world, const vec3& gravity, u64 maxBodies, u64 tempAllocatorSize) = 0;

            /**
             * @brief Get the Substep count
             * 
             * @return `u16` the amount of substeps
             */
            inline u16 getSubsteps() const noexcept
            {return m_substeps;}

        protected:

            /**
             * @brief store the backend name
             */
            std::string m_name = "UNNAMED_PHYSICS_BACKEND";

            /**
             * @brief store the backend version
             */
            Version m_version = {0,0,0};
            /**
             * @brief store the amount of used sub-steps
             * 
             * Default is 1
             */
            u16 m_substeps = 1;

        };

    }

}

#endif