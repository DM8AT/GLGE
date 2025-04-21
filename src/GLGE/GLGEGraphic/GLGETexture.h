/**
 * @file GLGETexture.cpp
 * @author DM8AT
 * @brief define the textures for GLGE
 * @version 0.1
 * @date 2025-04-19
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_TEXTURE_
#define _GLGE_TEXTURE_

//include the settings
#include "../GLGECore/GLGESettings.h"
//include the graphic API
#include "GraphicAPI/GLGEGraphicAPI.h"
//include instances
#include "../GLGECore/GLGEInstance.h"
//include colors
#include "GLGEColor.h"

//check if this is C++
#if GLGE_CPP

/**
 * @brief define a simple texture
 */
class Texture : public InstAttachableClass
{
public:

    /**
     * @brief Construct a new Texture
     */
    Texture() = default;

    /**
     * @brief Construct a new Texture
     * 
     * @param file the file for the texture
     * @param alpha store if a alpha channel should be used
     * @param instance the instance the texture will belong to
     */
    Texture(std::string_view file, bool alpha, Instance& instance) : InstAttachableClass(&instance, file) {create(file, alpha);}

    /**
     * @brief set the texture by loading a file
     * 
     * @param file the file for the texture
     * @param alpha store if a alpha channel should be used
     */
    void create(std::string_view file, bool alpha);

    /**
     * @brief Construct a new blank Texture
     * ew Graphic Texture
     * 
     * @param purpose the purpose of the texture (isHDR and alpha are only used for TEXTURE_PURPOSE_IMAGE and TEXTURE_PURPOSE_RENDER)
     * @param size the size of the texture in pixels
     * @param isHDR true : the texture will be in high dynamic range (all components are 32 bit floats) | false : the texture will be in low dynamic range (all components are 8 bit unsigned integers)
     * @param alpha specify if a alpha channel should exist or not
     * @param instance the instance the texture will belong to
     */
    Texture(TexturePurpose purpose, const uvec2& size, bool isHDR, bool alpha, Instance& instance) : InstAttachableClass(&instance, "dynamic_texture") {create(purpose, size, isHDR, alpha);}

    /**
     * @brief set the texture to a blank texture
     * ew Graphic Texture
     * 
     * @param purpose the purpose of the texture (isHDR and alpha are only used for TEXTURE_PURPOSE_IMAGE and TEXTURE_PURPOSE_RENDER)
     * @param size the size of the texture in pixels
     * @param isHDR true : the texture will be in high dynamic range (all components are 32 bit floats) | false : the texture will be in low dynamic range (all components are 8 bit unsigned integers)
     * @param alpha specify if a alpha channel should exist or not
     */
    void create(TexturePurpose purpose, const uvec2& size, bool isHDR, bool alpha);

    /**
     * @brief Get the size of the texture
     * 
     * @return const uvec2& the size of the texture in pixels
     */
    inline const uvec2& getSize() const noexcept {return m_texture->getSize();}

    /**
     * @brief Get the Color of a pixel
     * 
     * @param pos the position of the pixel
     * @return Color the color of the pixel
     */
    Color getColor(const uvec2& pos);

    /**
     * @brief Set the Color of a specific pixel
     * 
     * @param pos 
     * @param color 
     */
    void setColor(const uvec2& pos, const Color& color);

    /**
     * @brief Create the mip chain for the texture
     */
    inline void createMipMaps() {m_texture->createMipMap();}

protected:

    /**
     * @brief store the graphic texture
     */
    GraphicTexture* m_texture = 0;
    /**
     * @brief store if the texture should reupload
     */
    bool upload = false;

};

#endif //C++ section

#endif