/**
 * @file Image.h
 * @author DM8AT
 * @brief define a backend API for images
 * @version 0.1
 * @date 2026-03-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHICS_IMAGE_
#define _GLGE_GRAPHIC_BACKEND_GRAPHICS_IMAGE_

//add references
#include "Core/Reference.h"
//add CPU images
#include "Graphic/ImageCPU.h"

//add the resource base
#include "Graphic/Resource.h"

//use the backend namespace
namespace GLGE::Graphic::Backend::Graphic {

    //framebuffers are defined somewhere else
    class Framebuffer;

    /**
     * @brief define an API for all image classes
     */
    class Image : public Referable {
    public:

        /**
         * @brief Construct a new Image
         * 
         * @param size the size of the image
         * @param format the format of the image
         * @param samples the amount of samples per pixels
         */
        Image(const uvec2& size, PixelFormat format, u8 samples)
         : Referable(), m_size(size), m_format(format), m_samples(samples)
        {}

        /**
         * @brief Destroy the Image
         */
        virtual ~Image() = default;

        /**
         * @brief upload data to the image
         * 
         * @param image the image to upload
         */
        virtual void upload(const ImageCPU& image) = 0;

        /**
         * @brief write an image over a section of this image
         * 
         * @warning if the intended write location is out of bounds the write is clipped
         * 
         * @param image the image to write
         * @param position the position of the bottom left corner of the image
         */
        virtual void write(const ImageCPU& image, const uvec2& position) = 0;

        /**
         * @brief a function to make the image to a new image with a specific size, but also clears all contents
         * 
         * @param size the requested size for the image
         */
        virtual void resizeAndClear(const uvec2& size) = 0;

        /**
         * @brief a function to read the image back to the CPU
         * 
         * @warning this will clear the previous contents of `out`
         * 
         * @param out a reference to an image to write to
         */
        virtual void read(ImageCPU& out) const = 0;

        /**
         * @brief set the image to a stable default clear state
         */
        virtual void clear() = 0;

        /**
         * @brief a function that is called to build a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to bind to
         * @param unit the unit to bind to
         */
        virtual void onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) = 0;

        /**
         * @brief Get the pixel format of the image
         * 
         * @return `const PixelFormat&` a constant reference to the pixel format of the image
         */
        inline const PixelFormat& getFormat() const noexcept
        {return m_format;}

        /**
         * @brief Get the Size of the image in pixels
         * 
         * @return `const uvec2&` the size of the image in pixels
         */
        inline const uvec2& getSize() const noexcept
        {return m_size;}

        /**
         * @brief register a new framebuffer
         * 
         * @param fbuff a pointer to the framebuffer backend
         */
        virtual void registerFramebuffer(GLGE::Graphic::Backend::Graphic::Framebuffer* fbuff) = 0;

        /**
         * @brief remove a framebuffer
         * 
         * @param fbuff a pointer to the framebuffer to remove
         */
        virtual void removeFramebuffer(GLGE::Graphic::Backend::Graphic::Framebuffer* fbuff) = 0;

        /**
         * @brief Get the Samples Per Pixel
         * 
         * @return `u8` the amount of samples per pixel
         */
        inline u8 getSamplesPerPixel() const noexcept
        {return m_samples;}

    protected:

        /**
         * @brief store all framebuffers that use the image
         */
        std::vector<Framebuffer*> m_ref_framebuffers;

        /**
         * @brief store the pixel format of the image
         */
        PixelFormat m_format;
        /**
         * @brief store the amount of samples per pixel
         */
        u8 m_samples = 1;
        /**
         * @brief store the size in pixels of the image
         */
        uvec2 m_size;

    };

}

#endif