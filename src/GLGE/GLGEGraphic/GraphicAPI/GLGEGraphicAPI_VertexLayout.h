/**
 * @file GLGEGraphicAPI_VertexLayout.h
 * @author DM8AT
 * @brief define a layout for a class to wrap a vertex layout for the GPU
 * @version 0.1
 * @date 2025-07-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_GRAPHIC_API_VERTEX_LAYOUT_
#define _GLGE_GRAPHIC_API_VERTEX_LAYOUT_

//include the vertex layout
#include "../../GLGECore/Geometry/GLGEVertex.h"
//include the graphic instance
#include "GLGEGraphicAPI_Instance.h"
//include memory arenas for vertex and index buffer
#include "GLGEGraphicAPI_MemoryArena.h"

//check for C++
//for class creation
#if GLGE_CPP

/**
 * @brief render vertex layouts will be defined later
 */
class RenderVertexLayout;

/**
 * @brief define a wrapper for an API-Dependend implementation of a vertex layout
 */
class GraphicVertexLayout : public GraphicInstAttatchable
{
public:

    /**
     * @brief Construct a new Graphic Vertex Layout
     * Explicetly deleted, a render vertex layout is required
     */
    GraphicVertexLayout() = delete;

    /**
     * @brief Construct a new Graphic Vertex Layout
     * 
     * @param vLayout a pointer to the parent layout
     * @param instance a pointer to the instance the vertex layout will belong to
     */
    GraphicVertexLayout(RenderVertexLayout* vLayout, GraphicInstance* instance)
     : GraphicInstAttatchable(instance), m_layout(vLayout)
    {}

    /**
     * @brief Destroy the Graphic Vertex Layout
     */
    virtual ~GraphicVertexLayout() {onDestroy();}

    /**
     * @brief a virtual function to initalize the graphic vertex layout
     */
    virtual void onCreate() noexcept {}

    /**
     * @brief a function to clean up the vertex layout
     */
    virtual void onDestroy() noexcept {}

    /**
     * @brief get the vertex buffer from the vertex layout
     * 
     * @return GraphicMemoryArena* a pointer to the memory arena containing the vertex buffer
     */
    inline GraphicMemoryArena* getVBO() const noexcept {return m_vbo;}

    /**
     * @brief get the index buffer from the vertex layout
     * 
     * @return GraphicMemoryArena* a pointer to the memory arena containing the index buffer
     */
    inline GraphicMemoryArena* getIBO() const noexcept {return m_ibo;}

protected:

    /**
     * @brief store a pointer to the parent render layout
     */
    RenderVertexLayout* m_layout = 0;
    /**
     * @brief store a vertex buffer
     */
    GraphicMemoryArena* m_vbo = 0;
    /**
     * @brief store an index buffer
     */
    GraphicMemoryArena* m_ibo = 0;

};

#endif //end of C++ section

#endif