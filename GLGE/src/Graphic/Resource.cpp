/**
 * @file Resource.cpp
 * @author DM8AT
 * @brief implement the resource functions that need a resource set to work
 * @version 0.1
 * @date 2026-08-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the headers
#include "Graphic/Resource.h"
#include "Graphic/ResourceSet.h"

void GLGE::Graphic::Resource::buildBinding(ResourceSet* set, u32 unit) {
    //add as a child
    attachInvalidator(*static_cast<CommandInvalidator*>(set));
    //call the hook
    onBuildBinding(set, unit);
}

void GLGE::Graphic::Resource::removeBinding(ResourceSet* set) {
    //remove the child
    detachInvalidator(*static_cast<CommandInvalidator*>(set));
    //call the hook
    onRemoveBinding(set);
}