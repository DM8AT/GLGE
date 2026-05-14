/**
 * @file Material.cpp
 * @author DM8AT
 * @brief implement the vulkan material
 * @version 0.1
 * @date 2026-05-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add vulkan materials
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Material.h"
//add vulkan shaders
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Shader.h"
//add vulkan framebuffers
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Framebuffer.h"
//add shader frontends
#include "Graphic/Shader.h"

//add shared stuff
#include "Shared.h"

/**
 * @brief a helper function to get the vertex attribute format
 * 
 * @param f the format to map
 * @return `VkFormat` the format
 */
inline static VkFormat __toVkFormat(GLGE::Graphic::VertexAttribute::Format f) {
    switch (f) {
        case GLGE::Graphic::VertexAttribute::Format::Float:  return VK_FORMAT_R32_SFLOAT;
        case GLGE::Graphic::VertexAttribute::Format::vec2:   return VK_FORMAT_R32G32_SFLOAT;
        case GLGE::Graphic::VertexAttribute::Format::vec3:   return VK_FORMAT_R32G32B32_SFLOAT;
        case GLGE::Graphic::VertexAttribute::Format::vec4:   return VK_FORMAT_R32G32B32A32_SFLOAT;

        case GLGE::Graphic::VertexAttribute::Format::Int:    return VK_FORMAT_R32_SINT;
        case GLGE::Graphic::VertexAttribute::Format::ivec2:  return VK_FORMAT_R32G32_SINT;
        case GLGE::Graphic::VertexAttribute::Format::ivec3:  return VK_FORMAT_R32G32B32_SINT;
        case GLGE::Graphic::VertexAttribute::Format::ivec4:  return VK_FORMAT_R32G32B32A32_SINT;

        case GLGE::Graphic::VertexAttribute::Format::UInt:   return VK_FORMAT_R32_UINT;
        case GLGE::Graphic::VertexAttribute::Format::uvec2:  return VK_FORMAT_R32G32_UINT;
        case GLGE::Graphic::VertexAttribute::Format::uvec3:  return VK_FORMAT_R32G32B32_UINT;
        case GLGE::Graphic::VertexAttribute::Format::uvec4:  return VK_FORMAT_R32G32B32A32_UINT;

        case GLGE::Graphic::VertexAttribute::Format::unorm_u8x1: return VK_FORMAT_R8_UNORM;
        case GLGE::Graphic::VertexAttribute::Format::unorm_u8x2: return VK_FORMAT_R8G8_UNORM;
        case GLGE::Graphic::VertexAttribute::Format::unorm_u8x4: return VK_FORMAT_R8G8B8A8_UNORM;

        case GLGE::Graphic::VertexAttribute::Format::snorm_u8x1: return VK_FORMAT_R8_SNORM;
        case GLGE::Graphic::VertexAttribute::Format::snorm_u8x2: return VK_FORMAT_R8G8_SNORM;
        case GLGE::Graphic::VertexAttribute::Format::snorm_u8x4: return VK_FORMAT_R8G8B8A8_SNORM;

        default: return VK_FORMAT_UNDEFINED;
    }
}

/**
 * @brief a helper function to translate a depth compare mode
 * 
 * @param mode the mode to translate
 * @return `VkCompareOp` the translation result
 */
inline static VkCompareOp __toVkCompareOp(GLGE::Graphic::Backend::Graphic::Material::DepthMode mode) {
    switch (mode) {
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_ALWAYS: return VK_COMPARE_OP_ALWAYS; break;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_NEVER: return VK_COMPARE_OP_NEVER; break;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_LESS: return VK_COMPARE_OP_LESS; break;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_LESS_EQUALS: return VK_COMPARE_OP_LESS_OR_EQUAL; break;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_GREATER: return VK_COMPARE_OP_GREATER; break;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_GREATER_EQUALS: return VK_COMPARE_OP_GREATER_OR_EQUAL; break;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_EQUALS: return VK_COMPARE_OP_EQUAL; break;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_NOT_EQUAL: return VK_COMPARE_OP_NOT_EQUAL; break;

        default: return VK_COMPARE_OP_ALWAYS;
    }
}

/**
 * @brief a helper to translate the cull mode
 * 
 * @param mode the cull mode to translate
 * @return `VkCullModeFlags` the translation result
 */
inline static VkCullModeFlags __toVkCullMode(GLGE::Graphic::Backend::Graphic::Material::CullMode mode) {
    switch (mode) {
        case GLGE::Graphic::Backend::Graphic::Material::CullMode::OFF: return VK_CULL_MODE_NONE; break;
        case GLGE::Graphic::Backend::Graphic::Material::CullMode::BACK: return VK_CULL_MODE_BACK_BIT; break;
        case GLGE::Graphic::Backend::Graphic::Material::CullMode::FRONT: return VK_CULL_MODE_FRONT_BIT; break;

        default: return VK_CULL_MODE_NONE;
    }
}

