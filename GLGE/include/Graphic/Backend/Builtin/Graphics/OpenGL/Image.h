/**
 * @file Image.h
 * @author DM8AT
 * @brief define the overload of the image API for OpenGL
 * @version 0.1
 * @date 2026-03-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_IMAGE_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_IMAGE_

//add the image API
#include "Graphic/Backend/Graphics/Image.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::OpenGL {

    /**
     * @brief the API overload for OpenGL images
     */
    class Image : public GLGE::Graphic::Backend::Graphic::Image {
    public:

        /**
         * @brief Construct a new Image
         * 
         * @param size the size of the image in pixels
         * @param format the format of the image
         * @param samples the amount of samples per pixel
         */
        Image(const uvec2& size, PixelFormat format, u8 samples);

        /**
         * @brief Destroy the Image
         */
        virtual ~Image();

        /**
         * @brief upload data to the image
         * 
         * @param image the image to upload
         */
        virtual void upload(const ImageCPU& image) override;

        /**
         * @brief write an image over a section of this image
         * 
         * @warning if the intended write location is out of bounds the write is clipped
         * 
         * @param image the image to write
         * @param position the position of the bottom left corner of the image
         */
        virtual void write(const ImageCPU& image, const uvec2& position) override;

        /**
         * @brief a function to make the image to a new image with a specific size, but also clears all contents
         * 
         * @param size the requested size for the image
         */
        virtual void resizeAndClear(const uvec2& size) override;

        /**
         * @brief a function to read the image back to the CPU
         * 
         * @warning this will clear the previous contents of `out`
         * 
         * @param out a reference to an image to write to
         */
        virtual void read(ImageCPU& out) const override;

        /**
         * @brief set the image to a stable default clear state
         */
        virtual void clear() override;

        /**
         * @brief a function that is called to build a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to bind to
         * @param unit the unit to bind to
         */
        virtual void onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) override;

        /**
         * @brief Get the handle of the image
         * 
         * @return `u32` the image handle
         */
        inline u32 getHandle() const noexcept
        {return m_handle;}

        /**
         * @brief register a new framebuffer
         * 
         * @param fbuff a pointer to the framebuffer backend
         */
        virtual void registerFramebuffer(GLGE::Graphic::Backend::Graphic::Framebuffer* fbuff) override;

        /**
         * @brief remove a framebuffer
         * 
         * @param fbuff a pointer to the framebuffer to remove
         */
        virtual void removeFramebuffer(GLGE::Graphic::Backend::Graphic::Framebuffer* fbuff) override;

    protected:

        /**
         * @brief store a counter for name generation
         */
        inline static u32 ms_nameCounter = 0;

        /**
         * @brief store the internal format
         */
        u32 m_internalFormat = 0;

        /**
         * @brief store the image handle
         */
        u32 m_handle = 0;
        /**
         * @brief store the name enumerator
         */
        u32 m_name = ++ms_nameCounter;

    };

}

#endif