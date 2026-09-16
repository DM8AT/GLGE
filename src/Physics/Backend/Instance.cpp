/**
 * @file Instance.cpp
 * @author DM8AT
 * @brief implement the base functionality for backend instances
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add backend instances
#include "Physics/Backend/Instance.h"
//add physic worlds
#include "Physics/Backend/World.h"

GLGE::Physic::Backend::Instance::Instance(Physic::Instance* instance) 
 : m_instance(instance)
{
}

GLGE::Physic::Backend::Instance::~Instance() {
}

void GLGE::Physic::Backend::Instance::start() {
    //start the update thread
    m_alive.store(true, std::memory_order_release);
    m_physicsThread = std::thread(&Instance::async, this);
}

void GLGE::Physic::Backend::Instance::end() {
    //stop the physic thread
    m_alive.store(false, std::memory_order_release);
    m_physicsThread.join();
}

void GLGE::Physic::Backend::Instance::async() {
    //loop while alive
    while (m_alive.load(std::memory_order_acquire)) {
        //limit iteration rate
        m_limiter.startTick();

        //call the update function
        onUpdate();

        //iterate over all worlds and call the onUpdate hook
        for (auto& world : m_worlds)
        {world->onUpdate();}

        //end the tick
        m_limiter.endTick();
    }
}


void GLGE::Physic::Backend::Instance::registerWorld(World* world) {
    //call the virtual hook
    onNewWorld(world);
    //just add the world
    m_worlds.push_back(world);
}

void GLGE::Physic::Backend::Instance::removeWorld(World* world) {
    //search the world in the list
    for (size_t i = 0; i < m_worlds.size(); ++i) {
        if (m_worlds[i] == world) {
            //call the virtual hook
            onDeleteWorld(world);
            //then, erase the element
            m_worlds.erase(m_worlds.begin() + i);
        }
    }
}