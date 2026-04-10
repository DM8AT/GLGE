/**
 * @file SampledTexture.cpp
 * @author DM8AT
 * @brief implement the OpenGL sampled texture overload
 * @version 0.1
 * @date 2026-03-22
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the sampled texture backend
#include "Graphic/Backend/Builtin/Graphics/OpenGL/SampledTexture.h"

//add OpenGL resources
#include "Graphic/Backend/Builtin/Graphics/OpenGL/ResourceSet.h"
//add resource sets
#include "Graphic/ResourceSet.h"

//add the sampler, texture and image backend
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Texture.h"
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Image.h"
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Sampler.h"

//add OpenGL
#include "glad/glad.h"

void GLGE::Graphic::Backend::Graphic::OpenGL::SampledTexture::onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) {
    //store the binding function
    static void (*func)(u32, void*) = [](u32 unit, void* _self) {
        auto* self = reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::SampledTexture*>(_self);
        //get the handle
        u32 handle = 0;
        if (self->m_isTexture)
        {glBindTextureUnit(unit, reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Texture*>(self->m_data.texture.get())->getHandle());}
        else
        {glBindTextureUnit(unit, reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Image*>(self->m_data.image.get())->getHandle());}
        //the sampler to the unit
        glBindSampler(unit, reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Sampler*>(self->m_sampler.get())->getSampler());
    };
    //register the binding function
    reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::ResourceSet*>(set->getBackend().get())->registerBinder(func, this, unit);
}