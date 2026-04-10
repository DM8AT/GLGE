/**
 * @file Object.cpp
 * @author DM8AT
 * @brief implement the object component functionality
 * @version 0.1
 * @date 2026-03-11
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add objects
#include "Core/Object.h"

//IMPLEMENT NAME COMPONENT FUNCTIONS

void GLGE::Component::Name::store(std::vector<u8>& buffer, World* world) {
    //fetch the position to start storing at
    size_t offs = buffer.size();
    //get the name string
    const std::string& n = world->m_nameIdMap.at(name);
    //make enough space in the vector (4 bytes for u32 size)
    buffer.resize(buffer.size() + n.size() + 4);
    //store the raw binary string in the buffer, prefixed by the length
    u32 size = n.size();
    memcpy(buffer.data() + offs, &size, sizeof(size));
    memcpy(buffer.data() + 4 + offs, n.data(), size);
}

GLGE::u64 GLGE::Component::Name::load(Tiny::ECS::Entity entity, const std::vector<u8>& buffer, u64 offset, World* world) {
    //load the size of the string to read
    u32 size = 0;
    memcpy(&size, buffer.data() + offset, sizeof(size));
    offset += 4;
    //prepare the name string to hold the data
    std::string n;
    n.resize(size);
    //load the name
    memcpy(n.data(), buffer.data() + offset, size);

    //store the name mapping
    name = world->getNameIndex(n);

    //return the amount of read bytes
    return 4 + size;
}

// IMPLEMENT HIERARCHY NODE FUNCTIONS

void GLGE::Component::HierarchyNode::store(std::vector<u8>& buffer, World* world) {
    //make enough space in the vector (16 bytes for 4 entities)
    buffer.resize(buffer.size() + sizeof(Tiny::ECS::Entity)*4);
    //store the raw IDs in the buffer
    memcpy(buffer.data() + buffer.size(), this, sizeof(Tiny::ECS::Entity)*4);
}

GLGE::u64 GLGE::Component::HierarchyNode::load(Tiny::ECS::Entity entity, const std::vector<u8>& buffer, u64 offset, World* world) {
    //read the data
    memcpy(this, buffer.data() + offset, sizeof(Tiny::ECS::Entity)*4);
    //return the amount of read bytes
    return sizeof(Tiny::ECS::Entity)*4;
}