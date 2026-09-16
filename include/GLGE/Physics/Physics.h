/**
 * @file Physics.h
 * @author DM8AT
 * @brief include everything physics related
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_PHYSICS_
#define _GLGE_PHYSICS_

//add physics instances
#include "Instance.h"
//add collider
#include "Collider.h"
//add worlds
#include "World.h"
//add rigid bodies
#include "RigidBody.h"

//add the builtins
#include "Backend/Builtin/Jolt/Description.h"
#include "Backend/Builtin/Bullet/Description.h"

#endif