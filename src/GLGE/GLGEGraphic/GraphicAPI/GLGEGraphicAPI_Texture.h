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
    TEXTURE_PURPOSE_IMAGE = 0,
    /**
     * @brief state that the purpose of the texture is to be used as a target for rendering either through compute shaders (if they exist) or as a framebuffer color attatchment
     */
    TEXTURE_PURPOSE_RENDER,
    /**
     * @brief state that the purpose of the texture is to be used as a depth buffer
     */
    TEXTURE_PURPOSE_DEPTH,
    /**
     * @brief state that the purpose of the texture is to be only used on the CPU
     * @warning this disquallifies the texture from all render-related usage
     */
    TEXTURE_PURPOSE_CPU_ONLY
} TexturePurpose;

/**
 * @brief convert the texture purpose to a string
 * 
 * @return const char* the C-String
 */
const char* GLGE_C_FUNC(texturePurposeToString(TexturePurpose purpose));

//check for C++
#if GLGE_CPP

//say that textures will exist
class Texture;

/**
 * @brief print a texture purpose to an output stream
 * 
 * @param os the output stream to print to
 * @param purp the texture purpose to print
 * @return std::ostream& the filled output stream
 */
inline std::ostream& operator<<(std::ostream& os, const TexturePurpose& purp) noexcept {return os << GLGE_C_FUNC(texturePurposeToString)(purp);}

/**
 * @brief store a graphic texture
 */
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
     * @param texture a pointer to the parent texture
     * @param data the data for the texture
     * @param isHDR specify the texture format
     * @param alpha store if a alpha channel should be used
     * @param instance store a pointer to the graphic instance the texture belongs to
     */
    inline GraphicTexture(Texture* texture, void* data, bool isHDR, bool alpha, GraphicInstance* instance) {create(texture, data, isHDR, alpha, instance);}

    /**
     * @brief Construct a new Graphic Texture
     * 
     * @param texture a pointer to the parent texture
     * @param isHDR true : the texture will be in high dynamic range (all components are 32 bit floats) | false : the texture will be in low dynamic range (all components are 8 bit unsigned integers)
     * @param alpha specify if a alpha channel should exist or not
     * @param instance store a pointer to the graphic instance the texture belongs to
     */
    inline GraphicTexture(Texture* texture, bool isHDR, bool alpha, GraphicInstance* instance) {create(texture, isHDR, alpha, instance);};

    /**
     * @brief Destroy the Graphic Texture
     */
    inline virtual ~GraphicTexture() {destroy();}

    /**
     * @brief Construct a new Graphic Texture
     * 
     * @param texture a pointer to the parent texture
     * @param data the data for the texture
     * @param isHDR specify the texture format
     * @param alpha store if a alpha channel should be used
     * @param instance store a pointer to the graphic instance the texture belongs to
     */
    virtual void create(Texture* texture, void* data, bool isHDR, bool alpha, GraphicInstance* instance);

    /**
     * @brief Construct a new Graphic Texture
     * 
     * @param texture a pointer to the parent texture
     * @param isHDR true : the texture will be in high dynamic range (all components are 32 bit floats) | false : the texture will be in low dynamic range (all components are 8 bit unsigned integers)
     * @param alpha specify if a alpha channel should exist or not
     * @param instance store a pointer to the graphic instance the texture belongs to
     */
    virtual void create(Texture* texture, bool isHDR, bool alpha, GraphicInstance* instance);

    /**
     * @brief re-create the texture with entierly new data and potentialy a new size and format
     * 
     * @param data the new data for the texture
     * @param isHDR true : the texture will be in high dynamic range (all components are 32 bit floats) | false : the texture will be in low dynamic range (all components are 8 bit unsigned integers)
     * @param alpha specify if a alpha channel should exist or not
     */
    virtual void recreate(void*, bool, bool) noexcept {}

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
     * @brief Create the mip chain for the texture
     */
    virtual void createMipMap() noexcept {}

    /**
     * @brief get if the texture has an alpha channel
     * 
     * @return true : the texture has an alpha channel
     * @return false : the texture has no alpha channel
     */
    inline bool hasAlpha() const noexcept {return m_hasAlpha;}

    /**
     * @brief Get the parent texture
     * 
     * @return Texture* a pointer to the parent texture
     */
    inline Texture* getTexture() noexcept {return m_texture;}

    /**
     * @brief say that the texture could be about to be sampled (for rendering)
     */
    virtual void activate() noexcept {}

    /**
     * @brief say that the texture can no longer be sampled (for rendering)
     */
    virtual void deactivate() noexcept {}

    /**
     * @brief this resizes AND CLEARS TO BLACK the texture
     * 
     * @param newSize the new size for the texture
     */
    virtual void resize(const uvec2&) noexcept {}

    /**
     * @brief DIRECTLY set the size of the parent texture
     * @warning DO NOT USE DIRECTLY
     * 
     * @param size the new size of the texture
     */
    void setParentSize(const uvec2& size) noexcept;

    /**
     * @brief Get the texture's identifyer (the index in the instance's texture array)
     * 
     * @return uint64_t the number used to identify the texture
     */
    virtual uint64_t getIdentifyer() const noexcept {return 0;}

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
     * @brief store the parent texture
     */
    Texture* m_texture = 0;

};

#endif

#endif