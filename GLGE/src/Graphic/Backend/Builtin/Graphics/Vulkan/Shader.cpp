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
    //check if a compute stage exists
    bool hasCompute = false;
    for (size_t i = 0; i < m_modules.size(); ++i) {
        if (m_modules[i].tag == static_cast<i32>(GLGE::Graphic::Shader::Type::COMPUTE))
        {hasCompute = true;}
    }

    //if a compute stage exists, create a compute pipeline
    //this is required for legacy OpenGL compatibility. 
    if (hasCompute) {
        //create the compute pipeline
        VkComputePipelineCreateInfo compCreate {};
        compCreate.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        compCreate.
    }
}