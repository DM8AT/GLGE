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
     * @param purpose state the purpose of the texture
     * @param instance the instance the texture will belong to
     */
    Texture(std::string_view file, bool alpha, TexturePurpose purpose, Instance& instance) : InstAttachableClass(&instance, file) {create(file, purpose, alpha);}

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
     * @brief Construct a new Texture
     * 
     * @param data the data for the texture
     * @param purpose the purpose of the texture
     * @param size the size of the texture
     * @param isHDR spezify if the texture is in high dynamic range or low dynamic range
     * @param alpha spezify if four or three texture channels exist
     * @param instance a reference to the instance
     */
    Texture(void* data, TexturePurpose purpose, const uvec2& size, bool isHDR, bool alpha, Instance& instance) : InstAttachableClass(&instance, "data_texture") {create(data, purpose, size, isHDR, alpha);}

    /**
     * @brief Destroy the Texture
     */
    ~Texture();

    /**
     * @brief set the texture by loading a file
     * 
     * @param file the file for the texture
     * @param purpose the purpose of the texture
     * @param alpha store if a alpha channel should be used
     */
    void create(std::string_view file, TexturePurpose purpose, bool alpha);

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
     * @brief Load the texture from arbituary data
     * 
     * @param data the data for the texture
     * @param purpose the purpose of the texture
     * @param size the size of the texture
     * @param isHDR spezify if the texture is in high dynamic range or low dynamic range
     * @param alpha spezify if four or three texture channels exist
     */
    void create(void* data, TexturePurpose purpose, const uvec2& size, bool isHDR, bool alpha);

    /**
     * @brief Get the size of the texture
     * 
     * @return const uvec2& the size of the texture in pixels
     */
    inline const uvec2& getSize() const noexcept {return m_size;}

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

    /**
     * @brief Get the purpose of the texture
     * 
     * @return TexturePurpose the purpose of the texture
     */
    inline TexturePurpose getPurpose() const noexcept {return m_purpose;}

    /**
     * @brief Get the graphic texture
     * @warning ONLY USE IF YOU KNOW WHAT YOU'RE DOING
     * 
     * @return GraphicTexture* a pointer to the graphic texture
     */
    inline GraphicTexture* getGraphicTexture() noexcept {return m_texture;}

    /**
     * @brief print the texture
     * 
     * @param os the output stream to print to
     * @param tex the texture to print
     * @return std::ostream& the filled output stream
     */
    inline friend std::ostream& operator<<(std::ostream& os, const Texture& tex) noexcept {return os << "texture{size: " << tex.m_size << ", purpose: " << tex.m_purpose << "}";}

protected:

    /**
     * @brief store the graphic texture
     */
    GraphicTexture* m_texture = 0;
    /**
     * @brief store if the texture should reupload
     */
    bool upload = false;

    /**
     * @brief store the size of the image in pixels
     */
    uvec2 m_size = 0;
    /**
     * @brief store the purpose of the texture
     */
    TexturePurpose m_purpose = TEXTURE_PURPOSE_CPU_ONLY;

};

#endif //C++ section

#endif