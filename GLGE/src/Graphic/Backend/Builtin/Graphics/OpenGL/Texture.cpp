/**
 * @file Texture.cpp
 * @author DM8AT
 * @brief implement the texture class for OpenGL
 * @version 0.1
 * @date 2026-03-12
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the OpenGL texture backend
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Texture.h"
//add exceptions
#include "Core/Exception.h"

//add mappings
#include "__Mapper.h"

//add the OpenGL resource set
#include "Graphic/Backend/Builtin/Graphics/OpenGL/ResourceSet.h"

GLGE::Graphic::Backend::Graphic::OpenGL::Texture::Texture(const uvec2& size, u16 mipCount, PixelFormat format) 
 : GLGE::Graphic::Backend::Graphic::Texture(size, mipCount, format)
{
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Texture::Texture");
    //compute the OpenGL format
    GLTextureFormat form = __translate_pixelFormat(format);

    //if the format is invalid, throw
    if (form.internalFormat == GL_NONE)
    {throw GLGE::Exception("Failed to map pixel format to OpenGL format. No closest format found.", "GLGE::Graphic::Backend::Graphic::OpenGL::Texture::Texture");}

    //create the texture
    glCreateTextures(GL_TEXTURE_2D, 1, &m_handle);
    //in debug name the texture
    #if GLGE_DEBUG
    std::string name = std::string("Texture_2D_") + std::to_string(m_name);
    glObjectLabel(GL_TEXTURE, m_handle, name.length(), name.c_str());
    #endif
    //create the storage
    glTextureStorage2D(m_handle, m_mips, form.internalFormat, m_size.x, m_size.y);
}

GLGE::Graphic::Backend::Graphic::OpenGL::Texture::~Texture() {
    //if the OpenGL data exists, clean it
    if (m_handle)
    {glDeleteTextures(1, &m_handle);}
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Texture::update(const TextureCPU& texture) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Texture::update(const TextureCPU& texture)");

    //check if the size, format and mips match
    bool match = (m_size.x == texture[0].getSize().x) && (m_size.y == texture[0].getSize().y) && (m_mips == texture.getMipCount()) && 
                 (m_format.order == texture[0].getFormat().order) && (m_format.type == texture[0].getFormat().type) &&
                 (m_format.r_Bitcount == texture[0].getFormat().r_Bitcount) && (m_format.g_Bitcount == texture[0].getFormat().g_Bitcount) && 
                 (m_format.b_Bitcount == texture[0].getFormat().b_Bitcount) && (m_format.a_Bitcount == texture[0].getFormat().a_Bitcount);
    //translate the format
    GLTextureFormat form = __translate_pixelFormat(m_format);
    //if they don't match the texture must be re-created
    if (!match) {
        //else, the texture must be re-created
        glDeleteTextures(1, &m_handle);
        //store the new data
        m_size = texture[0].getSize();
        m_mips = texture.getMipCount();
        m_format = texture[0].getFormat();

        //translate the format
        GLTextureFormat form = __translate_pixelFormat(m_format);

        //create the texture
        glCreateTextures(GL_TEXTURE_2D, 1, &m_handle);
        //in debug name the texture
        #if GLGE_DEBUG
        std::string name = std::string("Texture_2D_") + std::to_string(m_name);
        glObjectLabel(GL_TEXTURE, m_handle, name.length(), name.c_str());
        #endif
        //create the storage
        glTextureStorage2D(m_handle, m_mips, form.internalFormat, m_size.x, m_size.y);
    }

    //finally upload all images
    for (size_t i = 0; i < m_mips; ++i) 
    {glTextureSubImage2D(m_handle, i, 0, 0, texture[i].getSize().x, texture[i].getSize().y, form.format, form.type, texture[i].getRaw());}
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Texture::update(const ImageCPU& image, u16 mip) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Texture::update(const ImageCPU& image, u16 mip)");

}

void GLGE::Graphic::Backend::Graphic::OpenGL::Texture::onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) {
    //store the binding function
    static void (*func)(u32, void*) = [](u32 unit, void* self) {
        //get the handle
        u32 handle = reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Texture*>(self)->getHandle();
        //bind to an unit
        glBindTextureUnit(unit, handle);
    };
    //register the binding function
    reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::ResourceSet*>(set->getBackend().get())->registerBinder(func, this, unit);
}