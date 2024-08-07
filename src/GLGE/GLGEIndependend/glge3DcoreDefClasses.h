/**
 * @file glge3DcoreDefClasses.h
 * @author DM8AT
 * @brief define the graphics api independand classes for the 3D Core
 * @version 0.1
 * @date 2024-01-09
 * 
 * @copyright Copyright DM8AT 2023. All rights reserved. This project is released under the MIT license.
 */

#ifndef _GLGE_INTERNAL_3D_CORE_DEF_CLASSES_H_
#define _GLGE_INTERNAL_3D_CORE_DEF_CLASSES_H_

//includes from the default C++ lib
#include <vector>
#include <string>

//include the needed components from CML
//vectors
#include "../CML/CMLVec3.h"
#include "../CML/CMLVec2.h"
//matrices
#include "../CML/CMLMat4.h"

//public defines

/**
 * @brief say the type of an model file
 */
#define GLGE_OBJ 1

//define the preset inputs

//check if the input for texture coordinates was allready defined
#ifndef GLGE_PRESET_USE_TEXTURE_COORDINATES
/**
 * @brief define the input for texture coordinates
 */
#define GLGE_PRESET_USE_TEXTURE_COORDINATES vec4(-1)
#endif //GLGE_PRESET_USE_TEXTURE_COORDINATES generation
//check if the input for a special function was allready defined
#ifndef GLGE_PRESET_USE_SPECIAL
/**
 * @brief define the input for presets used for special things (like color gradients)
 */
#define GLGE_PRESET_USE_SPECIAL vec4(-2)
#endif //GLGE_PRESET_USE_SPECIAL define

//define the presets

//check if a define for an empty object allreday exists
#ifndef GLGE_PRESET_EMPTY
/**
 * @brief generate an empty object without any mesh
 */
#define GLGE_PRESET_EMPTY 0
#endif //GLGE_PRESET_EMPTY creation
/**
 * @brief genearte an object with an cube as the mesh
 */
#define GLGE_PRESET_CUBE 1
/**
 * @brief genearte an object with an plane as the mesh
 */
#define GLGE_PRESET_PLANE 2
/**
 * @brief check if the circle allready exists
 */
#ifndef GLGE_PRESET_CIRCLE
/**
 * @brief genearte an object with an circle as the mesh
 */
#define GLGE_PRESET_CIRCLE 3
#endif // GLGE_PRESET_CIRCLE generation
/**
 * @brief genearte an object with an sphere as the mesh
 */
#define GLGE_PRESET_SPHERE 4
/**
 * @brief genearte an object with an icosphere as the mesh
 */
#define GLGE_PRESET_ICOSPHERE 5
/**
 * @brief genearte an object with an cylinder as the mesh
 */
#define GLGE_PRESET_CYLINDER 6
/**
 * @brief genearte an object with an cone as the mesh
 */
#define GLGE_PRESET_CONE 7

///////////
//STRUCTS//
///////////

/**
 * @brief a variable type to store position, rotation and scaling information for an object
 * 
 * the transform type is used by objects of the GLGE library to capsule all the movement operations in one varaiable, to not have a lot
 * of different variables to have in an object. 
 */
struct Transform
{
    vec3 pos;
    vec3 rot;
    vec3 scale = vec3(1,1,1);

    /**
     * @brief Construct a new Transform
     * 
     * default constructor
     */
    Transform();

    /**
     * @brief Construct a new Transform
     * 
     * @param position the position data stored in the transform
     * @param rotation the rotation stored in the transform
     * @param scale the size information stored in the transform
     */
    Transform(vec3 position, vec3 rotation, vec3 scale = vec3(1,1,1));

    /**
     * @brief Construct a new Transform
     * 
     * @param position the position of the transform
     * @param rotation the rotation of the transform
     * @param scale the scale on all axis of the transform
     */
    Transform(vec3 position, vec3 rotation, float scale = 1);

    /**
     * @brief Get the Matrix to transform an object to this
     * 
     * @return mat4 transformation matrix
     */
    mat4 getMatrix();

    /**
     * @brief Get the Rotation Matrix
     * 
     * @return mat4 the rotation matrix
     */
    mat4 getRotationMatrix();
};

