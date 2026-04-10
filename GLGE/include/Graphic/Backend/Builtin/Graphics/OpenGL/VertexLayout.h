/**
 * @file VertexLayout.h
 * @author DM8AT
 * @brief implement the OpenGL backend for the vertex layout
 * @version 0.1
 * @date 2026-03-25
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_VERTEX_LAYOUT_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_VERTEX_LAYOUT_

//add the vertex layout glue layer
#include "Graphic/Backend/Graphics/VertexLayout.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::OpenGL {

    /**
     * @brief define the vertex layout backend for OpenGL
     */
    class VertexLayout : public GLGE::Graphic::Backend::Graphic::VertexLayout {
    public:

        /**
         * @brief Construct a new Vertex Layout
         * 
         * @param ptr the pointer to the beginning of a continues array of attributes
         * @param size the amount of attributes in the array
         * @param stride the byte size of a single vertex
         * @param pool a reference to the mesh pool the layout attaches to
         */
        VertexLayout(const VertexAttribute* ptr, size_t size, size_t stride, Reference<GLGE::Graphic::Backend::Graphic::MeshPool> pool);

        /**
         * @brief bind the VAO
         * 
         * @param offset the byte offset of the VBO from the start
         */
        virtual void bind(size_t offset) override;

    protected:

        /**
         * @brief store a counter for name generation
         */
        inline static u32 ms_nameCounter = 0;
        /**
         * @brief store the name enumerator
         */
        u32 m_name = ++ms_nameCounter;

        /**
         * @brief store the vertex array
         */
        u32 m_vao = 0;

    };

}

#endif