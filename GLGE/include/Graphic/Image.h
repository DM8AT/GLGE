/**
 * @file Image.h
 * @author DM8AT
 * @brief an image is a single 2D pixel grid
 * @version 0.1
 * @date 2026-03-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_IMAGE_
#define _GLGE_GRAPHIC_IMAGE_

//include common stuff
#include "Core/Common.h"
//add the base class
#include "Core/BaseClass.h"
//add instances
#include "Core/Instance.h"
//add graphic instances
#include "Instance.h"

//add the image backend
#include "Backend/Graphics/Image.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief An image stores a 2D grid of pixels on the GPU
     */
    class Image : public BaseClass, public Resource {
    public:

        /**
         * @brief Construct a new Image
         * 
         * @param size the size of the image in pixels
         * @param format the format of the image
         * @param samples the amount of samples for the image
         */
        Image(const uvec2& size, PixelFormat format, u8 samples = 1)
         : BaseClass(), Resource(), m_image(getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicDescription()->createImage(size, format, samples, getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicBackendInstance().get()))
        {m_image->clear();}

        /**
         * @brief Construct a new Image
         * 
         * @param image the image to load from
         */
        Image(const ImageCPU& image)
         : Image(image.getSize(), image.getFormat())
        {m_image->upload(image);}

        /**
         * @brief upload data to the image
         * 
         * @param image the image to upload
         */
        inline void upload(const ImageCPU& image)
        {m_image->upload(image);}

        /**
         * @brief write an image over a section of this image
         * 
         * @warning if the intended write location is out of bounds the write is clipped
         * 
         * @param image the image to write
         * @param position the position of the bottom left corner of the image
         */
        void write(const ImageCPU& image, const uvec2& position)
        {m_image->write(image, position);}

        /**
         * @brief a function to make the image to a new image with a specific size, but also clears all contents
         * 
         * @param size the requested size for the image
         */
        void resizeAndClear(const uvec2& size)
        {m_image->resizeAndClear(size);}

        /**
         * @brief a function to read the image back to the CPU
         * 
         * @warning this will clear the previous contents of `out`
         * 
         * @param out a reference to an image to write to
         */
        void read(ImageCPU& out) const
        {m_image->read(out);}

        /**
         * @brief Get the pixel format of the image
         * 
         * @return `const PixelFormat&` a constant reference to the pixel format of the image
         */
        inline const PixelFormat& getFormat() const noexcept
        {return m_image->getFormat();}

        /**
         * @brief Get the Size of the image in pixels
         * 
         * @return `const uvec2&` the size of the image in pixels
         */
        inline const uvec2& getSize() const noexcept
        {return m_image->getSize();}

        /**
         * @brief get the backend of the image
         * 
         * @return `Reference<GLGE::Graphic::Backend::Graphic::Image>` a reference to the image backend
         */
        inline Reference<GLGE::Graphic::Backend::Graphic::Image> backend() noexcept
        {return m_image.get();}

        /**
         * @brief a function that is called to build a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to bind to
         * @param unit the unit to bind to
         */
        virtual void onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) override
        {m_image->onBuildBinding(set, unit);}
        
        /**
         * @brief a function that is called when a resource set holding a reference to this resource is destroyed
         * 
         * @param set a pointer to the resource set that was destroyed
         */
        virtual void onRemoveBinding(ResourceSet* set) override
        {m_image->onDropBinding(set);}

        /**
         * @brief Get the type of resource this is
         * 
         * @return `ResourceType` the type to identify the resource
         */
        virtual ResourceType getType() const noexcept override
        {return ResourceType::IMAGE;}

        /**
         * @brief Get the hash of the actual resource type
         * 
         * @return `u64` the hash of the type
         */
        virtual u64 getTypeHash() const noexcept override
        {return getTypeHash64<Image>();}
        
        /**
         * @brief get the backend image of the wrapper
         * 
         * @return `GLGE::Graphic::Backend::Graphic::Image>` a reference to the image backend
         */
        inline Reference<GLGE::Graphic::Backend::Graphic::Image> getBackend() const noexcept
        {return m_image;}

        /**
         * @brief get if this is a valid resource
         * 
         * @return `const char*` `nullptr` if the resource is valid, a human-readable error string otherwise
         */
        virtual const char* onValidation() override {
            //MSAA may not be a resource
            if (m_image->getSamplesPerPixel() > 1) {return "Cannot directly access an multi-sampled image, read-only access is granted using SampledTexture wrappers.";}
            //depth / stencil may not be an image
            if (m_image->getFormat().order == GLGE::Graphic::PixelFormat::Order::DEPTH || m_image->getFormat().order == GLGE::Graphic::PixelFormat::Order::STENCIL)
            {return "Cannot directly access a depth or stencil image. Use a SampledTexture for read-only access.";}
            //others are valid
            return nullptr;
        }

    protected:

        /**
         * @brief store the image
         */
        Reference<GLGE::Graphic::Backend::Graphic::Image> m_image;

    };

}

#endif