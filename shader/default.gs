/**
 * @file GLGERenderableMesh.h
 * @author DM8AT
 * @brief a simple shader for 3D models
 * @version 0.1
 * @date 2025-06-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//this works in all GLGE versions
#version GLGE 0.1.0

//get the textures and images
#include <GLGETextures>

//check for a vertex shader
#if (GLGE_SHADER_TYPE == 0)

struct Vertex
{
    float pos[3];
    float tex[2];
    float norm[3];
};

/**
 * store the vertex data
*/
layout (std430, binding = 3) readonly buffer glge_Vertices
{
    /**
     * a list of all vertices
    */
    Vertex glge_Vertex[];
};

/**
 * store all the indices
 */
layout (std430, binding = 4) readonly buffer glge_Indices
{
    /**
     * a list of all indices
    */
    uint glge_Index[];
};

@main (stage_vertex)
void main()
{
    Vertex current = glge_Vertex[glge_Index[gl_VertexID]];
    gl_Position = vec4(current.pos[0],current.pos[1],current.pos[2], 1);
}

#elif (GLGE_SHADER_TYPE == 1)

/**
 * @brief store the structure of a single material element
 */
struct Material
{
    vec4 color;
    float roughness;
    float metalic;
};

/**
 * store the data of all materials loaded
*/
layout (std430, binding = 2) readonly buffer glge_Materials
{
    /**
     * a list of all material data structures
    */
    Material glge_Material[];
};

layout (location = 0) out vec4 Color;

@main (stage_fragment)
void main()
{
    //always use the first material
    Material mat = glge_Material[0];
    Color = mat.color;
}

#else

#error Undefined shader type

#endif