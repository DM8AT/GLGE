/**
 * @file GLGE_OGL4_6_VertexLayout.h
 * @author DM8AT
 * @brief define the OpenGL specific implementation for a vertex
 * @version 0.1
 * @date 2025-07-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_OGL_4_6_VERTEX_LAYOUT_
#define _GLGE_OGL_4_6_VERTEX_LAYOUT_

//include the base class
#include "../../GLGEGraphicAPI_VertexLayout.h"

//check for C++
//to overload the class
#if GLGE_CPP

//check for C++ to overload the class
class OGL4_6_VertexLayout final : public GraphicVertexLayout
{
public:

    /**
     * @brief Construct a new OpenGL 4.6 vertex layout
     * 
     * @param vLayout a pointer to the parent render vertex layout
     * @param instance a pointer to the graphic instance the render vertex layout will belong to
     */
    OGL4_6_VertexLayout(RenderVertexLayout* vLayout, GraphicInstance* instance)
     : GraphicVertexLayout(vLayout, instance)
    {}

    /**
     * @brief override the function to create the grpahic vertex layout
     */
    virtual void onCreate() noexcept override;

    /**
     * @brief override the function to destroy the graphic vertex layout
     */
    virtual void onDestroy() noexcept override;

    /**
     * @brief get the vertex array object from the vertex layout
     * 
     * @return int32_t the OpenGL vertex array object
     */
    inline uint32_t getVAO() const noexcept {return m_vao;}

protected:

    /**
     * @brief initalize a vertex layout from the OpenGL thread
     * 
     * @param layout a pointer to the vertex layout to initalize
     */
    static void ogl_Init(OGL4_6_VertexLayout* layout, uint64_t) noexcept;

    /**
     * @brief store the vertex array object
     */
    uint32_t m_vao = 0;

};

#endif //end of C++ section

#endif