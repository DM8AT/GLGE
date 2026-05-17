/**
 * @file Sampler.cpp
 * @author DM8AT
 * @brief implement the vulkan sampler
 * @version 0.1
 * @date 2026-05-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add vulkan samplers
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Sampler.h"
//add vulkan instances
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"
//add resource set backends
#include "Graphic/Backend/Builtin/Graphics/Vulkan/ResourceSet.h"

//add textures and images
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Texture.h"
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Image.h"

//add references
#include "Graphic/ResourceSet.h"

//add texture samplers
#include "Graphic/SampledTexture.h"

//add shared stuff
#include "Shared.h"

static VkFilter __translateFilter(GLGE::Graphic::SamplerCPU::Filter filter) {
    switch (filter) {
    case GLGE::Graphic::SamplerCPU::FILTER_LINEAR:
        return VK_FILTER_LINEAR;
        break;
    case GLGE::Graphic::SamplerCPU::FILTER_NEAREST:
        return VK_FILTER_NEAREST;
        break;
    
    default:
        return VK_FILTER_NEAREST;
        break;
    }
}

static VkSamplerAddressMode __translateWrap(GLGE::Graphic::SamplerCPU::Wrap wrap) {
    switch (wrap) {
    case GLGE::Graphic::SamplerCPU::WRAP_REPEAT:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        break;
    case GLGE::Graphic::SamplerCPU::WRAP_MIRROR_REPEAT:
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        break;
    case GLGE::Graphic::SamplerCPU::WRAP_CLAMP_TO_EDGE:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        break;
    case GLGE::Graphic::SamplerCPU::WRAP_CLAMP_TO_BLACK:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        break;
    
    default:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        break;
    }
}

GLGE::Graphic::Backend::Graphic::Vulkan::Sampler::Sampler(const SamplerCPU& state, GLGE::Graphic::Backend::Graphic::Instance* instance) 
 : GLGE::Graphic::Backend::Graphic::Sampler(state, instance)
{
    //get the instance
    auto* inst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);

    //create the vulkan sampler
    VkSamplerCreateInfo samplerCreate {};
    samplerCreate.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreate.magFilter = __translateFilter(m_state.getFilterMagnification());
    samplerCreate.minFilter = __translateFilter(m_state.getFilterMinification());
    samplerCreate.addressModeU = __translateWrap(m_state.getWrap());
    samplerCreate.addressModeV = __translateWrap(m_state.getWrap());
    samplerCreate.addressModeW = __translateWrap(m_state.getWrap());
    samplerCreate.anisotropyEnable = inst->getExtensionInfo().anisotropic.supported ? (u8(m_state.getAnisotropy()+1) > 0) : VK_FALSE;
    samplerCreate.maxAnisotropy = glm::min(samplerCreate.anisotropyEnable ? f32(u8(m_state.getAnisotropy()+1)) : 0.f, inst->getExtensionInfo().anisotropic.maxAnisotropy);
    samplerCreate.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK; //vec4(0,0,0,0)
    samplerCreate.unnormalizedCoordinates = VK_FALSE;
    VkSampler sampler;
    if (vkCreateSampler(reinterpret_cast<VkDevice>(inst->getDevice()), &samplerCreate, nullptr, &sampler) != VK_SUCCESS)
    {throw Exception("Failed to create a sampler", "GLGE::Graphic::Backend::Graphic::Vulkan::Sampler::Sampler");}
    //store the sampler
    m_sampler = reinterpret_cast<void*>(sampler);
}

GLGE::Graphic::Backend::Graphic::Vulkan::Sampler::~Sampler() {
    //get the instance
    auto* inst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);
    //clean up
    vkDestroySampler(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkSampler>(m_sampler), nullptr);
    if (m_oldSampler)
    {vkDestroySampler(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkSampler>(m_oldSampler), nullptr);}
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Sampler::update() {
    //get the instance
    auto* inst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);

    //create a new sampler with the settings
    VkSamplerCreateInfo samplerCreate {};
    samplerCreate.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreate.magFilter = __translateFilter(m_state.getFilterMagnification());
    samplerCreate.minFilter = __translateFilter(m_state.getFilterMinification());
    samplerCreate.addressModeU = __translateWrap(m_state.getWrap());
    samplerCreate.addressModeV = __translateWrap(m_state.getWrap());
    samplerCreate.addressModeW = __translateWrap(m_state.getWrap());
    samplerCreate.anisotropyEnable = inst->getExtensionInfo().anisotropic.supported ? (u8(m_state.getAnisotropy()+1) > 0) : VK_FALSE;
    samplerCreate.maxAnisotropy = glm::min(samplerCreate.anisotropyEnable ? f32(u8(m_state.getAnisotropy()+1)) : 0.f, inst->getExtensionInfo().anisotropic.maxAnisotropy);
    samplerCreate.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK; //also called "true black" -> vec4(0,0,0,0)
    samplerCreate.unnormalizedCoordinates = VK_FALSE;
    VkSampler sampler;
    if (vkCreateSampler(reinterpret_cast<VkDevice>(inst->getDevice()), &samplerCreate, nullptr, &sampler) != VK_SUCCESS)
    {throw Exception("Failed to create a sampler", "GLGE::Graphic::Backend::Graphic::Vulkan::Sampler::update");}
    
    //update sampler storage
    if (m_oldSampler)
    {vkDestroySampler(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkSampler>(m_oldSampler), nullptr);}
    m_oldSampler = m_sampler;
    m_sampler = reinterpret_cast<void*>(sampler);

    //store the writes and image infos
    std::vector<VkDescriptorImageInfo> infos;
    infos.reserve(m_bindings.size());
    std::vector<VkWriteDescriptorSet> writes;
    writes.reserve(m_bindings.size());
    //update all bindings
    for (size_t i = 0; i < m_bindings.size(); ++i) {
        const auto& bind = m_bindings[i];
        auto* sampledTex = static_cast<GLGE::Graphic::SampledTexture*>((bind.first->resources())[bind.second]);
        VkImageView view;
        if (sampledTex->getBackend()->isTexture()) 
        {view = reinterpret_cast<VkImageView>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Texture*>(sampledTex->getBackend()->getData().texture.get())->getView());}
        else
        {view = reinterpret_cast<VkImageView>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Image*>(sampledTex->getBackend()->getData().image.get())->getView());}
        //create the image info
        VkDescriptorImageInfo imgInfo {};
        imgInfo.sampler = reinterpret_cast<VkSampler>(m_sampler);
        imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        imgInfo.imageView = view;
        infos.push_back(imgInfo);
        //create the write info
        VkWriteDescriptorSet write {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstBinding = bind.second;
        write.dstSet = reinterpret_cast<VkDescriptorSet>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::ResourceSet*>(bind.first->getBackend().get())->getDescriptorSet());
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.pImageInfo = &infos.back();
        writes.push_back(write);
    }
    //write everything
    vkUpdateDescriptorSets(reinterpret_cast<VkDevice>(inst->getDevice()), writes.size(), writes.data(), 0, nullptr);
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Sampler::onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) {
    //add the binding
    m_bindings.emplace_back(set, unit);
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Sampler::onRemoveBinding(GLGE::Graphic::ResourceSet* set) {
    //remove the unit
    for (size_t i = 0; i < m_bindings.size(); ++i)
    {if (m_bindings[i].first == set) {m_bindings.erase(m_bindings.begin() + i);}}
}