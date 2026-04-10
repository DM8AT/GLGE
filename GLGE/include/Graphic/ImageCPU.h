/**
 * @file ImageCPU.h
 * @author DM8AT
 * @brief define what a single image is
 * @version 0.1
 * @date 2026-01-03
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_IMAGE_CPU_
#define _GLGE_GRAPHIC_IMAGE_CPU_

//add common stuff
#include "Core/Common.h"
//add color information
#include "ColorInfo.h"

//add CPU samplers
#include "SamplerCPU.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief a two dimensional array where each entry contains color data
     * 
     * This data is only stored on the CPU side
     */
    class ImageCPU {
    public:

        /**
         * @brief Construct a new CPU Image
         * 
         * An empty image that contains no data
         */
        ImageCPU() = default;

        /**
         * @brief Construct a new CPU Image
         */
        ImageCPU(void* data, const PixelFormat& format, const uvec2& size);

        /**
         * @brief Construct a new CPU Image
         * 
         * @param other the image to copy
         */
        ImageCPU(const ImageCPU& other);

        /**
         * @brief Construct a new CPU Image
         * 
         * @param other the image to move from
         */
        ImageCPU(ImageCPU&& other) noexcept;

        /**
         * @brief Destroy the CPU Image
         */
        ~ImageCPU();

        /**
         * @brief Copy assignment operator
         * 
         * @param other the image to copy from
         * @return `ImageCPU&` a reference to this image after construction
         */
        ImageCPU& operator=(const ImageCPU& other);

        /**
         * @brief Move assignment operator
         * 
         * @param other the image to move from
         * @return `ImageCPU&` a reference to this image after moving
         */
        ImageCPU& operator=(ImageCPU&& other);

        /**
         * @brief Create an image that contains the same data as this one but with a different format
         * 
         * @param format the format of the new image
         * @return `ImageCPU` the reformatted image
         */
        ImageCPU toFormat(const PixelFormat& format);

        /**
         * @brief read a texel from the image
         * 
         * @param texel the texel coordinates to read
         * @return `const void*` a pointer to the correct place in memory where the texel data is stored in the internal format
         */
        const void* readTexel(const uvec2& texel) const;

        /**
         * @brief read a texel from the image and convert it to RGBA_F32
         * 
         * @param texel the texel coordinates of the texel to read
         * @return `vec4` a 4D float vector containing the data in RGBA order
         */
        vec4 readTexelRGBA(const uvec2& texel) const;

        /**
         * @brief set the value of a single texel of the image
         * 
         * @param texel the texel coordinates to update
         * @param value the value to set the texel to in the correct encoding
         */
        void writeTexel(const uvec2& texel, void* value);

        /**
         * @brief set the value of a single texel of the image
         * 
         * @param texel the texel coordinates to update
         * @param value the value for the texel in RGBA 32 float form
         */
        void writeTexelRGBA(const uvec2& texel, const vec4& value);

        /**
         * @brief take a sample of a specific location on the image
         * 
         * @param uv the uv coordinate to sample at
         * @param sampler the sampler used to determine how to sample
         * @return `vec4` the color in 32 bit float RGBA format of the color at that position
         */
        vec4 sample(vec2 uv, const SamplerCPU& sampler) const;

        /**
         * @brief take a sample of a specific location on the image while taking a perspective into acount
         * 
         * @param uv the uv coordinate to sample at
         * @param derivatives the derivatives of the uv coordinates over the x and y axis (x : u/x, y : u/y, z : v/x, w : v/y)
         * @param sampler the sampler used to determine how to sample
         * @return `vec4` the color in 32 bit float RGBA format of the color at that position
         */
        vec4 samplePerspective(vec2 uv, vec4 derivatives, const SamplerCPU& sampler) const;

        /**
         * @brief Get the raw data of the image
         * 
         * @return `const void*` the raw image data
         */
        inline const void* getRaw() const noexcept
        {return m_data;}

        /**
         * @brief Get the size of the the image
         * 
         * @return `uvec2` the size of the image in pixels
         */
        inline uvec2 getSize() const noexcept
        {return m_size;}

        /**
         * @brief Get the pixel format of the image
         * 
         * @return `const PixelFormat&` the pixel format of the internal data
         */
        inline const PixelFormat& getFormat() const noexcept
        {return m_format;}

        /**
         * @brief check if the image holds valid data
         * 
         * @return `true` if the image holds valid data, `false` otherwise
         */
        inline bool isValid() const noexcept
        {return !((m_size.x == 0) || 
                  (m_size.y) || 
                  (m_format.r_Bitcount == 0) || 
                  (m_format.order == PixelFormat::Order::UNDEFINED) || 
                  (m_format.type == PixelFormat::Type::UNDEFINED) || 
                  (m_data == nullptr) || 
                  (m_bytesPerPixel == 0));}

        /**
         * @brief check if the image holds valid data
         * 
         * @return `true` if the image holds valid data, `false` otherwise
         */
        inline operator bool() const noexcept
        {return isValid();}

    protected:

        /**
         * @brief store the size of the image
         */
        uvec2 m_size {0,0};
        /**
         * @brief store the format of the image data
         */
        PixelFormat m_format;
        /**
         * @brief store the amount of bytes each texel takes up
         */
        u64 m_bytesPerPixel = 0;
        /**
         * @brief store the actual image data
         */
        u8* m_data = nullptr;

    };

}

#endif