/**
 * @file GLGERenderVertexLayout.h
 * @author DM8AT
 * @brief define a vertex layout encapsulation that is used for rendering
 * This mapps a vertex layout to a graphic vertex layout. A graphic vertex layout is API-Dependend. A Render Vertex layout is required for the construction of a render mesh and render material. 
 * 
 * @version 0.1
 * @date 2025-07-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_RENDER_VERTEX_LAYOUT_
#define _GLGE_RENDER_VERTEX_LAYOUT_

//include vertex layouts
#include "../GLGECore/Geometry/GLGEVertex.h"
//include the graphic vertex layout
#include "GraphicAPI/GLGEGraphicAPI_VertexLayout.h"

//check for C++
//to create classes
#if GLGE_CPP

/**
 * @brief compine a vertex layout and a graphic vertex layout
 */
class RenderVertexLayout final : public InstAttachableClass
{
public:

    /**
     * @brief this creates a layout for a SimpleVertex
     * 
     * @param instance a reference to the instance the render vertex layout will belong to
     */
    RenderVertexLayout(Instance& instance);

    /**
     * @brief Get the vertex layout wrapped by the render vertex layout
     * 
     * @return VertexLayout& a reference to the wrapped vertex layout
     */
    inline VertexLayout& getLayout() noexcept {return m_layout;}

    /**
     * @brief Get the Graphic Vertex Layout created by the vertex layout
     * 
     * @return GraphicVertexLayout* a pointer to the graphic vertex layout
     */
    inline GraphicVertexLayout* getGraphicVertexLayout() const noexcept {return m_gLayout;}

    /**
     * @brief Get the Vertex Buffer used by the vertex layout
     * 
     * @return GraphicMemoryArena* a pointer to the used vertex buffer
     */
    inline GraphicMemoryArena* getVertexBuffer() const noexcept {return m_gLayout->getVBO();}

    /**
     * @brief Get the Index Buffer used by the vertex layout
     * 
     * @return GraphicMemoryArena* a pointer to the index buffer
     */
    inline GraphicMemoryArena* getIndexBuffer() const noexcept {return m_gLayout->getIBO();}

protected:

    /**
     * @brief store the own vertex layout
     */
    VertexLayout m_layout;
    /**
     * @brief store the graphic vertex layout
     * Zero should NOT occoure after intalization
     */
    GraphicVertexLayout* m_gLayout = 0;

};

#endif //end of C++ section

#endif