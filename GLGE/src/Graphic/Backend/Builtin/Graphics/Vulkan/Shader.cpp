/**
 * @file Shader.cpp
 * @author DM8AT
 * @brief implement the vulkan shader class backend
 * @version 0.1
 * @date 2026-05-12
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the vulkan shader backend
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Shader.h"
//get the shader frontend
#include "Graphic/Shader.h"
//add vulkan instances
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"

//add vulkan
#include "vulkan/vulkan.h"

GLGE::Graphic::Backend::Graphic::Vulkan::Shader::Shader(GLGE::Graphic::Shader* frontend) 
 : GLGE::Graphic::Backend::Graphic::Shader(frontend)
{}

GLGE::Graphic::Backend::Graphic::Vulkan::Shader::~Shader() {
    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_shader->getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicBackendInstance().get());

    //if compute stuff exists, clean it
    if (m_computePipeLayout)
    {vkDestroyPipelineLayout(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkPipelineLayout>(m_computePipeLayout), nullptr);}
    if (m_computePipe)
    {vkDestroyPipeline(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkPipeline>(m_computePipe), nullptr);}
    //cleanup all descriptor sets
    for (size_t i = 0; i < m_layouts.size(); ++i)
    {vkDestroyDescriptorSetLayout(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkDescriptorSetLayout>(m_layouts[i]), nullptr);}
    //cleanup all shader modules
    for (size_t i = 0; i < m_modules.size(); ++i)
    {vkDestroyShaderModule(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkShaderModule>(m_modules[i].shaderModule), nullptr);}
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Shader::addShaderElement(void* data, u32 size, const void* element) {
    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_shader->getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicBackendInstance().get());
    //get shader element data
    const GLGE::Graphic::Shader::ElementInfo* el = reinterpret_cast<const GLGE::Graphic::Shader::ElementInfo*>(element);

    //create a shader module
    VkShaderModuleCreateInfo modCreateInfo {};
    modCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    modCreateInfo.codeSize = size;
    modCreateInfo.pCode = reinterpret_cast<u32*>(data);
    VkShaderModule mod;
    if (vkCreateShaderModule(reinterpret_cast<VkDevice>(inst->getDevice()), &modCreateInfo, nullptr, &mod) != VK_SUCCESS)
    {throw Exception("Failed to create a shader module", "GLGE::Graphic::Backend::Graphic::Vulkan::Shader::addShaderElement");}
    //store the data
    m_modules.push_back(Module{
        .tag = static_cast<i32>(el->type),
        .shaderModule = reinterpret_cast<void*>(mod)
    });
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Shader::finalize() {
    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_shader->getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicBackendInstance().get());
    
    //build all descriptor sets
    m_layouts.reserve(m_shader->getSetCount());
    for (u32 set = 0; set < m_shader->getSetCount(); ++set) {
        //check if the set even exists
        if (!m_shader->hasSet(set)) {
            m_layouts.push_back(nullptr);
            continue;
        }

        //get the resource template
        const ResourceTemplate& templ = m_shader->getSet(set);
        const auto& presets = templ.getBindings();

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
            switch (p.getType()) {
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
                throw Exception("Failed to map a preset type", "GLGE::Graphic::Backend::Graphic::Vulkan::Shader::finalize");
                break;
            }

            //all stages are active
            b.stageFlags = VK_SHADER_STAGE_ALL;

            //store the binding
            bindings.push_back(b);
            flags.push_back(VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
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
        {throw Exception("Failed to create a descriptor set layout", "GLGE::Graphic::Backend::Graphic::Vulkan::Shader::finalize");}

        m_layouts.push_back(reinterpret_cast<void*>(layout));
    }

    //check if a compute stage exists
    bool hasCompute = false;
    const Module* computeModule = nullptr;
    const GLGE::Graphic::Shader::ElementInfo* computeInfo = nullptr;
    for (size_t i = 0; i < m_modules.size(); ++i) {
        if (m_modules[i].tag == static_cast<i32>(GLGE::Graphic::Shader::Type::COMPUTE)) {
            hasCompute = true;
            computeModule = &m_modules[i];
            computeInfo = &m_shader->getOfType(GLGE::Graphic::Shader::Type::COMPUTE);
        }
    }

    //if a compute stage exists, create a compute pipeline
    //this is required for legacy OpenGL compatibility. 
    if (!hasCompute) 
    {return;}

    //create a pipeline layout for the compute pipeline
    VkPipelineLayoutCreateInfo plCreate {};
    plCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    plCreate.setLayoutCount = m_layouts.size();
    plCreate.pSetLayouts = reinterpret_cast<VkDescriptorSetLayout*>(m_layouts.data());
    VkPipelineLayout pipeLayout;
    if (vkCreatePipelineLayout(reinterpret_cast<VkDevice>(inst->getDevice()), &plCreate, nullptr, &pipeLayout) != VK_SUCCESS)
    {throw Exception("Failed to create a pipeline layout", "GLGE::Graphic::Backend::Graphic::Vulkan::Shader::finalize");}
    m_computePipeLayout = reinterpret_cast<void*>(pipeLayout);

    //create the compute pipeline stage
    VkPipelineShaderStageCreateInfo compStageCreate {};
    compStageCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    compStageCreate.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    compStageCreate.module = reinterpret_cast<VkShaderModule>(computeModule->shaderModule);
    compStageCreate.pName = computeInfo->entryPoint.c_str();

    //create the compute pipeline
    VkComputePipelineCreateInfo compPipeCreate {};
    compPipeCreate.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    compPipeCreate.stage = compStageCreate;
    compPipeCreate.layout = pipeLayout;
    VkPipeline pipe;
    if (vkCreateComputePipelines(reinterpret_cast<VkDevice>(inst->getDevice()), VK_NULL_HANDLE, 1, &compPipeCreate, nullptr, &pipe) != VK_SUCCESS)
    {throw Exception("Failed to create a compute pipeline", "GLGE::Graphic::Backend::Graphic::Vulkan::Shader::finalize");}
    //store the pipeline
    m_computePipe = reinterpret_cast<void*>(pipe);
}