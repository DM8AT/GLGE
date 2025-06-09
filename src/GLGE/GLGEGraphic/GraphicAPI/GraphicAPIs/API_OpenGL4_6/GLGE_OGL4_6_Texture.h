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
     * @param texture a pointer to the parent texture
     * @param data the data for the texture
     * @param isHDR the format of the data (hdr = vec4, not hdr = uint8_t[4])
     * @param alpha store if a alpha channel should be used
     * @param instance the graphic instance the texture will belong to
     */
    inline OGL4_6_Texture(Texture* texture, void* data, bool isHDR, bool alpha, GraphicInstance* instance) : GraphicTexture(texture, data, isHDR, alpha, instance) {onCreate();}

    /**
     * @brief Construct a new Graphic Texture
     * 
     * @param texture a pointer to the parent texture
     * @param isHDR true : the texture will be in high dynamic range (all components are 32 bit floats) | false : the texture will be in low dynamic range (all components are 8 bit unsigned integers)
     * @param alpha specify if a alpha channel should exist or not
     * @param instance store a pointer to the graphic instance the texture belongs to
     */
    inline OGL4_6_Texture(Texture* texture, bool isHDR, bool alpha, GraphicInstance* instance) : GraphicTexture(texture, isHDR, alpha, instance) {onCreate();}

    /**
     * @brief create the OpenGL texture
     */
    virtual void onCreate() override;

    /**
     * @brief delete the OpenGL texture
     */
    virtual void onDestroy() override;

    /**
     * @brief Create a mip map for the texture
     */
    virtual void createMipMap() noexcept override;

    /**
     * @brief Get the OpenGL texture identifyer
     * 
     * @return uint32_t the OpenGL texture identifyer
     */
    inline uint32_t getOglTexture() const noexcept {return m_tex;}

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