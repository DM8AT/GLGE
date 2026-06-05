/**
 * @file Instance.cpp
 * @author DM8AT
 * @brief implement the bullet instance
 * @version 0.1
 * @date 2026-06-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the instance
#include "Physics/Backend/Builtin/Bullet/Instance.h"


GLGE::Physic::Backend::Bullet::Instance::Instance(GLGE::Physic::Instance* instance) 
 : Backend::Instance(instance)
{
}

GLGE::Physic::Backend::Bullet::Instance::~Instance()
{
}

void GLGE::Physic::Backend::Bullet::Instance::onUpdate()
{
}