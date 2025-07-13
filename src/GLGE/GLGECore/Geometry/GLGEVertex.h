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
//include vectors
#include "../../GLGEMath/Vectors/GLGEVectors.h"

/**
 * @brief store the simple vertex assumed by meshes if no custom vertex is supplied
 */
typedef struct s_SimpleVertex {
    /**
     * @brief store the position of the vertex
     */
    vec3 pos;
    /**
     * @brief store the texture coordinate
     */
    vec2 tex;
    /**
     * @brief store the normal of the vertex
     */
    vec3 normal;
} SimpleVertex;

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

    /**
     * @brief print the vertex layout into an output stream
     * 
     * @param os the output stream to print to
     * @param l the layout to print
     * @return std::ostream& the filled output stream
     */
    friend std::ostream& operator<<(std::ostream& os, VertexLayout& l) noexcept;

    /**
     * @brief check if two vertex layouts are identical
     * @warning the names of the elements are ignored. If one element has "m_Pos" at index 2, an other layout that has "_Norm" at index 2 is still equal to the first, as long as all the types match up. 
     * 
     * @param other the other element to check
     * @return true : vertex layouts result in the same structure
     * @return false : the vertex layouts do not result in the same structure
     */
    constexpr bool operator==(VertexLayout& other) noexcept;

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