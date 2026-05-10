/**
 * @file Translator.h
 * @author DM8AT
 * @brief define the functions to translate the frontend render pipeline modules into valid vulkan commands
 * @version 0.1
 * @date 2026-05-08
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#pragma once

//include the GLGE graphic frontend
#include "Graphic/Graphic.h"

//use a namespace for implementations
namespace VkImpl {

    /**
     * @brief clear a render target with a specific color
     * 
     * @param cmdBuff the command buffer to record the function into
     * @param handle the handle that holds the required data for the function
     * @return `true` if the clear was recorded successfully, `false` otherwise
     */
    bool clear(GLGE::Graphic::Backend::Graphic::CommandBuffer& cmdBuff, const GLGE::Graphic::Backend::Graphic::CommandHandle& handle);

}