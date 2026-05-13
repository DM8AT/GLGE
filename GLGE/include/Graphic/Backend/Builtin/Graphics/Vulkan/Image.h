/**
 * @file Image.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-05-10
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
namespace GLGE::Graphic::Backend::Graphic::Vulkan {

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
         * @param instance a pointer to the graphic instance the image belongs to
         */
        Image(const uvec2& size, PixelFormat format, u8 samples, GLGE::Graphic::Backend::Graphic::Instance* instance);

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
         * @brief a function that is called to remove the binding between an image and a resource set
         * 
         * @param set a pointer to the set to drop the binding to
         */
        virtual void onDropBinding(GLGE::Graphic::ResourceSet* set) override;

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

        /**
         * @brief Get the actual image
         * 
         * @return `void*` the image vulkan handle
         */
        inline void* getImage() const noexcept
        {return m_image;}

        /**
         * @brief Get the image view
         * 
         * @return `void*` the vulkan image view
         */
        inline void* getView() const noexcept
        {return m_view;}

        /**
         * @brief Set the Layout
         * 
         * @param layout the new layout of the image
         */
        inline void setLayout(i32 layout) noexcept
        {m_layout = layout;}

        /**
         * @brief Get the current layout
         * 
         * @return `i32` the current layout
         */
        inline i32 getLayout() const noexcept
        {return m_layout;}

        /**
         * @brief Get the Aspect Flags
         * 
         * @return `i32` the image aspect flags
         */
        inline i32 getAspectFlags() const noexcept
        {return m_aspectFlags;}

        /**
         * @brief Get the Vulkan Format
         * 
         * @return `i32` the vulkan format
         */
        inline i32 getVkFormat() const noexcept
        {return m_vkFormat;}

    protected:

        /**
         * @brief store the vulkan image
         */
        void* m_image = nullptr;

        /**
         * @brief store the VMA allocation
         */
        void* m_allocation = nullptr;

        /**
         * @brief store a view into the image
         */
        void* m_view = nullptr;

        /**
         * @brief store the vulkan format
         */
        i32 m_vkFormat = 0;
        /**
         * @brief store the image aspect flags
         */
        i32 m_aspectFlags = 0;

        /**
         * @brief store the current image layout
         * 
         * This is UNDEFINED if the image was never used and GENERAL if the image was used
         */
        i32 m_layout = 0;

        /**
         * @brief store a list of all set unit pairs that reference this image
         */
        std::vector<std::pair<GLGE::Graphic::ResourceSet*, u32>> m_references;

    };

}

#endif