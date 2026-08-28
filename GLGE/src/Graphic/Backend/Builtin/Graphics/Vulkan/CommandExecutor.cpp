/**
 * @file CommandExecutor.cpp
 * @author DM8AT
 * @brief implement the command executor for vulkan
 * @version 0.1
 * @date 2026-08-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the backend
#include "Graphic/Backend/Builtin/Graphics/Vulkan/CommandExecutor.h"

#include "Graphic/Window.h"
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Window.h"
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"
#include "Graphic/Backend/Builtin/Graphics/Vulkan/CommandBuffer.h"

//add vulkan
#include <vulkan/vulkan.h>

GLGE::Graphic::Backend::Graphic::Vulkan::CommandExecutor::CommandExecutor(GLGE::Graphic::Window* win)
 : Backend::Graphic::CommandExecutor(win)
{

}

GLGE::Graphic::Backend::Graphic::Vulkan::CommandExecutor::~CommandExecutor() {
    
}

void GLGE::Graphic::Backend::Graphic::Vulkan::CommandExecutor::dispatch(GLGE::Graphic::CommandStream* stream) {
    auto* inst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_window->getGraphicInstance()->getGraphicBackendInstance().get());
    VkDevice device = reinterpret_cast<VkDevice>(inst->getDevice());

}

void GLGE::Graphic::Backend::Graphic::Vulkan::CommandExecutor::awaitFinish() {
    
}