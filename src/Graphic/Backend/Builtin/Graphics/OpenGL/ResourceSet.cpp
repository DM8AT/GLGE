/**
 * @file ResourceSet.cpp
 * @author DM8AT
 * @brief implement the resource set for OpenGL
 * @version 0.1
 * @date 2026-03-15
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the resource set backend
#include "Graphic/Backend/Builtin/Graphics/OpenGL/ResourceSet.h"

GLGE::Graphic::Backend::Graphic::OpenGL::ResourceSet::ResourceSet(GLGE::Graphic::ResourceSet* set) 
 : GLGE::Graphic::Backend::Graphic::ResourceSet(set)
{}

void GLGE::Graphic::Backend::Graphic::OpenGL::ResourceSet::bind(CommandBuffer* buffer) {
    //the callback to call
    void (*call)(GLGE::Graphic::Backend::Graphic::OpenGL::ResourceSet*) = [](GLGE::Graphic::Backend::Graphic::OpenGL::ResourceSet* self) {
        //iterate and call all bind functions
        for (const auto& [binder, resource] : self->m_binders) 
        {binder(resource.second, resource.first);}
    };
    //add the backend function
    buffer->addCommand(call, this);
}