/**
 * @file GLGE_Software_Framebuffer.h
 * @author JuNi4
 * @brief deine the API for using framebuffers
 * @version 0.1
 * @date 2025-06-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_SOFTWARE_FRAMEBUFFER_
#define _GLGE_SOFTWARE_FRAMEBUFFER_

//include instances
#include "../../GLGEGraphicAPI_Framebuffer.h"
//include textures
#include "GLGE_Software_Texture.h"

//check if this is C++
#if GLGE_CPP

class Software_Framebuffer : public GraphicFramebuffer
{
public:

    /**
     * @brief Construct a new framebuffer for the software rasterizer
     */
    Software_Framebuffer() = default;

    /**
     * @brief Construct a new framebuffer for the software rasterizer
     * 
     * @param fbuff a pointer to the parent framebuffer
     * @param instance a pointer to the active graphics instance
     */
    Software_Framebuffer(Framebuffer* fbuff, Instance* instance) : GraphicFramebuffer(fbuff, instance) {}

    /**
     * @brief an overloadable hook to create the framebuffer
     */
    void onCreate() override;

    /**
     * @brief an overloadable hook to destroy the framebuffer
     */
    void onDestroy() override;

    /**
     * @brief clear the graphic framebuffer
     * 
     * @param cmdBuff the command buffer to queue the clearing in
     */
    void clear(GraphicCommandBuffer* cmdBuff) override;

    /**
     * @brief change the size of the framebuffer
     * @warning this will update the size of all the textures the framebuffer owns
     * 
     * @param cmdBuff a pointer to the command buffer the resize will be queued in
     * @param newSize the new size for the framebuffer
     */
    void resize(GraphicCommandBuffer*, const uvec2&) noexcept override;

    /**
     * @brief Get the parent high level frame buffer
     * 
     * @return Framebuffer* The high level parent of the current frame buffer
     */
    inline Framebuffer* getFrameBuffer() noexcept { return m_fbuff; }

protected:

    /**
     * @brief clear the framebuffer
     * 
     * @param fbuff a pointer to the framebuffer to clear
     * @param size the size (as a uvec2 but it's a uint64_t)
     */
    static void software_clear(Software_Framebuffer* fbuff, uint64_t size) noexcept;

    /**
     * @brief change the site of the actual framebuffer object
     * 
     * @param fbuff a pointer to the framebuffer the call came from
     * @param size the size (as a uvec2 but it's a uint64_t)
     */
    static void software_resize(Software_Framebuffer* fbuff, uint64_t size) noexcept;
};

#endif //C++ section

#endif