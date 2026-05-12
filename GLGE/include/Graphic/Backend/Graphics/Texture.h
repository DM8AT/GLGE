/**
 * @file Texture.h
 * @author DM8AT
 * @brief define a backend API for the textures
 * @version 0.1
 * @date 2026-03-11
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHICS_TEXTURE_
#define _GLGE_GRAPHIC_BACKEND_GRAPHICS_TEXTURE_

//add references
#include "Core/Reference.h"
//textures need a CPU texture for data
#include "Graphic/TextureCPU.h"
//add instances
#include "Instance.h"

//add the resource base
#include "Graphic/Resource.h"

//use the backend namespace
namespace GLGE::Graphic::Backend::Graphic {

    /**
     * @brief define a backend API for a GPU texture
     */
    class Texture : public Referable {
    public:

        /**
         * @brief Construct a new Texture
         * 
         * @param size the size in pixels of the texture
         * @param mipCount the amount of mip levels the texture has (0 = automatic)
         * @param format the pixel format of the texture
         * @param instance a pointer to the instance the texture belongs to
         */
        Texture(const uvec2& size, u16 mipCount, PixelFormat format, GLGE::Graphic::Backend::Graphic::Instance* instance)
         : m_instance(instance), m_size(size), m_format(format)
        {
            //store the valid max mip count
            u8 maxMips = glm::floor(glm::min<f32>(glm::log2(f32(size.x)), glm::log2(f32(size.y)))) + 1;
            //store the mip count
            m_mips = (mipCount == 0) ? maxMips : glm::min<u8>(mipCount, maxMips);
        }

        /**
         * @brief Destroy the Texture
         */
        virtual ~Texture() = default;

        /**
         * @brief change the contents of the texture
         * 
         * @param texture the new texture to load
         */
        virtual void update(const TextureCPU& texture) = 0;

        /**
         * @brief change the contents of a single specific mip level
         * 
         * @param image the data for the new mip level
         * @param mip the index of the mip level
         */
        virtual void update(const ImageCPU& image, u16 mip) = 0;

        /**
         * @brief Get the Size in pixels of the texture
         * 
         * @return `const uvec2&` the size in pixels of the texture
         */
        inline const uvec2& getSize() const noexcept
        {return m_size;}

        /**
         * @brief Get the Mip Count
         * 
         * @return `u16` the amount of MIPs
         */
        inline u16 getMipCount() const noexcept
        {return m_mips;}

        /**
         * @brief Get the Format
         * 
         * @return `const PixelFormat&` the format of the pixels
         */
        inline const PixelFormat& getFormat() const noexcept
        {return m_format;}

        /**
         * @brief a function that is called to build a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to bind to
         * @param unit the unit to bind to
         */
        virtual void onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) = 0;

    protected:

        /**
         * @brief store the instance the texture belongs to
         */
        GLGE::Graphic::Backend::Graphic::Instance* m_instance = nullptr;
        /**
         * @brief store the size of the texture
         */
        uvec2 m_size = {0,0};
        /**
         * @brief store the amount of MIPS
         */
        u16 m_mips = 0;
        /**
         * @brief store the current pixel format
         */
        PixelFormat m_format {
            .r_Bitcount = 0,
            .g_Bitcount = 0,
            .b_Bitcount = 0,
            .a_Bitcount = 0,
            .order = PixelFormat::Order::UNDEFINED,
            .type = PixelFormat::Type::UNDEFINED
        };

    };

}

#endif