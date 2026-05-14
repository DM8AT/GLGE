/**
 * @file Material.cpp
 * @author DM8AT
 * @brief implement the material binding for OpenGL
 * @version 0.1
 * @date 2026-03-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add materials
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Material.h"
//add OpenGL resource sets
//#include "Graphic/Backend/Builtin/Graphics/OpenGL/ResourceSet.h"

//add OpenGL implementations
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Shader.h"
#include "Graphic/Backend/Builtin/Graphics/OpenGL/VertexLayout.h"
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Framebuffer.h"

//add OpenGL
#include "__Mapper.h"

//helper to convert to depth function
static GLenum __toGLDepthFunc(GLGE::Graphic::Backend::Graphic::Material::DepthMode mode) {
    switch (mode) {
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_ALWAYS:         return GL_ALWAYS;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_NEVER:          return GL_NEVER;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_LESS:           return GL_LESS;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_LESS_EQUALS:    return GL_LEQUAL;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_GREATER:        return GL_GREATER;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_GREATER_EQUALS: return GL_GEQUAL;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_EQUALS:         return GL_EQUAL;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_NOT_EQUAL:      return GL_NOTEQUAL;
    }
    //fallback
    return GL_LESS;
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Material::bind(GLGE::Graphic::Backend::Graphic::CommandBuffer* buffer, size_t VBOOffset) {
    //binder function
    void (*func)(void*, size_t, u32, u8, uvec2) = [](void* _self, size_t offset, u32 target, u8 targetCount, uvec2 resolution) {
        //extract self
        GLGE::Graphic::Backend::Graphic::OpenGL::Material* self = reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Material*>(_self);
        //bind the shader
        u32 shader = reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Shader*>(self->m_shader.get())->getProgram();
        glUseProgram(shader);
        //bind the VAO
        self->m_layout->bind(offset);

        //enable / disable depth write
        glDepthMask(self->m_depthWrite ? GL_TRUE : GL_FALSE);
        //setup the depth function
        glDepthFunc(__toGLDepthFunc(self->m_depthMode));

        //set the cull mode
        if (self->m_cullMode == CullMode::OFF) {
            glDisable(GL_CULL_FACE);
        } else {
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
            glCullFace((self->m_cullMode == CullMode::BACK) ? GL_BACK : GL_FRONT);
        }

        //bind the target

        //activate all color attachments
        static constexpr const GLenum bufs[16] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7, GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9, GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11, GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15};
        glNamedFramebufferDrawBuffers(target, targetCount, bufs);
        glBindFramebuffer(GL_FRAMEBUFFER, target);
        glViewport(0, 0, resolution.x, resolution.y);
    };

    //bind the shader set
    if (m_shader->getFrontend()->hasSet(0)) {
        auto ptr = m_shader->getFrontend()->getResources(0);
        if (ptr) {ptr->getBackend()->bind(buffer);}
    }
    //get the framebuffer
    auto* fbuff = static_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer*>(m_fbuff.get());
    uvec2 size = fbuff->getColorAttachmentCount() ? fbuff->getColorAttachment(0)->getSize() : fbuff->getDepthAttachment(0)->getSize();
    //bind the shader
    buffer->addCommand(func, reinterpret_cast<void*>(this), size_t(VBOOffset), fbuff->getHandle(), fbuff->getColorAttachmentCount(), size);
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Material::rebindMesh(GLGE::Graphic::Backend::Graphic::CommandBuffer* buffer, size_t VBOOffset) {
    //binder function
    void (*func)(void*, size_t) = [](void* _self, size_t offset) {
        //extract self
        GLGE::Graphic::Backend::Graphic::OpenGL::Material* self = reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Material*>(_self);
        //bind the VAO
        self->m_layout->bind(offset);
    };

    //bind the VAO
    buffer->addCommand(func, reinterpret_cast<void*>(this), size_t(VBOOffset));
}