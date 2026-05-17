/**
 * @file Texture.h
 * @author DM8AT
 * @brief define the vulkan texture backend
 * @version 0.1
 * @date 2026-05-12
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_TEXTURE_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_TEXTURE_

//add the texture API
#include "Graphic/Backend/Graphics/Texture.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::Vulkan {

    /**
     * @brief a class that implements the Vulkan system for a texture
     */
    class Texture : public GLGE::Graphic::Backend::Graphic::Texture {
    public:

        /**
         * @brief Construct a new Texture
         * 
         * @param size the size in pixels of the texture
         * @param mipCount the amount of mip levels the texture has (0 = automatic)
         * @param format the pixel format of the texture
         * @param instance a pointer to the instance the texture belongs to
         */
        Texture(const uvec2& size, u16 mipCount, PixelFormat format, GLGE::Graphic::Backend::Graphic::Instance* instance);

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
         * @brief a function that is called to remove a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to remove from
         */
        virtual void onRemoveBinding(GLGE::Graphic::ResourceSet* set) override;

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
         * This is an array since single mips may have different layouts
         */
        i32 m_layout[16]{};

    };

}

#endif