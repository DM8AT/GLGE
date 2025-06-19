/**
 * A package is similar to a library
 *
 * This package's purpose is to add access to textures and images
 */

//header guard (prevents double inclusion)
#ifndef _GLGE_TEXTURES_
#define _GLGE_TEXTURES_

/**
 * store all textures GLGE has loaded
*/
layout (std430, binding = 0) buffer glge_Textures
{
    /**
     * a list of all textures
     * index by the texture's identifyer
    */
    uvec2 glge_Texture[];
};

/**
 * store all images GLGE has loaded
*/
layout (std430, binding = 1) buffer glge_Images
{
    /**
     * a list of all images
     * index by the texture's identifyer
    */
    uvec2 glge_Image[];
};

#endif //end of header guard