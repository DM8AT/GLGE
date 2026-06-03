/**
 * @file Instance.cpp
 * @author DM8AT
 * @brief implement the physics instance
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the physics instance
#include "Physics/Instance.h"

GLGE::Physic::Instance::Instance(Backend::Description* description)
 : m_descr(description)
{
    //create the instance
    m_inst = m_descr->createInstance(this);
}

GLGE::Physic::Instance::~Instance() {
    
}

void GLGE::Physic::Instance::onStart() {
    //start the physic instance
    m_inst->start();
}

void GLGE::Physic::Instance::onUpdate() {

}

void GLGE::Physic::Instance::onShutdown() {
    //end the physic instance (do NOT destroy)
    m_inst->end();
}

void GLGE::Physic::Instance::onInstanceSetting() {

}

void GLGE::Physic::Instance::onBind() {

}

void GLGE::Physic::Instance::onUnbind() {

}