/**
 * @file SimulatedEntity.cpp
 * @author DM8AT
 * @brief implement the simulated entity class for bullet
 * @version 0.1
 * @date 2026-06-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add simulated entities
#include "Physics/Backend/Builtin/Bullet/SimulatedEntity.h"
//add collider backends
#include "Physics/Backend/Builtin/Bullet/Collider.h"
//add the world backend
#include "Physics/Backend/Builtin/Bullet/World.h"

GLGE::Physic::Backend::Bullet::SimulatedEntity::SimulatedEntity(Backend::World* world, const Reference<Backend::Collider>& collider, BodyType type, float mass, float friction, float restitution, bool highQualityCollision)
 : Backend::SimulatedEntity(world, collider, type, mass, friction, restitution, highQualityCollision)
{
    //set mass to 0 for non-dynamic elements
    if (m_type == BodyType::STATIC)
    {m_mass = 0.f;}
    //store the shape
    btCollisionShape* shape = nullptr;
    //extract the shape
    switch (m_collider->getType()) {
    case Collider::Type::SPHERE:
        shape = static_cast<Bullet::SphereCollider*>(m_collider.get())->getShape();
        break;
    case Collider::Type::BOX:
        shape = static_cast<Bullet::BoxCollider*>(m_collider.get())->getShape();
        break;
    
    default:
        break;
    }
    //use the default motion state
    btTransform startTransform;
    startTransform.setIdentity();
    m_state = std::make_unique<btDefaultMotionState>();
    //compute local inertia
    btVector3 localInertia(0,0,0);
    if (mass != 0.f) 
    {shape->calculateLocalInertia(mass, localInertia);}
    //create the entity
    m_body = std::make_unique<btRigidBody>(m_mass, m_state.get(), shape, localInertia);
    //set the body type
    if (m_type == BodyType::STATIC)
    {m_body->setCollisionFlags(m_body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);}
    if (m_type == BodyType::KINEMATIC)
    {m_body->setCollisionFlags(m_body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);}
    //set high quality collision
    if(highQualityCollision) {
        m_body->setCcdMotionThreshold(0.04f);
        m_body->setCcdSweptSphereRadius(0.2f);
    }
}


void GLGE::Physic::Backend::Bullet::SimulatedEntity::setLinearVelocity(const vec3& velocity) {
    //stop if static
    if (m_type == BodyType::STATIC) {return;}

    //set the linear velocity
    m_body->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
}
GLGE::vec3 GLGE::Physic::Backend::Bullet::SimulatedEntity::getLinearVelocity() const {
    //get the linear velocity
    auto& vel = m_body->getLinearVelocity();
    //return as vec3
    return vec3(vel.getX(), vel.getY(), vel.getZ());
}
void GLGE::Physic::Backend::Bullet::SimulatedEntity::setAngularVelocity(const vec3& velocity) {
    //stop if static
    if (m_type == BodyType::STATIC) {return;}

    //set the angular velocity
    m_body->setAngularVelocity(btVector3(velocity.x, velocity.y, velocity.z));
}
GLGE::vec3 GLGE::Physic::Backend::Bullet::SimulatedEntity::getAngularVelocity() const {
    //extract the angular velocity
    auto& vel = m_body->getAngularVelocity();

    //return as vec3
    return vec3(vel.getX(), vel.getY(), vel.getZ());
}
void GLGE::Physic::Backend::Bullet::SimulatedEntity::applyForce(const vec3& force) {
    //stop if static
    if (m_type == BodyType::STATIC) {return;}

    //apply the force
    m_body->applyCentralForce(btVector3(force.x, force.y, force.z));
}
void GLGE::Physic::Backend::Bullet::SimulatedEntity::applyImpulse(const vec3& impulse) {
    //stop if static
    if (m_type == BodyType::STATIC) {return;}

    //apply the impulse
    m_body->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
}
void GLGE::Physic::Backend::Bullet::SimulatedEntity::applyTorque(const vec3& torque) {
    //stop if static
    if (m_type == BodyType::STATIC) {return;}

    //apply the torque
    m_body->applyTorque(btVector3(torque.x, torque.y, torque.z));
}

bool GLGE::Physic::Backend::Bullet::SimulatedEntity::isSleeping() const {
    //static is never sleeping
    if (m_type == BodyType::STATIC)
    {return false;}

    //get if the body is active
    return !m_body->isActive();
}
void GLGE::Physic::Backend::Bullet::SimulatedEntity::wakeUp() {
    //static can't wake up
    if (m_type == BodyType::STATIC)
    {return;}

    //wake the body up
    m_body->activate(true);
}
void GLGE::Physic::Backend::Bullet::SimulatedEntity::sleep() {
    //static can't sleep
    if (m_type == BodyType::STATIC)
    {return;}

    //let the body sleep
    m_body->updateDeactivation(m_body->getDeactivationTime());
}

void GLGE::Physic::Backend::Bullet::SimulatedEntity::flushState() {
    //check if the collider is dirty
    if (isDirty(DirtyFlags::COLLIDER)) {
        //store the shape
        btCollisionShape* shape = nullptr;
        //extract the shape
        switch (m_collider->getType()) {
        case Collider::Type::SPHERE:
            shape = static_cast<Bullet::SphereCollider*>(m_collider.get())->getShape();
            break;
        case Collider::Type::BOX:
            shape = static_cast<Bullet::BoxCollider*>(m_collider.get())->getShape();
            break;
        
        default:
            break;
        }
        //set the shape
        m_body->setCollisionShape(shape);
    }

    //check if the transform is dirty
    if (isDirty(DirtyFlags::TRANSFORM)) {
        //set the transform
        btTransform transf;
        transf.setOrigin(btVector3(m_transf.pos.x, m_transf.pos.y, m_transf.pos.z));
        transf.setRotation(btQuaternion(m_transf.rot.x, m_transf.rot.y, m_transf.rot.z, m_transf.rot.w));
        m_body->setWorldTransform(transf);
    }

    //check if the properties are out of date
    if (isDirty(DirtyFlags::PROPERTIES)) {
        //update the mass
        m_body->setMassProps(m_mass, btVector3());
        //update the friction
        m_body->setFriction(m_friction);
        //set the restitution
        m_body->setRestitution(m_restitution);
    }

    //check if the user data is out of date
    if (isDirty(DirtyFlags::USER_DATA)) {
        //set the user data
        m_body->setUserPointer(m_userData);
    }

    //clear dirty flags
    clearDirtyFlags();
}