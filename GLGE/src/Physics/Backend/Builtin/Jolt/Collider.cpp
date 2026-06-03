/**
 * @file Collider.cpp
 * @author DM8AT
 * @brief implement the collider for jolt
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the jolt backend
#include "Physics/Backend/Builtin/Jolt/Collider.h"

GLGE::Physic::Backend::Jolt::SphereCollider::SphereCollider(float radius) 
 : GLGE::Physic::Backend::SphereCollider(radius), m_sphere(new JPH::SphereShape(radius))
{}

GLGE::Physic::Backend::Jolt::BoxCollider::BoxCollider(const vec3& halfExtent) 
 : GLGE::Physic::Backend::BoxCollider(halfExtent), m_box(new JPH::BoxShape(JPH::Vec3Arg(
    m_halfExtent.x, m_halfExtent.y, m_halfExtent.z
 )))
{}