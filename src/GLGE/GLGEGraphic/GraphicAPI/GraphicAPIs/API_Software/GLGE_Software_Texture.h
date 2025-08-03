/**
 * @file GLGE_Software_Texture.h
 * @author JuNi4
 * @brief 
 * @version 0.1
 * @date 2025-06-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_SOFTWARE_TEXTURE_
#define _GLGE_SOFTWARE_TEXTURE_

//include the window
#include "../../GLGEGraphicAPI_Window.h"
//include memory arenas

//check if this is C++
#if GLGE_CPP

//say that OpenGL framebuffers will exist
class Software_Framebuffer;

/**
 * @brief define the texture for the software renderer
 */
class Software_Texture : public GraphicTexture
{
public:
    /**
     * @brief Construct a new software texture
     */
    Software_Texture() = default;

    /**
     * @brief Construct a new software texture
     * 
     * @param texture a pointer to the parent texture
     * @param data the data for the texture
     * @param isHDR the format of the data (hdr = vec4, not hdr = uint8_t[4])
     * @param alpha store if a alpha channel should be used
     * @param instance the graphic instance the texture will belong to
     */
    inline Software_Texture(Texture* texture, void* data, bool isHDR, bool alpha, GraphicInstance* instance) : GraphicTexture(texture, data, isHDR, alpha, instance) {onCreate();}

    /**
     * @brief Construct a new Graphic Texture
     * 
     * @param texture a pointer to the parent texture
     * @param isHDR true : the texture will be in high dynamic range (all components are 32 bit floats) | false : the texture will be in low dynamic range (all components are 8 bit unsigned integers)
     * @param alpha specify if a alpha channel should exist or not
     * @param instance store a pointer to the graphic instance the texture belongs to
     */
    inline Software_Texture(Texture* texture, bool isHDR, bool alpha, GraphicInstance* instance) : GraphicTexture(texture, isHDR, alpha, instance) {onCreate();}

    virtual void onCreate() override;

    /**
     * @brief this resizes AND CLEARS TO BLACK the texture
     * 
     * @param newSize the new size for the texture
     */
    virtual void resize(const uvec2&) noexcept override;
};

#endif //c++ section
#endif