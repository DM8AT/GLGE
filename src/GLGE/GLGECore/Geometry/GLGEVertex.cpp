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

constexpr bool VertexLayout::operator==(VertexLayout& other) noexcept
{
    //quick-check: if the sizes of the vertices don't match, they can't be equal. 
    if (m_vertexSize != other.m_vertexSize) {return false;}
    //if the amount of elements dosn't match, both can't be identical
    if (m_elementOrderd.size() != other.m_elementOrderd.size()) {return false;}

    //iterate over all elements of the vertex
    for (uint64_t i = 0; i < m_elementOrderd.size(); ++i)
    {
        //check if the orderd elements match
        if (m_sizeMap[m_elementOrderd[i]] != other.m_sizeMap[other.m_elementOrderd[i]])
        {
            //the element's don't match -> the vertex layout's can't match
            return false;
        }
    }

    //all elements match -> vertex layouts match
    return true;
}