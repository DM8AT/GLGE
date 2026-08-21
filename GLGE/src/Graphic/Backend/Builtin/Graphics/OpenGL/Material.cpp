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
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Framebuffer.h"
#include "Graphic/Backend/Builtin/Graphics/OpenGL/GeometryPoolStream.h"

//add OpenGL
#include "__Mapper.h"

/**
 * @brief store information about an OpenGL type
 */
struct GlTypeInfo {
    /**
     * @brief the amount of elements
     */
    GLGE::u32 elementCount = 0;
    /**
     * @brief the base type
     */
    GLenum type;
    /**
     * @brief store if the element is normalized
     */
    GLenum normalized = GL_FALSE;
};

//helper to get GL type
static GlTypeInfo __toGLType(GLGE::Graphic::VertexLayout::Type type) {
    //switch over the type
    switch (type) {
        case GLGE::Graphic::VertexLayout::Type::Int:        return GlTypeInfo {1, GL_INT,           GL_FALSE};
        case GLGE::Graphic::VertexLayout::Type::UInt:       return GlTypeInfo {1, GL_UNSIGNED_INT,  GL_FALSE};
        case GLGE::Graphic::VertexLayout::Type::Float:      return GlTypeInfo {1, GL_FLOAT,         GL_FALSE};
        case GLGE::Graphic::VertexLayout::Type::vec2:       return GlTypeInfo {2, GL_FLOAT,         GL_FALSE};
        case GLGE::Graphic::VertexLayout::Type::vec3:       return GlTypeInfo {3, GL_FLOAT,         GL_FALSE};
        case GLGE::Graphic::VertexLayout::Type::vec4:       return GlTypeInfo {4, GL_FLOAT,         GL_FALSE};
        case GLGE::Graphic::VertexLayout::Type::ivec2:      return GlTypeInfo {2, GL_INT,           GL_FALSE};
        case GLGE::Graphic::VertexLayout::Type::ivec3:      return GlTypeInfo {3, GL_INT,           GL_FALSE};
        case GLGE::Graphic::VertexLayout::Type::ivec4:      return GlTypeInfo {4, GL_INT,           GL_FALSE};
        case GLGE::Graphic::VertexLayout::Type::uvec2:      return GlTypeInfo {2, GL_UNSIGNED_INT,  GL_FALSE};
        case GLGE::Graphic::VertexLayout::Type::uvec3:      return GlTypeInfo {3, GL_UNSIGNED_INT,  GL_FALSE};
        case GLGE::Graphic::VertexLayout::Type::uvec4:      return GlTypeInfo {4, GL_UNSIGNED_INT,  GL_FALSE};
        case GLGE::Graphic::VertexLayout::Type::unorm_u8x1: return GlTypeInfo {1, GL_UNSIGNED_BYTE, GL_TRUE};
        case GLGE::Graphic::VertexLayout::Type::unorm_u8x2: return GlTypeInfo {2, GL_UNSIGNED_BYTE, GL_TRUE};
        case GLGE::Graphic::VertexLayout::Type::unorm_u8x4: return GlTypeInfo {4, GL_UNSIGNED_BYTE, GL_TRUE};
        case GLGE::Graphic::VertexLayout::Type::snorm_u8x1: return GlTypeInfo {1, GL_BYTE,          GL_TRUE};
        case GLGE::Graphic::VertexLayout::Type::snorm_u8x2: return GlTypeInfo {2, GL_BYTE,          GL_TRUE};
        case GLGE::Graphic::VertexLayout::Type::snorm_u8x4: return GlTypeInfo {4, GL_BYTE,          GL_TRUE};

    default: std::unreachable();
    }
}

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

GLGE::Graphic::Backend::Graphic::OpenGL::Material::Material(GLGE::Reference<GLGE::Graphic::Backend::Graphic::Shader> shader, GLGE::Graphic::VertexLayout* layout, Reference<GLGE::Graphic::Backend::Graphic::Framebuffer> fbuff, Graphic::Material::CullMode cullMode, Graphic::Material::DepthMode depthMode, bool depthWrite) 
 : GLGE::Graphic::Backend::Graphic::Material(shader, layout, fbuff, cullMode, depthMode, depthWrite)
{
    //build the VAO
    updateVAO();
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Material::bind(GLGE::Graphic::Backend::Graphic::CommandBuffer* buffer) {
    //binder function
    void (*func)(void*, u32, u8, uvec2, u32) = [](void* _self, u32 target, u8 targetCount, uvec2 resolution, u32 vao) {
        //extract self
        GLGE::Graphic::Backend::Graphic::OpenGL::Material* self = reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Material*>(_self);
        //bind the shader
        u32 shader = reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Shader*>(self->m_shader.get())->getProgram();
        glUseProgram(shader);
        //bind the VAO
        glBindVertexArray(vao);

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
    buffer->addCommand(func, reinterpret_cast<void*>(this), fbuff->getHandle(), fbuff->getColorAttachmentCount(), size, u32(m_vao));
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Material::updateVAO() {
    //get the OpenGL graphic instance
    GLGE::Graphic::Instance* inst = GLGE::Instance::getCurrentInstance()->getExtension<GLGE::Graphic::Instance>();
    //get the archetype for the layout
    std::vector<GeometryPool::AttributeIdentifier> identifiers;
    identifiers.reserve(m_layout->getAttributeCount());
    for (size_t i = 0; i < m_layout->getAttributeCount(); ++i) {
        const auto& attr = m_layout->getAttribute(i);
        identifiers.emplace_back(attr.usage, static_cast<u8>(attr.type), attr.streamId);
    }
    auto& arch = inst->meshManager().getPool().acquireArchetype(identifiers);

    //create the new VAO
    u32 newVao = 0;
    glCreateVertexArrays(1, &newVao);

    //enable all required streams
    for (u8 streamId = 0; streamId < Backend::Graphic::GeometryPool::MAX_STREAM_COUNT; ++streamId) {
        auto& stream = arch.accessStream(streamId);

        //if the stream is unused, skip it
        if (!stream.active) {continue;}

        //compute the stride of the stream
        u32 stride = 0;
        for (size_t i = 0; i < m_layout->getAttributeCount(); ++i) {
            const auto& attr = m_layout->getAttribute(i);
            if (attr.streamId != streamId) {continue;}
            stride = std::max<u32>(stride, attr.streamOffset + GLGE::Mesh::VertexAttribute::getTypeInfo(attr.type).size);
        }

        //get the corresponding VBO
        auto* vbo = static_cast<OpenGL::GeometryPoolStream*>(stream.stream.get());

        //bind the VBO for the index
        glVertexArrayVertexBuffer(newVao, streamId, vbo->getBuffer(), 0, stride);
    }

    //write the vertex attributes
    for (size_t i = 0; i < m_layout->getAttributeCount(); ++i) {
        //get the attribute
        const auto& attr = m_layout->getAttribute(i);
        
        //get the OpenGL type
        GlTypeInfo t = __toGLType(attr.type);

        //activate the attribute
        glEnableVertexArrayAttrib(newVao, i);
        //set the attribute format
        glVertexArrayAttribFormat(newVao, i, t.elementCount, t.type, t.normalized, attr.streamOffset);
        //bind to the correct stream
        glVertexArrayAttribBinding(newVao, i, attr.streamId);
    }

    //attach the IBO
    glVertexArrayElementBuffer(newVao, static_cast<OpenGL::GeometryPoolStream*>(arch.getIndexStream())->getBuffer());

    //if an old VAO exists, delete it
    if (m_vao != 0) {glDeleteVertexArrays(1, &m_vao);}
    m_vao = newVao;
}