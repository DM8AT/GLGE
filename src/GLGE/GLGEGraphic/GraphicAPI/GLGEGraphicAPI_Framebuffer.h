/**
 * @file GLGEGraphicAPI_Framebuffer.h
 * @author DM8AT
 * @brief define the API for using framebuffers
 * @version 0.1
 * @date 2025-04-15
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_GRAPHIC_API_FRAMEBUFFER_
#define _GLGE_GRAPHIC_API_FRAMEBUFFER_

//include the settings
#include "../../GLGECore/GLGESettings.h"
//include the graphic instance
#include "GLGEGraphicAPI_Instance.h"
//include the textures
#include "GLGEGraphicAPI_Texture.h"

//check if this is C++
#if GLGE_CPP

/**
 * @brief define the API for the framebuffer
 */
class GraphicFramebuffer : public GraphicInstAttatchable
{
public:

    /**
     * @brief Construct a new Graphic Framebuffer
     */
    GraphicFramebuffer() = default;

    /**
     * @brief Construct a new Graphic Framebuffer
     * 
     * @param textures an array of textures for the framebuffer
     * @param textureCount the amount of textures in the array
     * @param depthBuffer a pointer to a depth buffer or 0
     * @param instance a pointer to the instance the framebuffer will belong to
     */
    inline GraphicFramebuffer(GraphicTexture* textures, uint64_t textureCount, GraphicTexture* depthBuffer, GraphicInstance* instance)
    {create(textures, textureCount, depthBuffer, instance);}

    /**
     * @brief create a new framebuffer
     * 
     * @param textures an array of textures for the framebuffer
     * @param textureCount the amount of textures in the array
     * @param depthBuffer a pointer to a depth buffer or 0
     * @param instance a pointer to the instance the framebuffer will belong to
     */
    void create(GraphicTexture* textures, uint64_t textureCount, GraphicTexture* depthBuffer, GraphicInstance* instance);

    /**
     * @brief destroy a framebuffer
     */
    void destroy();

    /**
     * @brief make this the current active framebuffer
     */
    virtual void onAttatch() {}

    /**
     * @brief Get a specific texture from the framebuffer
     * 
     * @param idx the index of the texture
     * @return GraphicTexture* a pointer to the specific texture
     */
    inline GraphicTexture* getTexture(uint64_t idx) noexcept {return m_textures[idx];}

    /**
     * @brief Get the amount of color targets
     * 
     * @return uint64_t the amount of color targets
     */
    inline uint64_t getTextureCount() const noexcept {return m_textures.size();}

    /**
     * @brief Get the Depth Buffer
     * 
     * @return GraphicTexture* a pointer to the depth buffer
     */
    inline GraphicTexture* getDepthBuffer() noexcept {return m_depthBuffer;}

protected:

    /**
     * @brief a overloadable hook to create the framebuffer
     */
    virtual void onCreate() {}
    /**
     * @brief a overloadable hook to destroy the framebuffer
     */
    virtual void onDestroy() {}

    /**
     * @brief store all the attatched textures
     */
    std::vector<GraphicTexture*> m_textures;
    /**
     * @brief store a pointer to the depth buffer texture
     */
    GraphicTexture* m_depthBuffer = 0;

};

#endif //C++ section

#endif