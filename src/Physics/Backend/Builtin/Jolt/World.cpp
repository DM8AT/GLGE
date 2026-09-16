/**
 * @file World.cpp
 * @author DM8AT
 * @brief implement the physics world for jolt
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the jolt world
#include "Physics/Backend/Builtin/Jolt/World.h"
//add simulated entities
#include "Physics/Backend/Builtin/Jolt/SimulatedEntity.h"
//add collider backends
#include "Physics/Backend/Builtin/Jolt/Collider.h"

//required GLM utility
#include <glm/gtx/matrix_decompose.hpp>

//add instances
#include "Physics/Instance.h"

//add required components
#include "Core/Transform.h"
#include "Physics/RigidBody.h"

//jolt includes
#include <Jolt/Physics/Body/BodyCreationSettings.h>

//get the simulated entity frontend
#include "Physics/SimulatedEntity.h"

static JPH::RefConst<JPH::Shape> getShape(const GLGE::Reference<GLGE::Physic::Backend::Collider>& collider) {
    switch (collider->getType()) {
    case GLGE::Physic::Backend::Collider::Type::SPHERE:
        return static_cast<GLGE::Physic::Backend::Jolt::SphereCollider*>(collider.get())->getShape().GetPtr();

    case GLGE::Physic::Backend::Collider::Type::BOX:
        return static_cast<GLGE::Physic::Backend::Jolt::BoxCollider*>(collider.get())->getShape().GetPtr();

    default:
        return nullptr;
    }
}

GLGE::Physic::Backend::Jolt::World::World(GLGE::Physic::Backend::Instance* instance, GLGE::World& world, const vec3& gravity, u64 maxBodies, u64 tempAllocatorSize) 
 : Backend::World(instance, world, gravity, maxBodies, tempAllocatorSize),
   cm_maxBodyPairs(4*cm_maxBodies),
   cm_maxContactConstraints(2*cm_maxBodies),
   m_tempAllocator(cm_tempAllocSize),
   m_jobSystem(
    JPH::cMaxPhysicsJobs,
    JPH::cMaxPhysicsBarriers,
    glm::max<u64>(1u, std::thread::hardware_concurrency() - 1)
   )
{
    //initialize the physics system
    m_physicsSystem.Init(
        cm_maxBodies,
        0,
        cm_maxBodyPairs,
        cm_maxContactConstraints,
        m_broadPhaseLayerInterface,
        m_objectVsBroadPhaseLayerFilter,
        m_objectLayerPairFilter
    );
    //set the gravity
    m_physicsSystem.SetGravity(JPH::Vec3(m_gravity.x, m_gravity.y, m_gravity.z));

    //attach to instance
    m_instance->registerWorld(this);
}

GLGE::Physic::Backend::Jolt::World::~World() {
    //remove from the instance
    m_instance->removeWorld(this);
}

GLGE::Reference<GLGE::Physic::Backend::SimulatedEntity> GLGE::Physic::Backend::Jolt::World::createEntity(const Reference<Backend::Collider>& collider, Backend::SimulatedEntity::BodyType type, float mass, float friction, float restitution, bool hcCollision) {
    //return the new entity
    return Reference<Backend::SimulatedEntity>(new Jolt::SimulatedEntity(createJoltBody(collider, type, mass, friction, restitution, hcCollision), this, collider, type, mass, friction, restitution, hcCollision), false);
}

void GLGE::Physic::Backend::Jolt::World::onUpdate() {
    //flush pending bodies if some are there
    if (m_hasCache.load(std::memory_order_acquire)) {
        //lock
        std::unique_lock lock(m_pendingBodyMtx);

        //get the body interface
        auto& bodyInterface = m_physicsSystem.GetBodyInterface();
        //add and finalize
        auto state = bodyInterface.AddBodiesPrepare(m_pendingIds.data(), m_pendingIds.size());
        bodyInterface.AddBodiesFinalize(m_pendingIds.data(), m_pendingIds.size(), state, JPH::EActivation::Activate);

        //clean up
        m_pendingIds.clear();
        m_hasCache.store(false, std::memory_order_release);
    }

    //iterate over all entities and potentially initialize and update them
    m_world->parallel_each<WorldTransform, RigidBody>([this](const WorldTransform& transf, const RigidBody& body) {
        //if the entity is invalid, just stop
        if (!body.entity) {return;}
        //get the entity
        auto* ent = static_cast<Jolt::SimulatedEntity*>(body.entity->getBackend().get());

        //if the entity was never initialized, initialize it
        if (!ent->m_hadTick || (ent->getBodyType() != SimulatedEntity::BodyType::DYNAMIC)) 
        {ent->setTransform(transf);}

        //update it
        ent->flushState();
    });

    //get delta time
    double dt = m_instance->getLimiter().getCurrentDeltaTime() * 0.001;
    //tick the physics system
    m_physicsSystem.Update(dt, m_instance->getInstance()->getDescription()->getSubsteps(), &m_tempAllocator, &m_jobSystem);

    //say that now data exists
    m_world->parallel_each<WorldTransform, RigidBody>([this](const WorldTransform& wTransf, const RigidBody& body) {
        //stop on invalid entity
        if (!body.entity) {return;}
        //get the entity
        auto* ent = static_cast<Jolt::SimulatedEntity*>(body.entity->getBackend().get());

        //query the transform
        auto& bodyInterface = m_physicsSystem.GetBodyInterface();
        JPH::RVec3 pos;
        JPH::Quat rot;
        bodyInterface.GetPositionAndRotation(ent->getBody(), pos, rot);
        //store the transform
        WorldTransform transf;
        transf.pos = vec3(pos.GetX(), pos.GetY(), pos.GetZ());
        transf.rot = Quaternion(rot.GetW(), rot.GetX(), rot.GetY(), rot.GetZ());
        transf.scale = wTransf.scale;
        ent->m_transf = transf;

        //data exists
        ent->m_hadTick = true;
    });
}

void GLGE::Physic::Backend::Jolt::World::update() {
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
        auto* ent = static_cast<Jolt::SimulatedEntity*>(body.entity->getBackend().get());

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

void GLGE::Physic::Backend::Jolt::World::recreate(Jolt::SimulatedEntity* entity) {
    //get the body interface
    auto& bodyInterface = m_physicsSystem.GetBodyInterface();

    //delete the old entity
    bodyInterface.DestroyBody(entity->getBody());

    //create the new one
    entity->setBody(createJoltBody(entity->getCollider(), entity->getBodyType(), entity->getMass(), entity->getFriction(), entity->getRestitution(), entity->m_highQualityCollision));
    //mark as not updated
    entity->m_hadTick = false;
}

JPH::BodyID GLGE::Physic::Backend::Jolt::World::createJoltBody(const Reference<Backend::Collider>& collider, Backend::SimulatedEntity::BodyType type, float mass, float friction, float restitution, bool hcCollision) {
    //map the body type
    JPH::EMotionType motionType = JPH::EMotionType::Static;
    switch (type) {
    case SimulatedEntity::BodyType::STATIC:
        motionType = JPH::EMotionType::Static;
        break;
    case SimulatedEntity::BodyType::KINEMATIC:
        motionType = JPH::EMotionType::Kinematic;
        break;
    case SimulatedEntity::BodyType::DYNAMIC:
        motionType = JPH::EMotionType::Dynamic;
        break;
    }

    //get the shape (collider)
    auto shape = getShape(collider);

    //set the settings for the body
    JPH::BodyCreationSettings settings(
        shape,
        JPH::RVec3::sZero(),
        JPH::Quat::sIdentity(),
        motionType,
        Internal::Layers::MOVING
    );
    settings.mFriction = friction;
    settings.mRestitution = restitution;
    if (hcCollision)
    {settings.mMotionQuality = JPH::EMotionQuality::LinearCast;}
    if (motionType == JPH::EMotionType::Dynamic) 
    {settings.mMassPropertiesOverride.mMass = mass;}

    //create the simulated entity
    JPH::BodyID id = m_physicsSystem.GetBodyInterface().CreateBody(settings)->GetID();

    //check if a cache exists
    std::lock_guard lock(m_pendingBodyMtx);
    //add the ID to the queue
    m_pendingIds.push_back(id);
    //there is now a cache
    m_hasCache.store(true, std::memory_order_release);

    //return the ID
    return id;
}