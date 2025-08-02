/**
 * @file GLGERenderVertexLayout.cpp
 * @author DM8AT
 * @brief implement the render vertex layout
 * @version 0.1
 * @date 2025-07-29
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the render vertex layout
#include "GLGERenderVertexLayout.h"

//include all APIs
#include "GraphicAPI/GraphicAPIs/GLGE_AllAPIs.h"

RenderVertexLayout::RenderVertexLayout(Instance& instance)
 : InstAttachableClass(&instance, ATTACHMENT_TYPE_RENDER_VERTEX_LAYOUT, "RenderVertexLayout")
{
    //store the layout of a simple vertex
    VertexLayoutElement elements[] = {
        {"pos", sizeof(vec3)},
        {"tex", sizeof(vec2)},
        {"normal", sizeof(vec3)}
    };
    //initalize the own vertex layout
    m_layout = VertexLayout(elements, sizeof(elements)/sizeof(*elements));

    //create a graphic vertex layout
    switch (m_instance->getAPI())
    {
    case API_OPENGL_4_6:
        m_gLayout = new OGL4_6_VertexLayout(this, m_instance->getGraphicInstance());
        break;
    
    default:
        break;
    }
    //create the actual graphic vertex layout
    m_gLayout->onCreate();
}