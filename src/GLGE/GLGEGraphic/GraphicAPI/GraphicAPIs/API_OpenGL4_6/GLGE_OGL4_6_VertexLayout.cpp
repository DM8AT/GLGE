/**
 * @file GLGE_OGL4_6_VertexLayout.cpp
 * @author DM8AT
 * @brief implement the OpenGL overload for the vertex layout
 * @version 0.1
 * @date 2025-07-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the OpenGL vertex layout
#include "GLGE_OGL4_6_VertexLayout.h"
//include the render vertex layout
#include "../../../GLGERenderVertexLayout.h"
//include OpenGL memory arenas
#include "GLGE_OGL4_6_MemoryArena.h"
//include command buffer
#include "GLGE_OGL4_6_CommandBuffer.h"

//include OpenGL
#include <GL/glew.h>

void OGL4_6_VertexLayout::onCreate() noexcept
{
    //it is important to queue the creation of the buffers before the creation of the VAO
    //since the buffers are reqired for the VAO creation
    //create the vertex buffer
    m_vbo = new OGL4_6_MemoryArena(0, true, MEMORY_USAGE_VERTEX_BUFFER, *m_graphicInstance->getInstance());
    m_vbo->setAPI(true);
    m_vbo->onCreate();
    //create the index buffer
    m_ibo = new OGL4_6_MemoryArena(0, true, MEMORY_USAGE_INDEX_BUFFER, *m_graphicInstance->getInstance());
    m_ibo->setAPI(true);
    m_ibo->onCreate();

    //queue the init function
    m_graphicInstance->getBuffers()[0]->add(0, (void*)ogl_Init, this, sizeof(this));
}

void OGL4_6_VertexLayout::onDestroy() noexcept
{

}


void OGL4_6_VertexLayout::ogl_Init(OGL4_6_VertexLayout* layout, uint64_t) noexcept
{
    //create the vertex array object
    glGenVertexArrays(1, &layout->m_vao);
    glBindVertexArray(layout->m_vao);

    //bind the vertex and index buffer
    glBindBuffer(GL_ARRAY_BUFFER, ((OGL4_6_MemoryArena*)layout->m_vbo)->getBuffer());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ((OGL4_6_MemoryArena*)layout->m_ibo)->getBuffer());

    //store the stride in bytes
    uint64_t stride = 0;
    //iterate over all vertex elements to fill the VAO
    for (size_t i = 0; i < layout->m_layout->getLayout().getElements().size(); ++i)
    {
        //get the current element
        auto el = layout->m_layout->getLayout().getElements()[i];
        //enable the element
        glEnableVertexAttribArray(i);
        //set the size of the element
        glVertexAttribPointer(i, layout->m_layout->getLayout().getSizeOf(el) / sizeof(float), GL_FLOAT, GL_FALSE, layout->m_layout->getLayout().getVertexSize(), (void*)stride);
        //increment the stride by the size of this element
        stride += layout->m_layout->getLayout().getSizeOf(el);
    }

    //clean up the state
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}