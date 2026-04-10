/**
 * @file TextureCPU.h
 * @author DM8AT
 * @brief define a texture that only lives on the CPU
 * @version 0.1
 * @date 2026-01-03
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_TEXTURE_CPU_
#define _GLGE_GRAPHIC_TEXTURE_CPU_

//add common stuff
#include "Core/Common.h"
//add color information
#include "ColorInfo.h"

//add images
#include "ImageCPU.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief define a class for a texture that only lives on the CPU
     */
    class TextureCPU {
    public:

        /**
         * @brief store the maximum amount of mips allowed
         */
        inline static constexpr u16 MAX_MIPS = 16;

        /**
         * @brief store the structure of an iterator
         */
        template <typename T>
        class __iterator_base {
        public:
            /**
             * @brief store the layer the iterator is referencing
             */
            GLGE::u8 layer = 0;
            /**
             * @brief store the image the iterator is pointing to
             */
            T* img = nullptr;

        public:

            /**
             * @brief Construct a new iterator
             * 
             * @param _layer the level the iterator is on
             * @param _img a pointer to the image data of the layer
             */
            __iterator_base(GLGE::u8 _layer, T* _img)
             : layer(_layer), img(_img), m_content{_layer, _img}
            {}

            /**
             * @brief a helper structure to access the iterator's data
             */
            struct proxy {
                /**
                 * @brief store the layer the iterator is referencing
                 */
                u8 layer = 0;
                /**
                 * @brief a reference to the image the proxy refers to
                 */
                T* image = nullptr;

                /**
                 * @brief allow structure binding, but disable tuple-like access
                 */
                auto& operator[](GLGE::u8 i) = delete;
            };

            /**
             * @brief create a proxy from the iterator
             * 
             * @return `proxy` the proxy structure that corresponds to the iterator
             */
            inline proxy& operator*() noexcept 
            {return m_content;}

            /**
             * @brief get a constant proxy reference
             * 
             * @return `const proxy&` a constant reference to the contents
             */
            inline const proxy& operator*() const noexcept 
            requires std::is_const_v<T>
            {return m_content;}

            /**
             * @brief step the iterator forward by some amount
             * 
             * @param count the amount to step the iterator forward
             * @return `__iterator_base&` a reference to the iterator
             */
            inline __iterator_base& operator+=(GLGE::u8 count) noexcept
            {layer += count; img += count; m_content.layer += count; m_content.image += count; return *this;}

            /**
             * @brief step the iterator forward by one
             * 
             * @return `__iterator_base&` the stepped forward iterator
             */
            inline __iterator_base& operator++() noexcept
            {++layer; ++img; ++m_content.layer; ++m_content.image; return *this;}

            /**
             * @brief step the iterator backward by some amount
             * 
             * @param count the amount to step the iterator backward
             * @return `__iterator_base&` a reference to the iterator
             */
            inline __iterator_base& operator-=(GLGE::u8 count) noexcept
            {layer -= count; img -= count; m_content.layer -= count; m_content.image -= count; return *this;}

            /**
             * @brief step the iterator backward by one
             * 
             * @return `__iterator_base&` the stepped back iterator
             */
            inline __iterator_base& operator--() noexcept
            {--layer; --img; --m_content.layer; --m_content.image; return *this;}

            /**
             * @brief compare two iterators to check if they are the same
             * 
             * @param other the iterator to compare with
             * 
             * @return `true` if the iterators are the same, `false` if they are not the same
             */
            bool operator==(const __iterator_base& other) const
            {return (layer == other.layer) && (img == other.img);}

        private:

            /**
             * @brief store the proxy value to the current element
             */
            proxy m_content;
        };

        /**
         * @brief define what an iterator for a texture is
         */
        using iterator = __iterator_base<ImageCPU>;

        /**
         * @brief define what a constant iterator for a texture is
         */
        using const_iterator = __iterator_base<const ImageCPU>;

        /**
         * @brief Construct a new CPU texture
         * 
         * @param image the image to create the texture from
         * @param mipCount the amount of mips to create (`mipCount` = 0 is auto)
         */
        TextureCPU(const ImageCPU& image, u8 mipCount);

        /**
         * @brief Construct a new CPU Texture
         * 
         * @param mips an array of images that contain the MIP levels
         * @param mipCount the amount of mips in the array (`mipCount` = 0 is auto)
         */
        TextureCPU(const ImageCPU* mips, u8 mipCount);

        /**
         * @brief Get a specific mip level
         * 
         * @param mip the mip level to quarry
         * @return `const ImageCPU&` the image that represents that mip level
         */
        inline const ImageCPU& getMip(u8 mip) const noexcept
        {return m_imgs[mip];}

        /**
         * @brief Get the amount of mip maps
         * 
         * @return `u8` the amount of mip levels present
         */
        inline u8 getMipCount() const noexcept
        {return m_mipCount;}

        /**
         * @brief read a pixel from a specific mip level
         * 
         * @param pixel the pixel position in absolute pixels
         * @param mip the mip level to read from
         * @return `vec4` the color value in RGBA format
         */
        inline vec4 fetchTexel(const uvec2& pixel, u8 mip) const noexcept
        {return m_imgs[mip].readTexelRGBA(pixel);}

        /**
         * @brief access a specific mip layer of the texture
         * 
         * @param mip the mip level to read
         * @return `ImageCPU&` a reference to an CPU image containing the specific mip level
         */
        inline ImageCPU& image(u8 mip) noexcept
        {return m_imgs[mip];}

        /**
         * @brief access a specific mip layer of the texture
         * 
         * @param mip the mip level to read
         * @return `const ImageCPU&` a constant reference to an CPU image containing the specific mip level
         */
        inline const ImageCPU& image(u8 mip) const noexcept
        {return m_imgs[mip];}

        /**
         * @brief access a specific mip layer of the texture
         * 
         * @param mip the mip level to read
         * @return `ImageCPU&` a reference to an CPU image containing the specific mip level
         */
        inline ImageCPU& operator[](u8 mip) noexcept 
        {return image(mip);}

        /**
         * @brief access a specific mip layer of the texture
         * 
         * @param mip the mip level to read
         * @return `const ImageCPU&` a constant reference to an CPU image containing the specific mip level
         */
        inline const ImageCPU& operator[](u8 mip) const noexcept
        {return image(mip);}

        /**
         * @brief get an iterator to the beginning of the texture
         * 
         * @return `iterator` an iterator to the top mip layer
         */
        inline iterator begin() noexcept
        {return iterator{0, m_imgs};}

        /**
         * @brief get an iterator to the below-bottom mip layer
         * 
         * @return `iterator` an iterator to the below-bottom mip layer
         */
        inline iterator end() noexcept
        {return iterator{m_mipCount, m_imgs+m_mipCount};}

        /**
         * @brief get a constant iterator to the top mip layer
         * 
         * @return `const_iterator` the constant iterator pointing to the top layer
         */
        inline const_iterator begin() const noexcept
        {return const_iterator{0, m_imgs};}

        /**
         * @brief get a constant iterator to the below-bottom mip layer
         * 
         * @return `const_iterator` the constant iterator pointing to the below-bottom layer
         */
        inline const_iterator end() const noexcept
        {return const_iterator{m_mipCount, m_imgs+m_mipCount};}

    protected:

        /**
         * @brief store the amount of mip levels that the texture has
         */
        u8 m_mipCount = 0;
        /**
         * @brief store all the images that make up the texture
         */
        ImageCPU m_imgs[MAX_MIPS];

    };

}

#endif