GLGE::Graphic::Backend::Graphic::Vulkan::Material::Material(Reference<GLGE::Graphic::Backend::Graphic::Shader> shader, Reference<GLGE::Graphic::Backend::Graphic::VertexLayout> layout, Reference<GLGE::Graphic::Backend::Graphic::Framebuffer> fbuff, CullMode cullMode, DepthMode depthMode, bool depthWrite)
 : GLGE::Graphic::Backend::Graphic::Material(shader, layout, fbuff, cullMode, depthMode, depthWrite)
{
    //store the instance
    m_instance = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(shader->getFrontend()->getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicBackendInstance().get());
    //get the vulkan shader
    auto* vkShader = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Shader*>(shader.get());
    //get the vulkan framebuffer
    auto* vkFbuff = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer*>(m_fbuff.get());

    //create the pipeline layout
    VkPipelineLayoutCreateInfo pipeLayoutCreate {};
    pipeLayoutCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeLayoutCreate.setLayoutCount = vkShader->getLayouts().size();
    pipeLayoutCreate.pSetLayouts = reinterpret_cast<VkDescriptorSetLayout const*>(vkShader->getLayouts().data());
    VkPipelineLayout vkLayout;
    if (vkCreatePipelineLayout(reinterpret_cast<VkDevice>(m_instance->getDevice()), &pipeLayoutCreate, nullptr, &vkLayout) != VK_SUCCESS)
    {throw Exception("Failed to create a pipeline layout", "GLGE::Graphic::Backend::Graphic::Vulkan::Material::Material");}
    //store the pipeline layout
    m_pipeLayout = reinterpret_cast<void*>(vkLayout);

    //gather all shader stages for graphics
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    shaderStages.reserve(4);
    for (size_t i = 0; i < vkShader->getModules().size(); ++i) {
        //get the module
        const auto& mod = vkShader->getModules()[i];
        //get reflection information about the state
        const auto& state = vkShader->getFrontend()->getElements()[i];

        //get the vulkan stage type
        VkShaderStageFlagBits stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        switch (static_cast<GLGE::Graphic::Shader::Type>(mod.tag))
        {
        case GLGE::Graphic::Shader::VERTEX: stage = VK_SHADER_STAGE_VERTEX_BIT; break;
        case GLGE::Graphic::Shader::TESSELATION_CONTROL: stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT; break;
        case GLGE::Graphic::Shader::TESSELATION_EVALUATION: stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT; break;
        case GLGE::Graphic::Shader::GEOMETRY: stage = VK_SHADER_STAGE_GEOMETRY_BIT; break;
        case GLGE::Graphic::Shader::FRAGMENT: stage = VK_SHADER_STAGE_FRAGMENT_BIT; break;
        
        default:
            break;
        }
        //if the stage is unknown, stop
        if (stage == VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM)
        {continue;}

        //create the shader stage
        VkPipelineShaderStageCreateInfo shaderStageCreate {};
        shaderStageCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageCreate.module = reinterpret_cast<VkShaderModule>(mod.shaderModule);
        shaderStageCreate.stage = stage;
        shaderStageCreate.pName = state.entryPoint.c_str();
        shaderStages.push_back(shaderStageCreate);
    }

    //create all bindings and attributes
    std::vector<VkVertexInputAttributeDescription> vertAttrDesc;
    vertAttrDesc.reserve(m_layout->getAttributeCount());
    VkVertexInputBindingDescription vertBindDesc;
    vertBindDesc.binding = 0;
    vertBindDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    vertBindDesc.stride = static_cast<u32>(m_layout->getVertexSize());
    for (size_t i = 0; i < m_layout->getAttributeCount(); ++i) {
        //get the attribute
        const auto& attr = m_layout->getAttribute(i);

        //create the vertex attribute
        VkVertexInputAttributeDescription attrDesc {};
        attrDesc.binding = 0;
        attrDesc.format = __toVkFormat(attr.getFormat());
        attrDesc.location = attr.getBinding();
        attrDesc.offset = attr.getOffset();
        vertAttrDesc.push_back(attrDesc);
    }
    //create the vertex input layout
    VkPipelineVertexInputStateCreateInfo vertInputCreate {};
    vertInputCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertInputCreate.vertexBindingDescriptionCount = 1;
    vertInputCreate.pVertexBindingDescriptions = &vertBindDesc;
    vertInputCreate.vertexAttributeDescriptionCount = vertAttrDesc.size();
    vertInputCreate.pVertexAttributeDescriptions = vertAttrDesc.data();

    //input assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreat {};
    inputAssemblyCreat.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCreat.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyCreat.primitiveRestartEnable = VK_FALSE;

    //dynamic states (viewport + scissor)
    VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynStateCreate {};
    dynStateCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynStateCreate.dynamicStateCount = sizeof(dynamicStates) / sizeof(*dynamicStates);
    dynStateCreate.pDynamicStates = dynamicStates;

    //viewport state still required
    auto size = m_fbuff->getColorAttachmentCount() ? m_fbuff->getColorAttachment(0)->getSize() : m_fbuff->getDepthAttachment(0)->getSize();
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)size.x;
    viewport.height = (float)size.y;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = {size.x, size.y};
    VkPipelineViewportStateCreateInfo viewportStateCreate {};
    viewportStateCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreate.viewportCount = 1;
    viewportStateCreate.scissorCount = 1;
    viewportStateCreate.pScissors = &scissor;
    viewportStateCreate.pViewports = &viewport;

    //rasterizer
    VkPipelineRasterizationStateCreateInfo rasterStateCreate {};
    rasterStateCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterStateCreate.depthClampEnable = VK_FALSE;
    rasterStateCreate.rasterizerDiscardEnable = VK_FALSE;
    rasterStateCreate.polygonMode = VK_POLYGON_MODE_FILL;
    rasterStateCreate.lineWidth = 1.f;
    rasterStateCreate.cullMode = __toVkCullMode(m_cullMode);
    rasterStateCreate.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterStateCreate.depthBiasEnable = VK_FALSE;

    //multi-sample
    i32 sampleCount = m_fbuff->getColorAttachmentCount() ? m_fbuff->getColorAttachment(0)->getSamplesPerPixel() : m_fbuff->getDepthAttachment(0)->getSamplesPerPixel();
    VkPipelineMultisampleStateCreateInfo multiSampleStateCreate {};
    multiSampleStateCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multiSampleStateCreate.sampleShadingEnable = (sampleCount > 1) ? VK_TRUE : VK_FALSE;
    multiSampleStateCreate.rasterizationSamples = static_cast<VkSampleCountFlagBits>(sampleCount);

    //color blending
    VkPipelineColorBlendAttachmentState colorBlendAttachment {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    VkPipelineColorBlendStateCreateInfo colorBlendingStateCreate {};
    colorBlendingStateCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendingStateCreate.logicOpEnable = VK_FALSE;
    colorBlendingStateCreate.attachmentCount = 1;
    colorBlendingStateCreate.pAttachments = &colorBlendAttachment;

    //create the actual graphic pipeline
    VkGraphicsPipelineCreateInfo graphicsPipeCreate {};
    graphicsPipeCreate.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipeCreate.stageCount = shaderStages.size();
    graphicsPipeCreate.pStages = shaderStages.data();
    graphicsPipeCreate.pVertexInputState = &vertInputCreate;
    graphicsPipeCreate.pInputAssemblyState = &inputAssemblyCreat;
    graphicsPipeCreate.pViewportState = &viewportStateCreate;
    graphicsPipeCreate.pDynamicState = &dynStateCreate;
    graphicsPipeCreate.pRasterizationState = &rasterStateCreate;
    graphicsPipeCreate.pMultisampleState = &multiSampleStateCreate;
    graphicsPipeCreate.pColorBlendState = &colorBlendingStateCreate;
    graphicsPipeCreate.layout = static_cast<VkPipelineLayout>(m_pipeLayout);
    graphicsPipeCreate.renderPass = static_cast<VkRenderPass>(vkFbuff->getRenderPass());
    graphicsPipeCreate.subpass = 0;
    graphicsPipeCreate.basePipelineHandle = VK_NULL_HANDLE;
    VkPipeline pipe;
    if (vkCreateGraphicsPipelines(reinterpret_cast<VkDevice>(m_instance->getDevice()), VK_NULL_HANDLE, 1, &graphicsPipeCreate, nullptr, &pipe) != VK_SUCCESS)
    {throw Exception("Failed to create a graphics pipeline", "GLGE::Graphic::Backend::Graphic::Vulkan::Material::Material");}
    //store the pipeline
    m_pipe = reinterpret_cast<void*>(pipe);
}

GLGE::Graphic::Backend::Graphic::Vulkan::Material::~Material() {
    //destroy the pipeline
    vkDestroyPipeline(reinterpret_cast<VkDevice>(m_instance->getDevice()), reinterpret_cast<VkPipeline>(m_pipe), nullptr);
    //destroy the pipeline layout
    vkDestroyPipelineLayout(reinterpret_cast<VkDevice>(m_instance->getDevice()), reinterpret_cast<VkPipelineLayout>(m_pipeLayout), nullptr);
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Material::bind(GLGE::Graphic::Backend::Graphic::CommandBuffer* buffer, size_t VBOOffset) {

}

void GLGE::Graphic::Backend::Graphic::Vulkan::Material::rebindMesh(GLGE::Graphic::Backend::Graphic::CommandBuffer* buffer, size_t VBOOffset) {

}