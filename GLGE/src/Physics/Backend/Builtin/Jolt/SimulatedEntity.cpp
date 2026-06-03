/**
 * @file SimulatedEntity.cpp
 * @author DM8AT
 * @brief implement the simulated entity
 * @version 0.1
 * @date 2026-06-02
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add simulated entities
#include "Physics/Backend/Builtin/Jolt/SimulatedEntity.h"
//add collider backends
#include "Physics/Backend/Builtin/Jolt/Collider.h"
//add the world backend
#include "Physics/Backend/Builtin/Jolt/World.h"

//sanity check 64 bit OS
static_assert(sizeof(void*) == 8, "The library only works on a 64 bit operating system");

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

void GLGE::Physic::Backend::Jolt::SimulatedEntity::setLinearVelocity(const vec3& velocity) {
    //stop if static
    if (m_type == BodyType::STATIC) {return;}

    //get the body interface
    auto& bodyInterface = static_cast<Jolt::World*>(m_world)->getPhysicsSystem().GetBodyInterface();

    //set the linear velocity
    bodyInterface.SetLinearVelocity(
        m_body, 
        JPH::Vec3(velocity.x, velocity.y, velocity.z)
    );
}
GLGE::vec3 GLGE::Physic::Backend::Jolt::SimulatedEntity::getLinearVelocity() const {
    //get the body interface
    auto& bodyInterface = static_cast<Jolt::World*>(m_world)->getPhysicsSystem().GetBodyInterface();

    //extract the linear velocity
    auto vel = bodyInterface.GetLinearVelocity(m_body);

    //return as vec3
    return vec3(vel.GetX(), vel.GetY(), vel.GetZ());
}
void GLGE::Physic::Backend::Jolt::SimulatedEntity::setAngularVelocity(const vec3& velocity) {
    //stop if static
    if (m_type == BodyType::STATIC) {return;}

    //get the body interface
    auto& bodyInterface = static_cast<Jolt::World*>(m_world)->getPhysicsSystem().GetBodyInterface();

    //set the linear velocity
    bodyInterface.SetAngularVelocity(
        m_body, 
        JPH::Vec3(velocity.x, velocity.y, velocity.z)
    );
}
GLGE::vec3 GLGE::Physic::Backend::Jolt::SimulatedEntity::getAngularVelocity() const {
    //get the body interface
    auto& bodyInterface = static_cast<Jolt::World*>(m_world)->getPhysicsSystem().GetBodyInterface();

    //extract the linear velocity
    auto vel = bodyInterface.GetAngularVelocity(m_body);

    //return as vec3
    return vec3(vel.GetX(), vel.GetY(), vel.GetZ());
}
void GLGE::Physic::Backend::Jolt::SimulatedEntity::applyForce(const vec3& force) {
    //stop if static
    if (m_type == BodyType::STATIC) {return;}

    //get the body interface
    auto& bodyInterface = static_cast<Jolt::World*>(m_world)->getPhysicsSystem().GetBodyInterface();

    //apply a force
    bodyInterface.AddForce(
        m_body,
        JPH::Vec3(force.x, force.y, force.z)
    );
}
void GLGE::Physic::Backend::Jolt::SimulatedEntity::applyImpulse(const vec3& impulse) {
    //stop if static
    if (m_type == BodyType::STATIC) {return;}

    //get the body interface
    auto& bodyInterface = static_cast<Jolt::World*>(m_world)->getPhysicsSystem().GetBodyInterface();

    //apply a force
    bodyInterface.AddImpulse(
        m_body,
        JPH::Vec3(impulse.x, impulse.y, impulse.z)
    );
}
void GLGE::Physic::Backend::Jolt::SimulatedEntity::applyTorque(const vec3& torque) {
    //stop if static
    if (m_type == BodyType::STATIC) {return;}

    //get the body interface
    auto& bodyInterface = static_cast<Jolt::World*>(m_world)->getPhysicsSystem().GetBodyInterface();

    //apply a force
    bodyInterface.AddTorque(
        m_body,
        JPH::Vec3(torque.x, torque.y, torque.z)
    );
}

bool GLGE::Physic::Backend::Jolt::SimulatedEntity::isSleeping() const {
    //static is never sleeping
    if (m_type == BodyType::STATIC)
    {return false;}

    //get the body interface
    auto& bodyInterface = static_cast<Jolt::World*>(m_world)->getPhysicsSystem().GetBodyInterface();

    //query from the interface
    return !bodyInterface.IsActive(m_body);
}
void GLGE::Physic::Backend::Jolt::SimulatedEntity::wakeUp() {
    //static can't wake up
    if (m_type == BodyType::STATIC)
    {return;}

    //get the body interface
    auto& bodyInterface = static_cast<Jolt::World*>(m_world)->getPhysicsSystem().GetBodyInterface();

    //wake it up
    bodyInterface.ActivateBody(m_body);
}
void GLGE::Physic::Backend::Jolt::SimulatedEntity::sleep() {
    //static can't sleep
    if (m_type == BodyType::STATIC)
    {return;}

    //get the body interface
    auto& bodyInterface = static_cast<Jolt::World*>(m_world)->getPhysicsSystem().GetBodyInterface();

    //make it sleep
    bodyInterface.DeactivateBody(m_body);
}

void GLGE::Physic::Backend::Jolt::SimulatedEntity::flushState() {
    //check if the collider is dirty
    if (isDirty(DirtyFlags::COLLIDER)) {
        //get the shape
        auto shape = getShape(m_collider);

        //get the body interface
        auto& bodyInterface = static_cast<Jolt::World*>(m_world)->getPhysicsSystem().GetBodyInterface();

        //set the shape
        bodyInterface.SetShape(m_body, shape, true, JPH::EActivation::Activate);
    }

    //check if the transform is dirty
    if (isDirty(DirtyFlags::TRANSFORM)) {
        //get the body interface
        auto& bodyInterface = static_cast<Jolt::World*>(m_world)->getPhysicsSystem().GetBodyInterface();

        //make sure the transform is valid
        if (m_transf.rot.length() != 1)
        {m_transf.rot = Quaternion(1,0,0,0);}

        //set position and rotation of the object
        bodyInterface.SetPositionAndRotation(
            m_body,
            JPH::RVec3(m_transf.pos.x, m_transf.pos.y, m_transf.pos.z),
            JPH::Quat(m_transf.rot.x, m_transf.rot.y, m_transf.rot.z, m_transf.rot.w),
            JPH::EActivation::Activate
        );
    }

    //check if the properties are out of date
    if (isDirty(DirtyFlags::PROPERTIES)) {
        //recreate (because mass update required)
        static_cast<Jolt::World*>(m_world)->recreate(this);
    }

    //check if the user data is out of date
    if (isDirty(DirtyFlags::USER_DATA)) {
        //get the body interface
        auto& bodyInterface = static_cast<Jolt::World*>(m_world)->getPhysicsSystem().GetBodyInterface();

        //update the user data
        //the reinterpret_cast is only allowed because a 64 bit system is enforced
        bodyInterface.SetUserData(m_body, reinterpret_cast<JPH::uint64>(m_userData));
    }

    //clear dirty flags
    clearDirtyFlags();
}