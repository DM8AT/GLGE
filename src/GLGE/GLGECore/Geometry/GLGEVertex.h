/**
 * @file GLGEVertex.h
 * @author DM8AT
 * @brief define the layout of vertices for GLGE
 * @version 0.1
 * @date 2025-04-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_VERTEX_
#define _GLGE_VERTEX_

//include the settings
#include "../GLGESettings.h"

/**
 * @brief define a single element of the vertex structure
 */
typedef struct s_VertexLayoutElement {
    /**
     * @brief store the name of the vertex element
     */
    const char* name;
    /**
     * @brief store the size in bytes of the element
     */
    uint64_t size;
} VertexLayoutElement;

//C++ section
#if GLGE_CPP

/**
 * @brief specify the layout of a vertex
 */
class VertexLayout
{
public:

    VertexLayout() = default;

    /**
     * @brief Construct a new Vertex Layout
     * 
     * @param elements the elements of the vertex in order they will be used in
     * @param elementCount the amount of vertex layout elements in the list
     */
    VertexLayout(VertexLayoutElement* elements, uint64_t elementCount);

    /**
     * @brief Get the size of a single vertex according to this layout
     * 
     * @return const uint64_t& the size of a single vertex
     */
    inline const uint64_t& getVertexSize() const noexcept {return m_vertexSize;}

    /**
     * @brief Get the size of a specific element
     * @warning THE QUERRIED ELEMENT MUST BELONG TO THE VERTEX LAYOUT. IF THE ELEMENT DOSN'T EXIST, THIS FUNCTION WILL RESULT IN UNDEFINED BEHAVIOUR
     * 
     * @param element the element to querry
     * @return const uint64_t& the size of that element
     */
    inline const uint64_t& getVertexOf(std::string_view& element) noexcept {return m_sizeMap[element];}

    /**
     * @brief Get all elements that the vertex layout contains
     * 
     * @return const std::vector<std::string_view>& all stored elements
     */
    inline const std::vector<std::string_view>& getElements() const noexcept {return m_elementOrderd;}

    friend std::ostream& operator<<(std::ostream& os, VertexLayout& l) noexcept;

protected:

    /**
     * @brief store the size of all elements
     */
    std::unordered_map<std::string_view, uint64_t> m_sizeMap;
    /**
     * @brief store the orderd of elements
     */
    std::vector<std::string_view> m_elementOrderd;
    /**
     * @brief store the size of a whole vertex
     */
    uint64_t m_vertexSize = 0;

};

#endif //C++ section

#endif