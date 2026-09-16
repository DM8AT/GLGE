/**
 * @file World.cpp
 * @author DM8AT
 * @brief implement the world class for bullet
 * @version 0.1
 * @date 2026-06-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the Bullet world
#include "Physics/Backend/Builtin/Bullet/World.h"
//add simulated entities
#include "Physics/Backend/Builtin/Bullet/SimulatedEntity.h"
//add collider backends
#include "Physics/Backend/Builtin/Bullet/Collider.h"

//required GLM utility
#include <glm/gtx/matrix_decompose.hpp>

//add instances
#include "Physics/Instance.h"

//add required components
#include "Core/Transform.h"
#include "Physics/RigidBody.h"


//get the simulated entity frontend
#include "Physics/SimulatedEntity.h"

GLGE::Physic::Backend::Bullet::World::World(GLGE::Physic::Backend::Instance* instance, GLGE::World& world, const vec3& gravity, u64 maxBodies, u64 tempAllocatorSize) 
 : Backend::World(instance, world, gravity, maxBodies, tempAllocatorSize),
   m_config(std::make_unique<btDefaultCollisionConfiguration>()),
   m_dispatcher(std::make_unique<btCollisionDispatcher>(m_config.get())),
   m_broadphase(std::make_unique<btDbvtBroadphase>()),
   m_solver(std::make_unique<btSequentialImpulseConstraintSolver>()),
   m_discWorld(std::make_unique<btDiscreteDynamicsWorld>(m_dispatcher.get(), m_broadphase.get(), m_solver.get(), m_config.get()))
{
    //setup the contact solver
    btContactSolverInfo& info = m_discWorld->getSolverInfo();
    info.m_numIterations = 30;
    info.m_splitImpulse = 1;
    info.m_splitImpulsePenetrationThreshold = 0.f;
    info.m_erp = 0.6f;
    info.m_globalCfm = 0.0f;
    //set the gravity
    m_discWorld->setGravity(btVector3(m_gravity.x, m_gravity.y, m_gravity.z));
    //attach to instance
    m_instance->registerWorld(this);
}

GLGE::Physic::Backend::Bullet::World::~World() {
    //remove from the instance
    m_instance->removeWorld(this);
}

GLGE::Reference<GLGE::Physic::Backend::SimulatedEntity> GLGE::Physic::Backend::Bullet::World::createEntity(const Reference<Backend::Collider>& collider, Backend::SimulatedEntity::BodyType type, float mass, float friction, float restitution, bool hcCollision) {
    //create the new entity
    auto* entPtr = new Bullet::SimulatedEntity(this, collider, type, mass, friction, restitution, hcCollision);
    Reference<Backend::SimulatedEntity> ent(entPtr, false);

    //add the entity to the world
    m_discWorld->addRigidBody(entPtr->getBody());

    //return the entity
    return ent;
}

void GLGE::Physic::Backend::Bullet::World::onUpdate() {
    //iterate over all entities and potentially initialize and update them
    m_world->parallel_each<WorldTransform, RigidBody>([this](const WorldTransform& transf, const RigidBody& body) {
        //if the entity is invalid, just stop
        if (!body.entity) {return;}
        //get the entity
        auto* ent = static_cast<Bullet::SimulatedEntity*>(body.entity->getBackend().get());

        //if the entity was never initialized, initialize it
        if (!ent->m_hadTick || ((ent->getBodyType() != SimulatedEntity::BodyType::DYNAMIC) && (ent->m_transf.pos != transf.pos || ent->m_transf.rot != transf.rot || ent->m_transf.scale != transf.scale))) 
        {ent->setTransform(transf);}

        //update it
        ent->flushState();
    });

    //get delta time
    double dt = m_instance->getLimiter().getCurrentDeltaTime() * 0.001;
    //tick the physics world
    m_discWorld->stepSimulation(dt, m_instance->getInstance()->getDescription()->getSubsteps(), 1.f/240.f);

    //say that now data exists
    m_world->parallel_each<WorldTransform, RigidBody>([this](const WorldTransform& wTransf, const RigidBody& body) {
        //stop on invalid entity
        if (!body.entity) {return;}
        //get the entity
        auto* ent = static_cast<Bullet::SimulatedEntity*>(body.entity->getBackend().get());

        //query the transform
        const btTransform& physTransf = ent->m_body->getWorldTransform();
        //store the transform
        WorldTransform transf;
        transf.pos = vec3(physTransf.getOrigin().getX(), physTransf.getOrigin().getY(), physTransf.getOrigin().getZ());
        transf.rot = Quaternion(physTransf.getRotation().getW(), physTransf.getRotation().getX(), physTransf.getRotation().getY(), physTransf.getRotation().getZ());
        transf.scale = wTransf.scale;
        ent->m_transf = transf;

        //data exists
        ent->m_hadTick = true;
    });
}

void GLGE::Physic::Backend::Bullet::World::update() {
    //iterate over all components
    m_world->parallel_each<Transform, Component::HierarchyNode, WorldTransform, RigidBody>([this](
        Transform& transf,
        const Component::HierarchyNode& node,
        WorldTransform& worldTransf,
        const RigidBody& body
    ) {
        //stop on invalid entity
        if (!body.entity) {return;}
        //get the entity
        auto* ent = static_cast<Bullet::SimulatedEntity*>(body.entity->getBackend().get());

        //check if the cached transform matches the known one
        if ((ent->m_hadTick) && (ent->getBodyType() == SimulatedEntity::BodyType::DYNAMIC)) {
            //match -> do a physics update
            
            //re-construct the local transform
            WorldTransform physicsWorld = ent->getTransform();
            //if the scale is 0,0,0, set to 1,1,1
            if (physicsWorld.scale.x == 0 && physicsWorld.scale.y == 0 && physicsWorld.scale.z == 0)
            {physicsWorld.scale = vec3(1,1,1);}
            //check if the rotation is valid
            if (physicsWorld.rot.length() < 1)
            {physicsWorld.rot = Quaternion(1,0,0,0);}

            //get parent world transform
            WorldTransform parentWorld {
                .pos   = vec3(0.f),
                .rot   = Quaternion(1.f, 0.f, 0.f, 0.f),
                .scale = vec3(1.f)
            };
            if(node.parent != Tiny::ECS::Entity::getInvalid()) {
                if(auto* pWorld = m_world->get<WorldTransform>(node.parent))
                {parentWorld = *pWorld;}
            }

            //re-construct the local transform
            glm::mat4 parentMat = parentWorld.toMatrix();
            glm::mat4 worldMat  = physicsWorld.toMatrix();
            glm::mat4 localMat = glm::inverse(parentMat) * worldMat;

            vec3 scale;
            vec3 translation;
            vec3 skew;
            vec4 perspective;
            Quaternion rotation;
            glm::decompose(localMat, scale, rotation, translation, skew, perspective);

            //store the new transforms
            transf.pos   = translation;
            transf.rot   = glm::normalize(rotation);
            transf.scale = scale;

            worldTransf = physicsWorld;
        }

        //cache the transform
        ent->m_cachedTransform = worldTransf;
    });
}