/**
 * @file GLGEGraphicAPI_Texture.h
 * @author DM8AT
 * @brief implement the abstract texture class to handle textures for all graphic APIs
 * @version 0.1
 * @date 2025-04-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_GRAPHIC_API_TEXTURE_
#define _GLGE_GRAPHIC_API_TEXTURE_

//include the settings
#include "../../GLGECore/GLGESettings.h"

//include instances
#include "GLGEGraphicAPI_Instance.h"
//include vectors
#include "../../GLGEMath/Vectors/GLGEVectors.h"

/**
 * @brief define the purpose of a texture
 */
typedef enum e_TexturePurpose {
    /**
     * @brief state that the purpose of the texture is to be used as a read source for rendering
     */
    TEXTURE_PURPOSE_IMAGE,
    /**
     * @brief state that the purpose of the texture is to be used as a target for rendering either through compute shaders (if they exist) or as a framebuffer color attatchment
     */
    TEXTURE_PURPOSE_RENDER,
    /**
     * @brief state that the purpose of the texture is to be used as a depth buffer
     */
    TEXTURE_PURPOSE_DEPTH
} TexturePurpose;

//check for C++
#if GLGE_CPP

class GraphicTexture : public GraphicInstAttatchable
{
public:

    /**
     * @brief Construct a new Graphic Texture
     */
    GraphicTexture() = default;

    /**
     * @brief Construct a new Graphic Texture
     * 
     * @param data the data for the texture
     * @param isHDR specify the texture format
     * @param alpha store if a alpha channel should be used
     * @param size specify the size of the texture in pixels
     * @param instance store a pointer to the graphic instance the texture belongs to
     */
    inline GraphicTexture(void* data, bool isHDR, bool alpha, const uvec2& size, GraphicInstance* instance) {create(data, isHDR, alpha, size, instance);}

    /**
     * @brief Construct a new Graphic Texture
     * 
     * @param purpose the purpose of the texture (isHDR and alpha are only used for TEXTURE_PURPOSE_IMAGE and TEXTURE_PURPOSE_RENDER)
     * @param size the size of the texture in pixels
     * @param isHDR true : the texture will be in high dynamic range (all components are 32 bit floats) | false : the texture will be in low dynamic range (all components are 8 bit unsigned integers)
     * @param alpha specify if a alpha channel should exist or not
     * @param instance store a pointer to the graphic instance the texture belongs to
     */
    inline GraphicTexture(TexturePurpose purpose, const uvec2& size, bool isHDR, bool alpha, GraphicInstance* instance) {create(purpose, size, isHDR, alpha, instance);};

    /**
     * @brief Destroy the Graphic Texture
     */
    inline virtual ~GraphicTexture() {destroy();}

    /**
     * @brief Construct a new Graphic Texture
     * 
     * @param data the data for the texture
     * @param isHDR specify the texture format
     * @param alpha store if a alpha channel should be used
     * @param size specify the size of the texture in pixels
     * @param instance store a pointer to the graphic instance the texture belongs to
     */
    virtual void create(void* data, bool isHDR, bool alpha, const uvec2& size, GraphicInstance* instance);

    /**
     * @brief Construct a new Graphic Texture
     * 
     * @param purpose the purpose of the texture (isHDR and alpha are only used for TEXTURE_PURPOSE_IMAGE and TEXTURE_PURPOSE_RENDER)
     * @param size the size of the texture in pixels
     * @param isHDR true : the texture will be in high dynamic range (all components are 32 bit floats) | false : the texture will be in low dynamic range (all components are 8 bit unsigned integers)
     * @param alpha specify if a alpha channel should exist or not
     * @param instance store a pointer to the graphic instance the texture belongs to
     */
    virtual void create(TexturePurpose purpose, const uvec2& size, bool isHDR, bool alpha, GraphicInstance* instance);

    /**
     * @brief destroy the texture
     */
    virtual void destroy();

    /**
     * @brief get if the texture is in HDR or LDR format
     * 
     * @return true : the texture is in HDR format
     * @return false : the texture is in LDR format
     */
    inline bool isHDR() const noexcept {return m_isHDR;}

    /**
     * @brief access the raw data of the image
     * 
     * @return void* a pointer to the raw data of the texture
     */
    inline void* getData() noexcept {return m_data;}

    /**
     * @brief Get the Size of the texture
     * 
     * @return const uvec2& the size of the texture
     */
    inline const uvec2& getSize() const noexcept {return m_size;}

    /**
     * @brief Create the mip chain for the texture
     */
    virtual void createMipMap() noexcept {}

    /**
     * @brief Get the purpose of the texture
     * 
     * @return TexturePurpose the purpose of the texture
     */
    inline TexturePurpose getPurpose() const noexcept {return m_purpose;}

    /**
     * @brief get if the texture has an alpha channel
     * 
     * @return true : the texture has an alpha channel
     * @return false : the texture has no alpha channel
     */
    inline bool hasAlpha() const noexcept {return m_hasAlpha;}

protected:

    /**
     * @brief a overloadable hook that is called when the texture is created
     */
    virtual void onCreate() {}
    /**
     * @brief a overloadable hook that is called when the texture is destroyed
     */
    virtual void onDestroy() {}

    /**
     * @brief store if the data is stored in HDR format or in LDR format
     * 
     * HDR : The data is stored as an array of 4 element Floating-Point vectors
     * LDR : The data is stored as an array of 4 element 8 Bit Unsigned Integer vectors
     */
    bool m_isHDR = false;
    /**
     * @brief store if a alpha channel exists
     * 
     * true : all pixels have 4 elements
     * false : all pixels have 3 elements
     */
    bool m_hasAlpha = true;
    /**
     * @brief store the data of the image in the format specified by `m_isHDR`
     */
    void* m_data = 0;
    /**
     * @brief store the size of the image in pixels
     */
    uvec2 m_size = 0;
    /**
     * @brief store the purpose of the texture
     */
    TexturePurpose m_purpose = TEXTURE_PURPOSE_IMAGE;

};

#endif

#endif