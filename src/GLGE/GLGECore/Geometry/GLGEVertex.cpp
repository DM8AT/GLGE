/**
 * @file GLGEVertex.cpp
 * @author DM8AT
 * @brief implement the vertex part of the geometry subpacket
 * @version 0.1
 * @date 2025-04-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include vertices
#include "GLGEVertex.h"

VertexLayout::VertexLayout(VertexLayoutElement* elements, uint64_t elementCount)
{
    //reserve the space for the elements
    m_elementOrderd.reserve(elementCount);
    m_sizeMap.reserve(elementCount);
    //set the total size to 0
    m_vertexSize = 0;
    //iterate over all vertex elements
    for (uint64_t i = 0; i < elementCount; ++i)
    {
        //create a new order index
        m_elementOrderd.emplace_back(elements[i].name);
        //add the size of the element
        m_sizeMap[elements[i].name] = elements[i].size;
        //add the size to the overall size
        m_vertexSize += elements[i].size;
    }
}

//define the print function
std::ostream& operator<<(std::ostream& os, VertexLayout& l) noexcept
{
    //print the header
    os << "vertexLayout{elements: {\n";
    //iterate over all elements
    for (uint64_t i = 0; i < l.m_elementOrderd.size(); ++i)
    {
        //print the element
        os << "    " << i << " : \"" << l.m_elementOrderd[i] << "\", " << l.m_sizeMap[l.m_elementOrderd[i]] << "\n";
    }
    //print the overall size and return 
    return os << "} vertexSize: " << l.m_vertexSize << "}";
}