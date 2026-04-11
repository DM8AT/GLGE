/**
 * @file VertexLayout.cpp
 * @author DM8AT
 * @brief implement the vertex layout
 * @version 0.1
 * @date 2026-03-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//include vertex layouts
#include "Graphic/Backend/Builtin/Graphics/OpenGL/VertexLayout.h"
//include mesh pools
#include "Graphic/Backend/Builtin/Graphics/OpenGL/MeshPool.h"

//add OpenGL
#include "__Mapper.h"

GLGE::Graphic::Backend::Graphic::OpenGL::VertexLayout::VertexLayout(const VertexAttribute* ptr, size_t size, size_t stride, Reference<GLGE::Graphic::Backend::Graphic::MeshPool> pool) 
 : GLGE::Graphic::Backend::Graphic::VertexLayout(ptr, size, stride, pool)
{
    //create a new vertex array
    glCreateVertexArrays(1, &m_vao);
    //in debug name the vao
    #if GLGE_DEBUG
    std::string name = std::string("VertexLayout_") + std::to_string(m_name);
    glObjectLabel(GL_VERTEX_ARRAY, m_vao, name.length(), name.c_str());
    #endif

    //fill in the attributes
    for (size_t i = 0; i < m_attributeCount; ++i) {
        //get the attribute
        auto& attrib = m_attributes[i];
        //enable the slot
        glEnableVertexArrayAttrib(m_vao, attrib.getBinding());
        //setup the data pointer
        switch (attrib.getFormat())
        {
        case GLGE::Graphic::VertexAttribute::Format::Int: {
                glVertexArrayAttribIFormat(m_vao, attrib.getBinding(), 1, GL_INT, attrib.getOffset());
            } break;
        case GLGE::Graphic::VertexAttribute::Format::UInt: {
                glVertexArrayAttribIFormat(m_vao, attrib.getBinding(), 1, GL_UNSIGNED_INT, attrib.getOffset());
            } break;
        case GLGE::Graphic::VertexAttribute::Format::Float: {
                glVertexArrayAttribFormat(m_vao, attrib.getBinding(), 1, GL_FLOAT, GL_FALSE, attrib.getOffset());
            } break;
        case GLGE::Graphic::VertexAttribute::Format::vec2: {
                glVertexArrayAttribFormat(m_vao, attrib.getBinding(), 2, GL_FLOAT, GL_FALSE, attrib.getOffset());
            } break;
        case GLGE::Graphic::VertexAttribute::Format::vec3: {
                glVertexArrayAttribFormat(m_vao, attrib.getBinding(), 3, GL_FLOAT, GL_FALSE, attrib.getOffset());
            } break;
        case GLGE::Graphic::VertexAttribute::Format::vec4: {
                glVertexArrayAttribFormat(m_vao, attrib.getBinding(), 4, GL_FLOAT, GL_FALSE, attrib.getOffset());
            } break;
        case GLGE::Graphic::VertexAttribute::Format::ivec2: {
                glVertexArrayAttribIFormat(m_vao, attrib.getBinding(), 2, GL_INT, attrib.getOffset());
            } break;
        case GLGE::Graphic::VertexAttribute::Format::ivec3: {
                glVertexArrayAttribIFormat(m_vao, attrib.getBinding(), 3, GL_INT, attrib.getOffset());
            } break;
        case GLGE::Graphic::VertexAttribute::Format::ivec4: {
                glVertexArrayAttribIFormat(m_vao, attrib.getBinding(), 4, GL_INT, attrib.getOffset());
            } break;
        case GLGE::Graphic::VertexAttribute::Format::uvec2: {
                glVertexArrayAttribIFormat(m_vao, attrib.getBinding(), 2, GL_UNSIGNED_INT, attrib.getOffset());
            } break;
        case GLGE::Graphic::VertexAttribute::Format::uvec3: {
                glVertexArrayAttribIFormat(m_vao, attrib.getBinding(), 3, GL_UNSIGNED_INT, attrib.getOffset());
            } break;
        case GLGE::Graphic::VertexAttribute::Format::uvec4: {
                glVertexArrayAttribIFormat(m_vao, attrib.getBinding(), 4, GL_UNSIGNED_INT, attrib.getOffset());
            } break;
        case GLGE::Graphic::VertexAttribute::Format::unorm_u8x1: {
                glVertexArrayAttribFormat(m_vao, attrib.getBinding(), 1, GL_UNSIGNED_BYTE, GL_TRUE, attrib.getOffset());
            } break;
        case GLGE::Graphic::VertexAttribute::Format::unorm_u8x2: {
                glVertexArrayAttribFormat(m_vao, attrib.getBinding(), 2, GL_UNSIGNED_BYTE, GL_TRUE, attrib.getOffset());
            } break;
        case GLGE::Graphic::VertexAttribute::Format::unorm_u8x4: {
                glVertexArrayAttribFormat(m_vao, attrib.getBinding(), 4, GL_UNSIGNED_BYTE, GL_TRUE, attrib.getOffset());
            } break;
        case GLGE::Graphic::VertexAttribute::Format::snorm_u8x1: {
                glVertexArrayAttribFormat(m_vao, attrib.getBinding(), 1, GL_BYTE, GL_TRUE, attrib.getOffset());
            } break;
        case GLGE::Graphic::VertexAttribute::Format::snorm_u8x2: {
                glVertexArrayAttribFormat(m_vao, attrib.getBinding(), 2, GL_BYTE, GL_TRUE, attrib.getOffset());
            } break;
        case GLGE::Graphic::VertexAttribute::Format::snorm_u8x4: {
                glVertexArrayAttribFormat(m_vao, attrib.getBinding(), 4, GL_BYTE, GL_TRUE, attrib.getOffset());
            } break;
        
        default:
            throw GLGE::Exception("Tried to create vertex layout using unknown attribute format", "GLGE::Graphic::Backend::Graphic::OpenGL::VertexLayout::VertexLayout");
            break;
        }

        //attach to vertex buffer binding slot 0
        glVertexArrayAttribBinding(m_vao, attrib.getBinding(), 0);
    }
}

void GLGE::Graphic::Backend::Graphic::OpenGL::VertexLayout::bind(size_t offset) {
    //set the VAO binding
    glVertexArrayVertexBuffer(m_vao, 0, reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool*>(m_pool.get())->getVBO(), offset, m_stride);
    //bind the VAO
    glBindVertexArray(m_vao);
    //bind the IBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool*>(m_pool.get())->getIBO());
}