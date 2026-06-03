/**
 * @file Description.cpp
 * @author DM8AT
 * @brief implement the jolt description
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the jolt description
#include "Physics/Backend/Builtin/Jolt/Description.h"

//add all backend implementations
#include "Physics/Backend/Builtin/Jolt/Collider.h"
#include "Physics/Backend/Builtin/Jolt/Instance.h"
#include "Physics/Backend/Builtin/Jolt/World.h"

GLGE::Physic::Builtin::Jolt::Jolt() {
    //store the name
    m_name = "JOLT_BUILTIN";
    //store the version
    m_version = Version(0, 1, 0);
}

GLGE::Reference<GLGE::Physic::Backend::Instance> GLGE::Physic::Builtin::Jolt::createInstance(Physic::Instance* instance) 
{return Reference<GLGE::Physic::Backend::Instance>(new Backend::Jolt::Instance(instance), false);}

GLGE::Reference<GLGE::Physic::Backend::SphereCollider> GLGE::Physic::Builtin::Jolt::createSphereCollider(float radius) 
{return Reference<GLGE::Physic::Backend::SphereCollider>(new Backend::Jolt::SphereCollider(radius), false);}

GLGE::Reference<GLGE::Physic::Backend::BoxCollider> GLGE::Physic::Builtin::Jolt::createBoxCollider(const vec3& halfExtent) 
{return Reference<GLGE::Physic::Backend::BoxCollider>(new Backend::Jolt::BoxCollider(halfExtent), false);}

GLGE::Reference<GLGE::Physic::Backend::World> GLGE::Physic::Builtin::Jolt::createWorld(GLGE::Physic::Backend::Instance* instance, GLGE::World& world, const vec3& gravity, u64 maxBodies, u64 tempAllocatorSize)
{return Reference<GLGE::Physic::Backend::World>(new Backend::Jolt::World(instance, world, gravity, maxBodies, tempAllocatorSize), false);}