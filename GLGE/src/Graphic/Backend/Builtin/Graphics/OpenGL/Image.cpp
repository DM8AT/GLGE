/**
 * @file Image.cpp
 * @author DM8AT
 * @brief implement the OpenGL overload for an image
 * @version 0.1
 * @date 2026-03-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add images
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Image.h"
//add OpenGL resources
#include "Graphic/Backend/Builtin/Graphics/OpenGL/ResourceSet.h"
//add mappings
#include "__Mapper.h"

#include <SDL3/SDL.h>

GLGE::Graphic::Backend::Graphic::OpenGL::Image::Image(const uvec2& size, PixelFormat format, u8 samples, GLGE::Graphic::Backend::Graphic::Instance* instance) 
 : GLGE::Graphic::Backend::Graphic::Image(size, format, samples, instance)
{
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Image::Image");
    //potential sample limit
    i32 maxSamples = 0;
    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
    if (samples > maxSamples) {
        //warning in debug
        #if GLGE_DEBUG
        std::cerr << "[GLGE GRAPHIC] Warning: Tried to create an image with more samples than allowed. Maximum sample count is " << maxSamples << ".\n";
        #endif
        m_samples = maxSamples;
    }
    //create the image
    if (m_samples == 1) 
    {glCreateTextures(GL_TEXTURE_2D, 1, &m_handle);} 
    else 
    {glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_handle);}
    //in debug name the texture
    #if GLGE_DEBUG
    std::string name = std::string("Image_2D_") + std::to_string(m_name);
    glObjectLabel(GL_TEXTURE, m_handle, name.length(), name.c_str());
    #endif
    //translate the format
    GLTextureFormat form = __translate_pixelFormat(format);
    //store the internal format
    m_internalFormat = form.internalFormat;
    //prepare the storage
    if (m_samples == 1)
    {glTextureStorage2D(m_handle, 1, form.internalFormat, m_size.x, m_size.y);}
    else
    {glTextureStorage2DMultisample(m_handle, m_samples, form.internalFormat, m_size.x, m_size.y, GL_TRUE);}
}

GLGE::Graphic::Backend::Graphic::OpenGL::Image::~Image() {
    //if the image exists, delete it
    if (m_handle)
    {glDeleteTextures(1, &m_handle);}
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Image::upload(const ImageCPU& image) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Image::upload");

    //check if the size, format and mips match
    bool match = (m_size.x == image.getSize().x) && (m_size.y == image.getSize().y) && 
                 (m_format.order == image.getFormat().order) && (m_format.type == image.getFormat().type) &&
                 (m_format.r_Bitcount == image.getFormat().r_Bitcount) && (m_format.g_Bitcount == image.getFormat().g_Bitcount) && 
                 (m_format.b_Bitcount == image.getFormat().b_Bitcount) && (m_format.a_Bitcount == image.getFormat().a_Bitcount);
    //translate the format
    GLTextureFormat form = __translate_pixelFormat(m_format);
    //if they don't match the image must be re-created
    if (!match) {
        //else, the image must be re-created
        glDeleteTextures(1, &m_handle);
        //store the new data
        m_size = image.getSize();
        m_format = image.getFormat();

        //translate the format
        GLTextureFormat form = __translate_pixelFormat(m_format);

        //create the texture
        if (m_samples == 1) {
            glCreateTextures(GL_TEXTURE_2D, 1, &m_handle);
        } else {
            glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_handle);
        }
        //in debug name the texture
        #if GLGE_DEBUG
        std::string name = std::string("Image_2D_") + std::to_string(m_name);
        glObjectLabel(GL_TEXTURE, m_handle, name.length(), name.c_str());
        #endif
        //store the internal format
        m_internalFormat = form.internalFormat;
        //create the storage
        if (m_samples == 1)
        {glTextureStorage2D(m_handle, 1, form.internalFormat, m_size.x, m_size.y);}
        else
        {glTextureStorage2DMultisample(m_handle, m_samples, form.internalFormat, m_size.x, m_size.y, GL_TRUE);}
    }

    //finally upload the image
    glTextureSubImage2D(m_handle, 0, 0, 0, m_size.x, m_size.y, form.format, form.type, image.getRaw());
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Image::write(const ImageCPU& image, const uvec2& position) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Image::write");

    //clip subregion
    u32 width  = glm::min(image.getSize().x, m_size.x - position.x);
    u32 height = glm::min(image.getSize().y, m_size.y - position.y);
    //write the region
    GLTextureFormat form = __translate_pixelFormat(image.getFormat());
    glTextureSubImage2D(m_handle, 0, position.x, position.y, width, height, form.format, form.type, image.getRaw());
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Image::resizeAndClear(const uvec2& size) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Image::resizeAndClear");

    //if the size matches allready, early out
    if ((m_size.x == size.x) && (m_size.y == size.y)) {return;}

    //store the new size
    m_size = size;

    //else, re-creation is required
    glDeleteTextures(1, &m_handle);
    m_handle = 0;
    //create the texture
    if (m_samples == 1) {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_handle);
    } else {
        glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_handle);
    }
    //in debug name the texture
    #if GLGE_DEBUG
    std::string name = std::string("Image_2D_") + std::to_string(m_name);
    glObjectLabel(GL_TEXTURE, m_handle, name.length(), name.c_str());
    #endif
    //translate the format
    GLTextureFormat form = __translate_pixelFormat(m_format);
    //create the storage
    if (m_samples == 1)
    {glTextureStorage2D(m_handle, 1, form.internalFormat, m_size.x, m_size.y);}
    else
    {glTextureStorage2DMultisample(m_handle, m_samples, form.internalFormat, m_size.x, m_size.y, GL_TRUE);}

    //default clear value is all zero
    u8 empty[64]{};
    //clear the data
    glClearTexSubImage(m_handle, 0, 0, 0, 0, m_size.x, m_size.y, 1, form.format, form.type, &empty);

    //update all framebuffer references
    for (Framebuffer* fbuff : m_ref_framebuffers)
    {fbuff->onImageUpdated(this);}
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Image::read(ImageCPU& out) const {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Image::read");

    //prepare the output
    out = ImageCPU(nullptr, m_format, m_size);
    //read back the data
    GLTextureFormat form = __translate_pixelFormat(m_format);
    glGetTextureImage(m_handle, 0, form.format, form.type, m_size.x * m_size.y * glm::ceil(m_format.r_Bitcount+m_format.g_Bitcount+m_format.b_Bitcount+m_format.a_Bitcount/8.f), 
                      const_cast<void*>(out.getRaw()));
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Image::clear() {
    //default clear value is all zero
    u8 empty[64]{};
    //clear the data
    GLTextureFormat form = __translate_pixelFormat(m_format);
    glClearTexImage(m_handle, 0, form.format, form.type, &empty);
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Image::onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) {
    //store the binding function
    static void (*func)(u32, void*) = [](u32 unit, void* self) {
        //get the handle
        u32 handle = reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Image*>(self)->getHandle();
        //bind to an image unit
        GLenum format = reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Image*>(self)->m_internalFormat;
        glBindImageTexture(unit, handle, 0, GL_FALSE, 0, GL_READ_WRITE, format);
    };
    //register the binding function
    reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::ResourceSet*>(set->getBackend().get())->registerBinder(func, this, unit);
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Image::onDropBinding(GLGE::Graphic::ResourceSet* set) {
    //just do nothin
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Image::registerFramebuffer(GLGE::Graphic::Backend::Graphic::Framebuffer* fbuff) {
    //add the framebuffer
    m_ref_framebuffers.push_back(fbuff);
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Image::removeFramebuffer(GLGE::Graphic::Backend::Graphic::Framebuffer* fbuff) {
    //remove the pointer
    for (size_t i = 0; i < m_ref_framebuffers.size(); ++i)
    {if (m_ref_framebuffers[i] == fbuff) {m_ref_framebuffers.erase(m_ref_framebuffers.begin() + i); return;}}
}