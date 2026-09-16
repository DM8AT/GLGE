/**
 * @file SampledTexture.cpp
 * @author DM8AT
 * @brief implement the vulkan sampled texture
 * @version 0.1
 * @date 2026-05-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add sampled textures
#include "Graphic/Backend/Builtin/Graphics/Vulkan/SampledTexture.h"

void GLGE::Graphic::Backend::Graphic::Vulkan::SampledTexture::onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) {
    //parse the binding
    m_sampler->onBuildBinding(set, unit);
    if (m_isTexture)
    {m_data.texture->onBuildBinding(set, unit);}
    else
    {m_data.image->onBuildBinding(set, unit);}
}

void GLGE::Graphic::Backend::Graphic::Vulkan::SampledTexture::onDropBinding(GLGE::Graphic::ResourceSet* set) {
    //parse the un-binding
    m_sampler->onRemoveBinding(set);
    if (m_isTexture)
    {m_data.texture->onRemoveBinding(set);}
    else
    {m_data.image->onDropBinding(set);}
}