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
#include "Graphic/Backend/Builtin/Graphics/OpenGL/GeometryPoolStream.h"
//add GLAD
#include "glad/glad.h"

GLGE::Graphic::Backend::Graphic::OpenGL::GeometryPoolStream::GeometryPoolStream(u64 size, bool isIbo) 
 : GeometryPool::Stream(size, isIbo)
{
    //set the target
    m_target = m_isIBO ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;

    //resize to create
    onResize(size);
}

GLGE::Graphic::Backend::Graphic::OpenGL::GeometryPoolStream::~GeometryPoolStream() {
    //clean up
    glDeleteBuffers(1, &m_buff);
}

void GLGE::Graphic::Backend::Graphic::OpenGL::GeometryPoolStream::onFlush() {
    //no-op
}

void GLGE::Graphic::Backend::Graphic::OpenGL::GeometryPoolStream::onResize(u64 newSize) {
    //create the new buffer
    u32 newBuff = 0;
    glCreateBuffers(1, &newBuff);

    //setup static storage
    glNamedBufferStorage(newBuff, newSize, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    //map the buffer
    void* newMapping = glMapNamedBufferRange(newBuff, 0, newSize, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

    //if old data exists, copy over
    if (m_mapped) 
    {memcpy(newMapping, m_mapped, glm::min(newSize, m_size));}

    //cache old buffer
    u32 oldBuff = m_buff;

    //update
    m_buff = newBuff;
    m_mapped = newMapping;
    m_size = newSize;

    //if an old buffer exists, delete it
    if (oldBuff)
    {glDeleteBuffers(1, &oldBuff);}
}

void GLGE::Graphic::Backend::Graphic::OpenGL::GeometryPoolStream::onWrite(Region region, const void* data, u64 offset, u64 size) {
    //write
    memcpy(reinterpret_cast<u8*>(m_mapped) + region.offset + offset, data, size);
}