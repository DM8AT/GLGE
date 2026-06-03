/**
 * @file Instance.cpp
 * @author DM8AT
 * @brief implement the jolt instance
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add jolt instances
#include "Physics/Backend/Builtin/Jolt/Instance.h"

//add Jolt
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>

//add world backends
#include "Physics/Backend/Builtin/Jolt/World.h"

GLGE::Physic::Backend::Jolt::Instance::Instance(Physic::Instance* instance)
 : Backend::Instance(instance)
{
    //if it does not exist, create the jolt instance
    if (!JPH::Factory::sInstance) {
        JPH::RegisterDefaultAllocator();
        JPH::Factory::sInstance = new JPH::Factory();
        JPH::RegisterTypes();
    }
    //increment the reference count
    ++ms_refCount;
}

GLGE::Physic::Backend::Jolt::Instance::~Instance() {
    //decrement the reference count
    --ms_refCount;
    //if it hits 0, clean up
    if (ms_refCount == 0) {
        JPH::UnregisterTypes();
        delete JPH::Factory::sInstance;
        JPH::Factory::sInstance = nullptr;
    }
}

void GLGE::Physic::Backend::Jolt::Instance::onUpdate() {
    
}