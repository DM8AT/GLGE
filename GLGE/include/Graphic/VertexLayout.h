/**
 * @file VertexLayout.h
 * @author DM8AT
 * @brief define a structure that says how a vertex is structured
 * @version 0.1
 * @date 2026-03-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_VERTEX_LAYOUT_
#define _GLGE_GRAPHIC_VERTEX_LAYOUT_

//add common stuff
#include "Core/Common.h"
//add the base class
#include "Core/BaseClass.h"
//add instances
#include "Core/Instance.h"
//add graphic instances
#include "Instance.h"

//add vertex layout backends
#include "Backend/Graphics/VertexLayout.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief store the structure of a single vertex
     */
    class VertexLayout : public BaseClass {
    public:

        /**
         * @brief Construct a new Vertex Layout
         * 
         * @param attributes an initializer list of the attributes to create the layout from
         * @param vertexSize the byte size of a whole vertex
         */
        VertexLayout(std::initializer_list<VertexAttribute> attributes, size_t vertexSize)
         : VertexLayout(attributes.begin(), attributes.size(), vertexSize)
        {}

        /**
         * @brief Construct a new Vertex Layout
         * 
         * @param attributes a vector containing the attributes to create the layout from
         * @param vertexSize the byte size of a whole vertex
         */
        VertexLayout(const std::vector<VertexAttribute>& attributes, size_t vertexSize)
         : VertexLayout(&attributes.front(), attributes.size(), vertexSize)
        {}

        /**
         * @brief Construct a new Vertex Layout
         * 
         * @param attributes a pointer to a constant array of attributes
         * @param count the amount of elements in the attribute array
         * @param vertexSize the byte size of a whole vertex
         */
        VertexLayout(const VertexAttribute* attributes, size_t count, size_t vertexSize)
         : BaseClass(), m_layout(getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicDescription()->createVertexLayout(attributes, count, vertexSize, 
                                    getInstance()->getExtension<GLGE::Graphic::Instance>()->getMeshPool())
                                )
        {}

        /**
         * @brief Get the Layout backend
         * 
         * @return `Reference<GLGE::Graphic::Backend::Graphic::VertexLayout>` a reference to the layout backend
         */
        inline Reference<GLGE::Graphic::Backend::Graphic::VertexLayout> getLayout() const noexcept
        {return m_layout;}

    protected:

        /**
         * @brief store a reference to the vertex layout
         */
        Reference<GLGE::Graphic::Backend::Graphic::VertexLayout> m_layout;

    };

}

#endif