/**
 * @brief this variable stores information about a single vertex of an Object
 */
struct Vertex
{
    //store the vertex position
    vec3 pos;
    //store the vertex color
    vec4 color;
    //store the texture coordinate
    vec2 texCoord = vec2(1,1);
    //store the normal of the vertex
    vec3 normal = vec3(0,0,0);

    /**
     * @brief Construct a new Vertex
     * 
     * default constructor
     */
    Vertex();

    /**
     * @brief Construct a new Vertex
     * 
     * @param pos the positon for the vertex
     * @param color the vertex color
     */
    Vertex(vec3 pos, vec4 color = vec4(1,1,1,1));

    /**
     * @brief Construct a new Vertex
     * 
     * @param x the x positoin of the vertex
     * @param y the y position of the vertex
     * @param z the z position of the vertex
     * @param color the vertex color
     */
    Vertex(float x, float y, float z, vec4 color = vec4(1,1,1,1));

    /**
     * @brief Construct a new Vertex
     * 
     * @param x the x positoin of the vertex
     * @param y the y position of the vertex
     * @param z the z position of the vertex
     * @param r the red part of the color
     * @param g the green part of the color
     * @param b the blue part of the color
     * @param a the alpha of the color, used for transparency
     */
    Vertex(float x, float y, float z, float r, float g, float b, float a);

    /**
     * @brief Construct a new Vertex
     * 
     * @param pos the positon for the vertex
     * @param texCoord the texture coordinate
     */
    Vertex(vec3 pos, vec2 texCoord);

    /**
     * @brief Construct a new Vertex
     * 
     * @param x the x positoin of the vertex
     * @param y the y position of the vertex
     * @param z the z position of the vertex
     * @param texCoord the texture coordinate
     */
    Vertex(float x, float y, float z, vec2 texCoord);

    /**
     * @brief Construct a new Vertex
     * 
     * @param x the x positoin of the vertex
     * @param y the y position of the vertex
     * @param z the z position of the vertex
     * @param tX the texture x coordinate (or: s, v)
     * @param tY the texture y coordinate (or: r, u)
     */
    Vertex(float x, float y, float z, float tX, float tY);

    /**
     * @brief Construct a new Vertex
     * 
     * @param pos the position of the vertex
     * @param normal the normal of the vertex
     */
    Vertex(vec3 pos, vec3 normal);

    /**
     * @brief Construct a new Vertex
     * 
     * @param pos the position of the vertex
     * @param texCoord the texture coordinate of the vertex
     * @param normal the normal of the vertex
     */
    Vertex(vec3 pos, vec2 texCoord, vec3 normal);

    /**
     * @brief Construct a new Vertex from a lot of floats
     * 
     * @param x the x position of the vertex
     * @param y the y position of the vertex
     * @param z the z position of the vertex
     * @param tx the texture x coordinate (or: s, v)
     * @param ty the texture y coordinate (or: r, u)
     * @param nx the normal on the x axis
     * @param ny the normal on the y axis
     * @param nz the normal on the z axis
     */
    Vertex(float x, float y, float z, float tx, float ty, float nx, float ny, float nz);

    /**
     * @brief Construct a new Vertex from 10 floats
     * 
     * @param x the x position of the vertex
     * @param y the y position of the vertex
     * @param z the z position of the vertex
     * @param r the red part of the color
     * @param g the green part of the color
     * @param b the blue part of the color
     * @param a the alpha of the color, used for transparency
     * @param nx the normal on the x axis
     * @param ny the normal on the y axis
     * @param nz the normal on the z axis
     */
    Vertex(float x, float y, float z, float r, float g, float b, float a, float nx, float ny, float nz);

    /**
     * @brief Construct a new Vertex from 6 floats and one vec4
     * 
     * @param x the x position of the vertex
     * @param y the y position of the vertex
     * @param z the z position of the vertex
     * @param color the color for the vertex
     * @param nx the normal on the x axis
     * @param ny the normal on the y axis
     * @param nz the normal on the z axis
     */
    Vertex(float x, float y, float z, vec4 color, float nx, float ny, float nz);
};

#endif