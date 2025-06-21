/**
 * @file GLGE_OGL4_6_Framebuffer.h
 * @author DM8AT
 * @brief define the layout of framebuffers for OpenGL 4.6
 * @version 0.1
 * @date 2025-06-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_OGL_4_6_FRAMEBUFFER_
#define _GLGE_OGL_4_6_FRAMEBUFFER_

//include graphic framebuffers
#include "../../GLGEGraphicAPI_Framebuffer.h"

//check for C++
#if GLGE_CPP

/**
 * @brief store framebuffers for OpenGL 4.6
 * 
 */
class OGL4_6_Framebuffer : public GraphicFramebuffer
{
public:

    /**
     * @brief Construct a new OpenGL 4.6 Framebuffer
     */
    OGL4_6_Framebuffer() = default;

    /**
     * @brief Construct a new framebuffer for OpenGL 4.6
     * 
     * @param fbuff a pointer to the parent framebuffer
     */
    OGL4_6_Framebuffer(Framebuffer* fbuff, Instance* instance)
     : GraphicFramebuffer(fbuff, instance) {}

    /**
     * @brief Destroy the OpenGL framebuffer
     */
    virtual ~OGL4_6_Framebuffer() {destroy();}

    /**
     * @brief a overloadable hook to create the framebuffer
     */
    virtual void onCreate() override;
    /**
     * @brief a overloadable hook to destroy the framebuffer
     */
    virtual void onDestroy() override;

    /**
     * @brief clear the graphic framebuffer
     * 
     * @param cmdBuff the command buffer to queue the clearing in
     */
    virtual void clear(GraphicCommandBuffer* cmdBuff) override;

    /**
     * @brief Get the OpenGL framebuffer encapsulated by this instance
     * 
     * @return uint32_t the OpenGL framebuffer
     */
    inline uint32_t getOpenGLFramebuffer() const noexcept {return m_OglFbuff;}

    /**
     * @brief change the size of the framebuffer
     * @warning this will update the size of all the textures the framebuffer owns
     * 
     * @param newSize the new size for the framebuffer
     */
    virtual void resize(const uvec2& newSize) noexcept;

protected:

    /**
     * @brief create the OpenGL framebuffer
     * 
     * @param fbuff a pointer to the OpenGL framebuffer to create
     */
    static void ogl_create(OGL4_6_Framebuffer* fbuff, uint64_t) noexcept;

    /**
     * @brief delete an OpenGL framebuffer
     * 
     * @param fbuff a pointer to the OpenGL framebuffer to delete
     */
    static void ogl_destroy(OGL4_6_Framebuffer* fbuff, uint64_t) noexcept;

    /**
     * @brief clear the framebuffer
     * 
     * @param fbuff a pointer to the framebuffer to clear
     */
    static void ogl_clear(OGL4_6_Framebuffer* fbuff, uint64_t) noexcept;

    /**
     * @brief change the size of the actual framebuffer object
     * 
     * @param fbuff a pointer to the framebuffer the call came from
     * @param size the size (packed from a uvec2)
     */
    static void ogl_resize(OGL4_6_Framebuffer* fbuff, uint64_t size) noexcept;

    /**
     * @brief store the identifyer of the framebuffer
     */
    uint32_t m_OglFbuff = 0;

};

#endif //end of C++ section

#endif