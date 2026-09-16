/**
 * @file ResourceSet.cpp
 * @author DM8AT
 * @brief implement the resource set
 * @version 0.1
 * @date 2026-05-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add resource sets
#include "Graphic/Backend/Builtin/Graphics/Vulkan/ResourceSet.h"
//add vulkan instances
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"
//add vulkan command buffers
#include "Graphic/Backend/Builtin/Graphics/Vulkan/CommandBuffer.h"
//add the resource set frontend
#include "Graphic/ResourceSet.h"

//add frontend images
#include "Graphic/Image.h"
//add vulkan images
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Image.h"
//add vulkan textures
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Texture.h"
//add vulkan samplers
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Sampler.h"
//get frontend buffers
#include "Graphic/Buffer.h"
//add vulkan buffers
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Buffer.h"
//add sampled textures
#include "Graphic/SampledTexture.h"

//add vulkan
#include "vulkan/vulkan.h"

GLGE::Graphic::Backend::Graphic::Vulkan::ResourceSet::ResourceSet(GLGE::Graphic::ResourceSet* set) 
 : GLGE::Graphic::Backend::Graphic::ResourceSet(set)
{
    //get the instance
    auto* inst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(set->getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicBackendInstance().get());

    //store how many of what type exist
    std::unordered_map<VkDescriptorType, u32> descriptorCounts;

    //build the descriptor set layout

    //get the resource template
    const auto& presets = m_resourceSet->getStructure();

    //store all actual bindings
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bindings.reserve(presets.size());
    std::vector<VkDescriptorBindingFlags> flags;
    flags.reserve(presets.size());

    //iterate over all presets and create the bindings
    for (const auto& p : presets) {
        //create the descriptor set layout binding
        VkDescriptorSetLayoutBinding b {};
        b.binding = p.getUnit();
        b.descriptorCount = 1;

        //translate the type of preset
        switch (m_resourceSet->resources()[p.getUnit()]->getType()) {
        case GLGE::Graphic::ResourceType::UNIFORM_BUFFER:
            b.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            break;
        case GLGE::Graphic::ResourceType::STORAGE_BUFFER:
            b.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            break;
        case GLGE::Graphic::ResourceType::IMAGE:
            b.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            break;
        case GLGE::Graphic::ResourceType::SAMPLED_TEXTURE:
            b.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            break;
        
        default:
            //error
            throw Exception("Failed to map a preset type", "GLGE::Graphic::Backend::Graphic::Vulkan::ResourceSet::ResourceSet");
            break;
        }

        //all stages are active
        b.stageFlags = VK_SHADER_STAGE_ALL;

        //store the binding
        bindings.push_back(b);
        flags.push_back(VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);

        //count up
        ++descriptorCounts[b.descriptorType];
    }

    //create the binding flags
    VkDescriptorSetLayoutBindingFlagsCreateInfo descLayoutBindingFlagCreate {};
    descLayoutBindingFlagCreate.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    descLayoutBindingFlagCreate.bindingCount = flags.size();
    descLayoutBindingFlagCreate.pBindingFlags = flags.data();

    //create the descriptor set layout
    VkDescriptorSetLayoutCreateInfo layoutCreate {};
    layoutCreate.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutCreate.bindingCount = bindings.size();
    layoutCreate.pBindings = bindings.data();
    layoutCreate.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
    layoutCreate.pNext = &descLayoutBindingFlagCreate;
    VkDescriptorSetLayout layout;
    if (vkCreateDescriptorSetLayout(reinterpret_cast<VkDevice>(inst->getDevice()), &layoutCreate, nullptr, &layout) != VK_SUCCESS)
    {throw Exception("Failed to create a descriptor set layout", "GLGE::Graphic::Backend::Graphic::Vulkan::ResourceSet::ResourceSet");}
    //store the layout
    m_layout = reinterpret_cast<void*>(layout);

    //create an exactly fitting descriptor pool
    std::vector<VkDescriptorPoolSize> poolSizes;
    for (const auto& [type, count] : descriptorCounts) {
        poolSizes.push_back(VkDescriptorPoolSize{
            .type = type,
            .descriptorCount = count
        });
    }
    VkDescriptorPoolCreateInfo poolCreate {};
    poolCreate.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreate.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT | VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
    poolCreate.maxSets = 1;
    poolCreate.poolSizeCount = poolSizes.size();
    poolCreate.pPoolSizes = poolSizes.data();
    VkDescriptorPool pool;
    if (vkCreateDescriptorPool(reinterpret_cast<VkDevice>(inst->getDevice()), &poolCreate, nullptr, &pool) != VK_SUCCESS)
    {throw Exception("Failed to create a descriptor pool", "GLGE::Graphic::Backend::Graphic::Vulkan::ResourceSet::ResourceSet");}
    //store the pool
    m_pool = reinterpret_cast<void*>(pool);

    //allocate the descriptor set
    VkDescriptorSetAllocateInfo setAlloc {};
    setAlloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAlloc.descriptorPool = pool;
    setAlloc.descriptorSetCount = 1;
    setAlloc.pSetLayouts = &layout;
    VkDescriptorSet vkSet;
    if (vkAllocateDescriptorSets(reinterpret_cast<VkDevice>(inst->getDevice()), &setAlloc, reinterpret_cast<VkDescriptorSet*>(&vkSet)) != VK_SUCCESS)
    {throw Exception("Failed to allocate a descriptor set", "GLGE::Graphic::Backend::Graphic::Vulkan::ResourceSet::ResourceSet");}
    //store the descriptor set
    m_set = reinterpret_cast<void*>(vkSet);

    //fill the descriptor set
    std::vector<VkWriteDescriptorSet> writes;
    writes.reserve(m_resourceSet->getStructure().size());
    std::vector<VkDescriptorImageInfo> imgInfos;
    std::vector<VkDescriptorBufferInfo> buffInfos;
    u32 imgInfoCount = 0;
    u32 buffInfoCount = 0;
    for (const auto& b : m_resourceSet->getStructure()) {
        //switch over the resource type and sum up what is required
        switch (m_resourceSet->resources()[b.getUnit()]->getType()) {
        case GLGE::Graphic::ResourceType::IMAGE: imgInfoCount++; break;
        case GLGE::Graphic::ResourceType::SAMPLED_TEXTURE: imgInfoCount++; break;
        case GLGE::Graphic::ResourceType::STORAGE_BUFFER: buffInfoCount++; break;
        case GLGE::Graphic::ResourceType::UNIFORM_BUFFER: buffInfoCount++; break;
        
        default:
            break;
        }
    }
    //exact reserve required
    imgInfos.reserve(imgInfoCount);
    buffInfos.reserve(buffInfoCount);
    for (const auto& b : m_resourceSet->getStructure()) {
        //switch over the resource type
        switch (m_resourceSet->resources()[b.getUnit()]->getType())
        {
        case GLGE::Graphic::ResourceType::IMAGE: {
                //store information about the image
                VkDescriptorImageInfo info {};
                info.sampler = VK_NULL_HANDLE;
                info.imageView = reinterpret_cast<VkImageView>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Image*>(static_cast<GLGE::Graphic::Image*>(m_resourceSet->resources()[b.getUnit()])->getBackend().get())->getView());
                info.imageLayout = VK_IMAGE_LAYOUT_GENERAL; //this MUST be general as of the vulkan spec
                imgInfos.push_back(info);

                //build the write call
                VkWriteDescriptorSet write {};
                write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstSet = vkSet;
                write.dstBinding = b.getUnit();
                write.descriptorCount = 1;
                write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                write.pImageInfo = &imgInfos.back();
                writes.push_back(write);
            }
            break;
        case GLGE::Graphic::ResourceType::SAMPLED_TEXTURE: {
                auto* sampledTex = static_cast<GLGE::Graphic::SampledTexture*>(m_resourceSet->resources()[b.getUnit()]);
                VkImageView view;
                if (sampledTex->getBackend()->isTexture()) 
                {view = reinterpret_cast<VkImageView>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Texture*>(sampledTex->getBackend()->getData().texture.get())->getView());}
                else
                {view = reinterpret_cast<VkImageView>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Image*>(sampledTex->getBackend()->getData().image.get())->getView());}
                //create the image info
                VkDescriptorImageInfo imgInfo {};
                imgInfo.sampler = reinterpret_cast<VkSampler>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Sampler*>(sampledTex->getBackend()->getSampler().get())->getSampler());
                imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                imgInfo.imageView = view;
                imgInfos.push_back(imgInfo);
                //create the write info
                VkWriteDescriptorSet write {};
                write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstBinding = b.getUnit();
                write.dstSet = vkSet;
                write.descriptorCount = 1;
                write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                write.pImageInfo = &imgInfos.back();
                writes.push_back(write);
            }
            break;
        case GLGE::Graphic::ResourceType::STORAGE_BUFFER: {
                //get the buffer
                auto* buff = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Buffer*>(static_cast<GLGE::Graphic::Buffer*>(m_resourceSet->resources()[b.getUnit()])->getBackendReference().get());
                //store information about the buffer
                VkDescriptorBufferInfo info {};
                info.offset = 0;
                info.range = buff->getSize();
                info.buffer = reinterpret_cast<VkBuffer>(buff->getBuffer());
                buffInfos.push_back(info);

                //build the write call
                VkWriteDescriptorSet write {};
                write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstSet = vkSet;
                write.dstBinding = b.getUnit();
                write.descriptorCount = 1;
                write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                write.pBufferInfo = &buffInfos.back();
                writes.push_back(write);
            }
            break;
        case GLGE::Graphic::ResourceType::UNIFORM_BUFFER: {
                //get the buffer
                auto* buff = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Buffer*>(static_cast<GLGE::Graphic::Buffer*>(m_resourceSet->resources()[b.getUnit()])->getBackendReference().get());
                //store information about the buffer
                VkDescriptorBufferInfo info {};
                info.offset = 0;
                info.range = buff->getSize();
                info.buffer = reinterpret_cast<VkBuffer>(buff->getBuffer());
                buffInfos.push_back(info);

                //build the write call
                VkWriteDescriptorSet write {};
                write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstSet = vkSet;
                write.dstBinding = b.getUnit();
                write.descriptorCount = 1;
                write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                write.pBufferInfo = &buffInfos.back();
                writes.push_back(write);
            }
            break;
        
        default:
            //unsupported type error
            throw Exception("Found an unsupported descriptor type", "GLGE::Graphic::Backend::Graphic::Vulkan::ResourceSet::ResourceSet");
            break;
        }
    }

    //update everything
    if (!writes.empty())
    {vkUpdateDescriptorSets(reinterpret_cast<VkDevice>(inst->getDevice()), writes.size(), writes.data(), 0, nullptr);}
}

GLGE::Graphic::Backend::Graphic::Vulkan::ResourceSet::~ResourceSet() {
    //get the instance
    auto* inst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_resourceSet->getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicBackendInstance().get());

    //remove the descriptor pool
    vkDestroyDescriptorPool(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkDescriptorPool>(m_pool), nullptr);
    //clean up
    vkDestroyDescriptorSetLayout(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkDescriptorSetLayout>(m_layout), nullptr);
}