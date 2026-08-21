/**
 * @file GeometryPoolStream.cpp
 * @author DM8AT
 * @brief implement the geometry pool stream for OpenGL
 * @version 0.1
 * @date 2026-06-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add geometry pool streams
#include "Graphic/Backend/Builtin/Graphics/Vulkan/GeometryPoolStream.h"
//add Vulkan
#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

GLGE::Graphic::Backend::Graphic::Vulkan::GeometryPoolStream::GeometryPoolStream(u64 size, bool isIbo) 
 : GeometryPool::Stream(size, isIbo)
{
}

GLGE::Graphic::Backend::Graphic::Vulkan::GeometryPoolStream::~GeometryPoolStream() {
}

void GLGE::Graphic::Backend::Graphic::Vulkan::GeometryPoolStream::onFlush() {
}

void GLGE::Graphic::Backend::Graphic::Vulkan::GeometryPoolStream::onResize(u64 newSize) {
}

void GLGE::Graphic::Backend::Graphic::Vulkan::GeometryPoolStream::onWrite(Region region, const void* data, u64 offset, u64 size) {
}