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
//include command buffers
#include "GLGEGraphicAPI_CommandBuffer.h"

//check if this is C++
#if GLGE_CPP

//say that framebuffers will be defined later
class Framebuffer;

//say that pipelines will exist
class RenderPipeline;

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
     * @param framebuffer a pointer to the framebuffer this graphic framebuffer belongs to
     */
    inline GraphicFramebuffer(Framebuffer* framebuffer, Instance* instance)
     : GraphicInstAttatchable(instance->getGraphicInstance()), m_fbuff(framebuffer) {}

    /**
     * @brief Destroy the Graphic Framebuffer
     */
    virtual ~GraphicFramebuffer() {destroy();}

    /**
     * @brief destroy a framebuffer
     */
    virtual void destroy();

    /**
     * @brief a overloadable hook to create the framebuffer
     */
    virtual void onCreate() {}
    /**
     * @brief a overloadable hook to destroy the framebuffer
     */
    virtual void onDestroy() {}

    /**
     * @brief clear the graphic framebuffer
     * 
     * @param cmdBuff the command buffer to queue the clearing in
     */
    virtual void clear(GraphicCommandBuffer*) {}

    /**
     * @brief get wich command buffers reference this window
     * 
     * @return std::vector<RenderPipeline*> a vector containing pointrs to all command buffers that reference this window
     */
    inline std::vector<RenderPipeline*>& isReferencedBy() noexcept {return m_referencedBy;}

    /**
     * @brief add that the window was referenced by a command buffer
     * 
     * @param cmdBuff a pointer to the command buffer that referenced this window
     */
    void addReference(RenderPipeline* dBuff);

    /**
     * @brief change the size of the framebuffer
     * @warning this will update the size of all the textures the framebuffer owns
     * 
     * @param newSize the new size for the framebuffer
     */
    virtual void resize(const uvec2&) noexcept {}

protected:

    /**
     * @brief store a pointer to the parent framebuffer
     */
    Framebuffer* m_fbuff = 0;
    
    /**
     * @brief store all command buffer that reference this window
     */
    std::vector<RenderPipeline*> m_referencedBy;
};

#endif //C++ section

#endif