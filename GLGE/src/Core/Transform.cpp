/**
 * @file Transform.cpp
 * @author DM8AT
 * @brief implement transform-related code
 * @version 0.1
 * @date 2026-05-31
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add transforms
#include "Core/Transform.h"

void GLGE::Transform::load(const std::span<const u8>& buffer) {
    //copy the data
    memcpy(&pos, buffer.data(), sizeof(pos));
    memcpy(&rot, buffer.data() + sizeof(pos), sizeof(rot));
    memcpy(&scale, buffer.data() + sizeof(pos) + sizeof(rot), sizeof(scale));
}

void GLGE::Transform::store(std::vector<u8>& buffer) {
    //add the data
    buffer.insert(buffer.end(), reinterpret_cast<u8*>(&pos), reinterpret_cast<u8*>(&pos) + sizeof(pos));
    buffer.insert(buffer.end(), reinterpret_cast<u8*>(&rot), reinterpret_cast<u8*>(&rot) + sizeof(rot));
    buffer.insert(buffer.end(), reinterpret_cast<u8*>(&scale), reinterpret_cast<u8*>(&scale) + sizeof(scale));
}

void GLGE::Transform2D::load(const std::span<const u8>& buffer) {
    //copy the data
    memcpy(&pos, buffer.data(), sizeof(pos));
    memcpy(&angle, buffer.data() + sizeof(angle), sizeof(angle));
    memcpy(&scale, buffer.data() + sizeof(pos) + sizeof(angle), sizeof(scale));
}

void GLGE::Transform2D::store(std::vector<u8>& buffer) {
    //add the data
    buffer.insert(buffer.end(), reinterpret_cast<u8*>(&pos), reinterpret_cast<u8*>(&pos) + sizeof(pos));
    buffer.insert(buffer.end(), reinterpret_cast<u8*>(&angle), reinterpret_cast<u8*>(&angle) + sizeof(angle));
    buffer.insert(buffer.end(), reinterpret_cast<u8*>(&scale), reinterpret_cast<u8*>(&scale) + sizeof(scale));
}

static void BakeChildren(GLGE::World& world, GLGE::Tiny::ECS::Entity entity, const GLGE::WorldTransform& parentGlobal) {
    //entity sanity check is the caller's job
    //this is OK because this is just a local function

    //get the local transform
    GLGE::Transform* lTransf = world.get<GLGE::Transform>(entity);
    //if a world transform exists, reuse it. Else, add it
    GLGE::WorldTransform* wTransf = world.get<GLGE::WorldTransform>(entity);
    if (!wTransf) {
        world.add<GLGE::WorldTransform>(entity);
        wTransf = world.get<GLGE::WorldTransform>(entity);
    }

    //compute the world transform (scale -> rot -> pos)
    //world Scale
    wTransf->scale = parentGlobal.scale * lTransf->scale;
    //world Rotation
    wTransf->rot = parentGlobal.rot * lTransf->rot;
    //world Position = ParentPos + (ParentRot * (ParentScale * LocalPos))
    wTransf->pos = parentGlobal.pos + (parentGlobal.rot * (parentGlobal.scale * lTransf->pos));

    //recurse to the children's children
    GLGE::Component::HierarchyNode* node = world.get<GLGE::Component::HierarchyNode>(entity);
    if (node->firstChild != GLGE::Tiny::ECS::Entity::getInvalid()) 
    {BakeChildren(world, node->firstChild, *wTransf);}

    //process next sibling
    if (node->nextSibling != GLGE::Tiny::ECS::Entity::getInvalid())
    {BakeChildren(world, node->nextSibling, *wTransf);}
}

void GLGE::System::BakeTransforms(World& world) {
    //get all objects with a transform and without a parent
    std::vector<Object> roots;
    world.each<Transform, Component::HierarchyNode>([&roots](const Object& obj, const Transform&, const Component::HierarchyNode& node){
        if (node.parent == Tiny::ECS::Entity::getInvalid())
        {roots.push_back(obj);}
    });

    //iterate over all the objects
    for (const auto& obj : roots) {
        //get the local transform
        Transform* lTransf = world.get<Transform>(obj);
        //if a world transform exists, reuse it. Else, add it
        WorldTransform* wTransf = world.get<WorldTransform>(obj);
        if (!wTransf) {
            world.add<WorldTransform>(obj);
            wTransf = world.get<WorldTransform>(obj);
        }
        //this is a root object, so just copy the data over
        wTransf->pos = lTransf->pos;
        wTransf->rot = lTransf->rot;
        wTransf->scale = lTransf->scale;

        //bake the children
        Component::HierarchyNode* node = world.get<Component::HierarchyNode>(obj);
        if (node->firstChild != Tiny::ECS::Entity::getInvalid())
        {BakeChildren(world, node->firstChild, *wTransf);}
    }
}