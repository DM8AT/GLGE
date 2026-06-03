/**
 * @file SimulatedEntity.cpp
 * @author DM8AT
 * @brief implement the default functionality for the simulated entity
 * @version 0.1
 * @date 2026-06-02
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add simulated entities
#include "Physics/Backend/SimulatedEntity.h"

void GLGE::Physic::Backend::SimulatedEntity::setCollider(const Reference<Collider>& collider) {
    //store the new collider
    m_collider = collider;
    //mark the collider as dirty
    m_dirty = static_cast<DirtyFlags>(static_cast<u8>(m_dirty) | static_cast<u8>(DirtyFlags::COLLIDER));
}

void GLGE::Physic::Backend::SimulatedEntity::setMass(float mass) {
    //store the mass
    m_mass = mass;
    //mark the properties as dirty
    m_dirty = static_cast<DirtyFlags>(static_cast<u8>(m_dirty) | static_cast<u8>(DirtyFlags::PROPERTIES));
}

void GLGE::Physic::Backend::SimulatedEntity::setFriction(float friction) {
    //store the friction
    m_friction = friction;
    //mark the properties as dirty
    m_dirty = static_cast<DirtyFlags>(static_cast<u8>(m_dirty) | static_cast<u8>(DirtyFlags::PROPERTIES));
}

void GLGE::Physic::Backend::SimulatedEntity::setRestitution(float restitution) {
    //store the restitution
    m_restitution = restitution;
    //mark the properties as dirty
    m_dirty = static_cast<DirtyFlags>(static_cast<u8>(m_dirty) | static_cast<u8>(DirtyFlags::PROPERTIES));
}

void GLGE::Physic::Backend::SimulatedEntity::setFilter(const Filter& filter) {
    //store the filter
    m_filter = filter;
    //mark the properties as dirty
    m_dirty = static_cast<DirtyFlags>(static_cast<u8>(m_dirty) | static_cast<u8>(DirtyFlags::PROPERTIES));
}

void GLGE::Physic::Backend::SimulatedEntity::setBodyType(BodyType type) {
    //store the body type
    m_type = type;
    //mark the properties as dirty
    m_dirty = static_cast<DirtyFlags>(static_cast<u8>(m_dirty) | static_cast<u8>(DirtyFlags::PROPERTIES));
}

void GLGE::Physic::Backend::SimulatedEntity::setUserData(void* userData) {
    //store the user data
    m_userData = userData;
    //mark the user data as dirty
    m_dirty = static_cast<DirtyFlags>(static_cast<u8>(m_dirty) | static_cast<u8>(DirtyFlags::USER_DATA));
}

void GLGE::Physic::Backend::SimulatedEntity::setTransform(const Transform& transf) {
    //store the transform
    m_transf.pos = transf.pos;
    m_transf.rot = transf.rot;
    m_transf.scale = transf.scale;
    //mark transform as dirty
    m_dirty = static_cast<DirtyFlags>(static_cast<u8>(m_dirty) | static_cast<u8>(DirtyFlags::TRANSFORM));
}