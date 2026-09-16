/**
 * @file Description.cpp
 * @author DM8AT
 * @brief implement 
 * @version 0.1
 * @date 2026-06-03
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//include the description
#include "Physics/Backend/Builtin/Bullet/Description.h"
//include the backend implementations
#include "Physics/Backend/Builtin/Bullet/Collider.h"
#include "Physics/Backend/Builtin/Bullet/Instance.h"
#include "Physics/Backend/Builtin/Bullet/World.h"

GLGE::Physic::Builtin::Bullet::Bullet() {
    //store the name
    m_name = "BULLET_BUILTIN";
    //store the version
    m_version = Version(0, 1, 0);
    //store the substep count
    m_substeps = 10;
}

GLGE::Reference<GLGE::Physic::Backend::Instance> GLGE::Physic::Builtin::Bullet::createInstance(Physic::Instance* instance) 
{return Reference<Backend::Instance>(new GLGE::Physic::Backend::Bullet::Instance(instance), false);}

GLGE::Reference<GLGE::Physic::Backend::SphereCollider> GLGE::Physic::Builtin::Bullet::createSphereCollider(float radius) 
{return Reference<Backend::SphereCollider>(new GLGE::Physic::Backend::Bullet::SphereCollider(radius), false);}

GLGE::Reference<GLGE::Physic::Backend::BoxCollider> GLGE::Physic::Builtin::Bullet::createBoxCollider(const vec3& halfExtent)
{return Reference<Backend::BoxCollider>(new GLGE::Physic::Backend::Bullet::BoxCollider(halfExtent), false);}

GLGE::Reference<GLGE::Physic::Backend::World> GLGE::Physic::Builtin::Bullet::createWorld(GLGE::Physic::Backend::Instance *instance, GLGE::World &world, const vec3& gravity, u64 maxBodies, u64 tempAllocatorSize) 
{return Reference<Backend::World>(new GLGE::Physic::Backend::Bullet::World(instance, world, gravity, maxBodies, tempAllocatorSize), false);}