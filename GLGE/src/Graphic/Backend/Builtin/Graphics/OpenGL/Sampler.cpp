/**
 * @file Sampler.cpp
 * @author DM8AT
 * @brief implement the OpenGL sampler overload
 * @version 0.1
 * @date 2026-03-11
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the sampler
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Sampler.h"
//add the instance
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Instance.h"

//add the mapping
#include "__Mapper.h"

//add the OpenGL resource set
#include "Graphic/Backend/Builtin/Graphics/OpenGL/ResourceSet.h"

GLGE::Graphic::Backend::Graphic::OpenGL::Sampler::Sampler(const SamplerCPU& state, GLGE::Graphic::Backend::Graphic::Instance* instance) 
 : GLGE::Graphic::Backend::Graphic::Sampler(state, instance)
{
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Sampler::Sampler");

    //create a new sampler
    glCreateSamplers(1, &m_sampler);
    //label the object in debug
    #if GLGE_DEBUG
    std::string name = std::string("Sampler_") + std::to_string(m_name);
    glObjectLabel(GL_SAMPLER, m_sampler, name.length(), name.c_str());
    #endif
    //update to initial state
    update();
}

GLGE::Graphic::Backend::Graphic::OpenGL::Sampler::~Sampler() {
    //when the sampler exists delete it
    if (m_sampler)
    {glDeleteSamplers(1, &m_sampler);}
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Sampler::update() {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Sampler::update");

    //set the interpolation mode
    glSamplerParameteri(m_sampler, GL_TEXTURE_MIN_FILTER, m_state.getMipChainEnabled()
         ? __translate_filter_with_mip(m_state.getFilterMinification(), m_state.getFilterMipChain())
         : __translate_filter(m_state.getFilterMinification()));
    glSamplerParameteri(m_sampler, GL_TEXTURE_MAG_FILTER, __translate_filter(m_state.getFilterMagnification()));
    //set the wrapping behaviour
    glSamplerParameteri(m_sampler, GL_TEXTURE_WRAP_S, __translate_wrap(m_state.getWrap()));
    glSamplerParameteri(m_sampler, GL_TEXTURE_WRAP_T, __translate_wrap(m_state.getWrap()));
    glSamplerParameteri(m_sampler, GL_TEXTURE_WRAP_R, __translate_wrap(m_state.getWrap()));

    //set potential anisotropy
    glSamplerParameterf(m_sampler, GL_TEXTURE_MAX_ANISOTROPY, glm::min<float>(m_state.getAnisotropy()+1, m_instance->getExtensionInfo().anisotropic.maxAnisotropy));
    //set the minimum and maximum level of detail
    glSamplerParameterf(m_sampler, GL_TEXTURE_MIN_LOD, m_state.getLodMinimum());
    glSamplerParameterf(m_sampler, GL_TEXTURE_MAX_LOD, m_state.getLodMaximum());
    //potentially set the compare function
    glSamplerParameteri(m_sampler, GL_TEXTURE_COMPARE_MODE, m_state.getCompareOpEnabled() ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE);
    glSamplerParameteri(m_sampler, GL_TEXTURE_COMPARE_FUNC, __translate_compare_op(m_state.getCompareOp()));
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Sampler::onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) {
    //store the binding function
    static void (*func)(u32, void*) = [](u32 unit, void* self) {
        //get the sampler
        u32 sampler = reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Sampler*>(self)->getSampler();
        //bind to an unit
        glBindSampler(unit, sampler);
    };
    //register the binding function
    reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::ResourceSet*>(set->getBackend().get())->registerBinder(func, this, unit);
}