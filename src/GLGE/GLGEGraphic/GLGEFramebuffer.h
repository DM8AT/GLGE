/**
 * @file GLGEFramebuffer.h
 * @author DM8AT
 * @brief define framebuffers for GLGE
 * @version 0.1
 * @date 2025-04-19
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_FRAMEBUFFER_
#define _GLGE_FRAMEBUFFER_

//include the settings
#include "../GLGECore/GLGESettings.h"
//include textures
#include "GLGETexture.h"

//check if this is C++
#if GLGE_CPP

/**
 * @brief store a set of textures that can be renderd to and read from
 */
class Framebuffer : public InstAttachableClass
{
public:

    Framebuffer() = default;

    /**
     * @brief Construct a new Framebuffer
     * 
     * @param colorAttatchments an array of color attatchments (all textures must have purpose TEXTURE_PURPOSE_RENDER)
     * @param colorAttatchmentCount the amount of color attatchments in the array
     * @param depthAttatchment a pointer to a texture for a depth attatchment (purpose must be TEXTURE_PURPOSE_DEPTH)
     * @param instance a reference to the instance the framebuffer will belong to
     */
    inline Framebuffer(Texture** colorAttatchments, uint64_t colorAttatchmentCount, Texture* depthAttatchment, Instance& instance)
     : InstAttachableClass(&instance, "Framebuffer")
    {
        //create the framebuffer
        create(colorAttatchments, colorAttatchmentCount, depthAttatchment);
    }

    /**
     * @brief Construct a new Framebuffer
     * 
     * @param colorAttatchments an array of color attatchments (all textures must have purpose TEXTURE_PURPOSE_RENDER)
     * @param colorAttatchmentCount the amount of color attatchments in the array
     * @param depthAttatchment a pointer to a texture for a depth attatchment (purpose must be TEXTURE_PURPOSE_DEPTH)
     */
    void create(Texture** colorAttatchments, uint64_t colorAttatchmentCount, Texture* depthAttatchment);

    /**
     * @brief Construct a new Framebuffer
     * 
     * @param colorAttatchmentCount the amount of color attatchments
     * @param alpha specify if all textures should have alpha channels
     * @param hdr specify if all textures should be in HDR or LDR
     * @param depthAttatchment the depth attatchment
     * @param size the size of the framebuffer
     * @param instance a reference to the instance the framebuffer will belong to
     */
    inline Framebuffer(uint64_t colorAttatchmentCount, bool alpha, bool hdr, bool depthAttatchment, const uvec2& size, Instance& instance)
     : InstAttachableClass(&instance, "Framebuffer")
    {
        //create the framebuffer
        create(colorAttatchmentCount, alpha, hdr, depthAttatchment, size);
    }

    /**
     * @brief Destroy the Framebuffer
     */
    ~Framebuffer();

    /**
     * @brief Construct a new Framebuffer
     * 
     * @param colorAttatchmentCount the amount of color attatchments
     * @param alpha specify if all textures should have alpha channels
     * @param hdr specify if all textures should be in HDR or LDR
     * @param depthAttatchment the depth attatchment
     * @param size the size of the framebuffer
     */
    void create(uint64_t colorAttatchmentCount, bool alpha, bool hdr, bool depthAttatchment, const uvec2& size);

    /**
     * @brief Get the size of the framebuffer
     * 
     * @return const uvec2& the size of the framebuffer in pixels
     */
    inline const uvec2& getSize() const noexcept {return ((m_colorAttatchments.size()) ? m_colorAttatchments[0]->getSize() : m_depthAttatchment->getSize());}

    /**
     * @brief print a framebuffer
     * 
     * @param os the output stream to print to
     * @param fbuff the framebuffer to print
     * @return std::ostream& the filled output stream
     */
    friend std::ostream& operator<<(std::ostream& os, const Framebuffer& fbuff) noexcept;

    /**
     * @brief Get a specific color attatchment
     * 
     * @param idx the index of the color attatchment
     * @return Texture* a pointer to the attatched texture
     */
    inline Texture* getColorAttatchment(uint64_t idx) noexcept {return m_colorAttatchments[idx];}

    /**
     * @brief Get the Amount of color attatchments of the framebuffer
     * 
     * @return uint64_t the amount of attatched color attatchments
     */
    inline uint64_t getColorAttatchmentCount() const noexcept {return m_colorAttatchments.size();}

    /**
     * @brief Get the Depth attatchment of this framebuffer
     * 
     * @return Texture* a pointer to the depth attatchment of the framebuffer
     */
    inline Texture* getDepthAttatchment() noexcept {return m_depthAttatchment;}

    /**
     * @brief Set the clear color of the window
     * 
     * @param r the red component of the clear color
     * @param g the green component of the clear color
     * @param b the blue component of the clear color
     * @param a the alpha component of the clear color
     */
    inline void setClearColor(float r, float g, float b, float a) noexcept {m_clear = Color(r,g,b,a, COLOR_SPACE_RGBA);}

    /**
     * @brief Set the clear color of the window
     * 
     * @param color the color in RGBA for the clear color
     */
    inline void setClearColor(const vec4& color) noexcept {m_clear = Color(color, COLOR_SPACE_RGBA);}

    /**
     * @brief Set the clear color of the window
     * 
     * @param color the clear color of the window
     */
    inline void setClearColor(const Color& color) noexcept {m_clear = color;}

    /**
     * @brief Get the clear color of the window
     * 
     * @return const Color& the clear color of the window
     */
    inline const Color& getClearColor() const noexcept {return m_clear;}

    /**
     * @brief Get the Graphic Framebuffer instance
     * @warning ONLY USE IF YOU KNOW WHAT YOU'RE DOING
     * 
     * @return GraphicFramebuffer* a pointer to the graphic framebuffer
     */
    inline GraphicFramebuffer* getGraphicFramebuffer() noexcept {return m_fbuff;}

    /**
     * @brief change the size of the framebuffer
     * 
     * @param newSize the new size of the framebuffer
     */
    inline void resize(const uvec2& newSize) noexcept {m_fbuff->resize(newSize);}

protected:

    /**
     * @brief store all color attatchments
     */
    std::vector<Texture*> m_colorAttatchments;
    /**
     * @brief store the depth attatchments
     */
    Texture* m_depthAttatchment = 0;
    /**
     * @brief store the own clear color
     */
    Color m_clear = Color(vec4(0.5,0.5,0.5,1), COLOR_SPACE_RGBA);

    /**
     * @brief store the graphic framebuffer
     */
    GraphicFramebuffer* m_fbuff = 0;

};

//start a C-Section
extern "C" {
#endif //C++ section

/**
 * @brief store an instance of the framebuffer class
 */
typedef struct s_Framebuffer Framebuffer_t;

//check if a C-Section should be ended
#if GLGE_CPP
}
#endif //End of C-Section

#endif