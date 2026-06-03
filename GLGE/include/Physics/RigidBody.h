/**
 * @file RigidBody.h
 * @author DM8AT
 * @brief define the rigid body ECS component
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_PHYSICS_RIGID_BODY_
#define _GLGE_PHYSICS_RIGID_BODY_

//add simulated entities
#include "SimulatedEntity.h"

//use the physics namespace
namespace GLGE::Physic {

    /**
     * @brief the rigid body component that marks an entity as capable of rigid-body physics
     * 
     * @warning This component only works if the entity has a `WorldTransform` component. To create `WorldTransforms`, either manually add one or (Preferred) add a `Transform` and call `GLGE::System::BakeTransforms(world)`. 
     * 
     * @warning This component may modify transform ownership behaviour. If the entity body type is dynamic, the physics engine will own the canonic transform. For static and kinematic entities, the ECS owns the canonic transform. The physic system will pull the transforms from them automatically. 
     */
    struct RigidBody {
        /**
         * @brief store a pointer to the simulated entity to use
         */
        SimulatedEntity* entity;
    };

}

#endif