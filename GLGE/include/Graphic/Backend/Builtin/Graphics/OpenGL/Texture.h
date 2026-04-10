/**
 * @file Texture.h
 * @author DM8AT
 * @brief define the OpenGL texture backend
 * @version 0.1
 * @date 2026-03-11
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_TEXTURE_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_TEXTURE_

//add the texture API
#include "Graphic/Backend/Graphics/Texture.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::OpenGL {

    /**
     * @brief a class that implements the OpenGL system for a texture
     */
    class Texture : public GLGE::Graphic::Backend::Graphic::Texture {
    public:

        /**
         * @brief Construct a new Texture
         * 
         * @param size the size in pixels of the texture
         * @param mipCount the amount of mip levels the texture has (0 = automatic)
         * @param format the pixel format of the texture
         */
        Texture(const uvec2& size, u16 mipCount, PixelFormat format);

        /**
         * @brief Destroy the Texture
         */
        virtual ~Texture();

        /**
         * @brief change the contents of the texture
         * 
         * @param texture the new texture to load
         */
        virtual void update(const TextureCPU& texture) override;

        /**
         * @brief change the contents of a single specific mip level
         * 
         * @param image the data for the new mip level
         * @param mip the index of the mip level
         */
        virtual void update(const ImageCPU& image, u16 mip) override;

        /**
         * @brief a function that is called to build a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to bind to
         * @param unit the unit to bind to
         */
        virtual void onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) override;

        /**
         * @brief Get the Handle of the OpenGL texture
         * 
         * @return `u32` the handle of the OpenGL texture
         */
        inline u32 getHandle() const noexcept
        {return m_handle;}

    protected:

        /**
         * @brief store a counter for name generation
         */
        inline static u32 ms_nameCounter = 0;

        /**
         * @brief store the OpenGL handle of the texture
         */
        u32 m_handle = 0;
        /**
         * @brief store the name enumerator
         */
        u32 m_name = ++ms_nameCounter;

    };

}

#endif