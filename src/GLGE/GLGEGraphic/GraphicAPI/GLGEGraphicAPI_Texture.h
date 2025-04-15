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
     * @param size specify the size of the texture in pixels
     * @param instance store a pointer to the graphic instance the texture belongs to
     */
    inline GraphicTexture(void* data, bool isHDR, const uvec2& size, GraphicInstance* instance) {create(data, isHDR, size, instance);}

    /**
     * @brief Destroy the Graphic Texture
     */
    inline virtual ~GraphicTexture() {destroy();}

    /**
     * @brief Construct a new Graphic Texture
     * 
     * @param data the data for the texture
     * @param isHDR specify the texture format
     * @param size specify the size of the texture in pixels
     * @param instance store a pointer to the graphic instance the texture belongs to
     */
    void create(void* data, bool isHDR, const uvec2& size, GraphicInstance* instance);

    /**
     * @brief destroy the texture
     */
    void destroy();

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
    inline void* data() noexcept {return m_data;}

    /**
     * @brief Get the Size of the texture
     * 
     * @return const uvec2& the size of the texture
     */
    inline const uvec2& getSize() const noexcept {return m_size;}

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
     * @brief store the data of the image in the format specified by `m_isHDR`
     */
    void* m_data;
    /**
     * @brief store the size of the image in pixels
     */
    uvec2 m_size;

};

#endif

#endif