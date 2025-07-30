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

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec2 v_tex;
layout (location = 2) in vec3 v_normal;

/**
 * @brief the layout of a single camera element
 */
struct Camera
{
    float fov;
    float near;
    float far;

    //spacer
    float buff;

    mat4 view;
    mat4 invView;
    mat4 rotation;
    mat4 projection;
    mat4 invProjection;
};

/**
 * store all the cameras
 */
layout (std430, binding = 3) readonly buffer glge_Cameras
{
    /**
     * a list of all cameras
    */
    Camera glge_Camera[];
};

struct ObjectTransform
{
    mat4 toWorld;
    mat4 invToWorld;
    mat4 rotation;
};

/**
 * stores the index of the object to render.
 * @warning if no object is being renderd and this uniform is accessed it results in undefined behaviour
 */
uniform int glge_ObjectIndex;

/**
 * store all the object transforms
 */
layout (std430, binding = 4) readonly buffer glge_ObjTransforms
{
    /**
     * a list of all object transformations
    */
    ObjectTransform glge_ObjTransform[];
};

layout (location = 0) out vec3 f_normal;
layout (location = 1) out vec2 f_tex;

@main (stage_vertex)
void main()
{
    vec4 pos = vec4(v_pos, 1);
    pos = pos * glge_ObjTransform[glge_ObjectIndex].toWorld;
    pos = pos * glge_Camera[0].view;
    gl_Position = pos * glge_Camera[0].projection;

    vec4 norm = vec4(v_normal, 1);
    f_normal = norm.xyz;

    f_tex = v_tex;
}

#elif (GLGE_SHADER_TYPE == 1)

const vec3 lightDir = normalize(vec3(0, -1, 0));
const vec3 lightCol = vec3(1);
const float lightStren = 1;
const float ambient = 0.1;
const vec3 ambientColor = vec3(0.5);

/**
 * @brief store the structure of a single material element
 */
struct Material
{
    uint tex;
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

layout (location = 0) in vec3 f_normal;
layout (location = 1) in vec2 f_tex;

@main (stage_fragment)
void main()
{
    //always use the first material
    Material mat = glge_Material[0];

    vec3 light = vec3(0);
    float lStren = (dot(f_normal, -lightDir) * 0.5 + 0.5) * lightStren;
    light += lightCol * lStren;
    light += ambientColor * ambient;

    vec4 color = texture(sampler2D(glge_Texture[int(mat.tex)]), f_tex);
    Color = vec4(color.xyz * light, color.w);;
}

#else

#error Undefined shader type

#endif