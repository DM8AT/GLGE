/**
 * @file GLGE_OGL4_6_Texture.h
 * @author DM8AT
 * @brief define a texture for a OpenGL 4.6
 * @version 0.1
 * @date 2025-04-19
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_OGL_4_6_TEXTURE_
#define _GLGE_OGL_4_6_TEXTURE_

//include the window
#include "../../GLGEGraphicAPI_Window.h"

//check if this is C++
#if GLGE_CPP

/**
 * @brief define the texture for OpenGL 4.6
 */
class OGL4_6_Texture : public GraphicTexture
{
public:

    /**
     * @brief Construct a new OpenGL 4.6 texture
     */
    OGL4_6_Texture() = default;

    /**
     * @brief Construct a new OpenGL 4.6 texture
     * 
     * @param data the data for the texture
     * @param isHDR the format of the data (hdr = vec4, not hdr = uint8_t[4])
     * @param alpha store if a alpha channel should be used
     * @param size the size of the texture in pixels
     * @param instance the graphic instance the texture will belong to
     */
    inline OGL4_6_Texture(void* data, bool isHDR, bool alpha, const uvec2& size, GraphicInstance* instance) : GraphicTexture(data, isHDR, alpha, size, instance) {onCreate();}

    /**
     * @brief Construct a new Graphic Texture
     * 
     * @param purpose the purpose of the texture (isHDR and alpha are only used for TEXTURE_PURPOSE_IMAGE and TEXTURE_PURPOSE_RENDER)
     * @param size the size of the texture in pixels
     * @param isHDR true : the texture will be in high dynamic range (all components are 32 bit floats) | false : the texture will be in low dynamic range (all components are 8 bit unsigned integers)
     * @param alpha specify if a alpha channel should exist or not
     * @param instance store a pointer to the graphic instance the texture belongs to
     */
    inline OGL4_6_Texture(TexturePurpose purpose, const uvec2& size, bool isHDR, bool alpha, GraphicInstance* instance) : GraphicTexture(purpose, size, isHDR, alpha, instance) {onCreate();}

    /**
     * @brief create the OpenGL texture
     */
    virtual void onCreate() override;

    /**
     * @brief delete the OpenGL texture
     */
    virtual void onDestroy() override;

    virtual void createMipMap() noexcept override;

protected:

    /**
     * @brief Create a OpenGL texture
     * 
     * @param data the data inputed to the function
     * @param dataSize the size of the inputed data
     */
    static void createOgl(void* data, uint64_t dataSize);

    /**
     * @brief Delete the OpenGL texture
     * 
     * @param data the data inputed to the function
     * @param dataSize the size of the inputed data
     */
    static void deleteOgl(void* data, uint64_t dataSize);

    /**
     * @brief Create the mip chain for an OpenGL texture
     * 
     * @param data the data inputed to the function
     * @param dataSize the size of the inputed data
     */
    static void mipMap(void* data, uint64_t dataSize);

    /**
     * @brief store the actual OpenGL texture
     */
    uint32_t m_tex = 0;

};

#endif //C++ section

#endif