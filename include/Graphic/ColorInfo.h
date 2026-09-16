/**
 * @file ColorInfo.h
 * @author DM8AT
 * @brief define structures that store some information about colors
 * @version 0.1
 * @date 2025-12-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_COLOR_INFO_
#define _GLGE_GRAPHIC_COLOR_INFO_

//include common stuff
#include "Core/Common.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief store the color space a display is using
     */
    enum class ColorSpace : u8 {
        /**
         * @brief the color space is undefined
         */
        UNDEFINED = 0,
        /**
         * @brief normal SRGB color space
         * 
         * This implies that the values are mapped not linearly
         */
        SRGB,
        /**
         * @brief SRGB, but the values are linearly mapped
         */
        SRGB_LINEAR,
        /**
         * @brief Open HDR default color space
         */
        HDR10,
        /**
         * @brief Color space used by JPEG images (Also known as YcBcR)
         */
        JPEG,
        /**
         * @brief a cut down version of the BT 601 color space
         */
        BT601_LIMITED,
        /**
         * @brief the full version of the BT 601 color space
         */
        BT601_FULL,
        /**
         * @brief a cut down version of the BT 709 color space
         */
        BT709_LIMITED,
        /**
         * @brief the full version of the BT 709 color space
         */
        BT709_FULL,
        /**
         * @brief a limited version of the BT 2020 color space
         */
        BT2020_LIMITED,
        /**
         * @brief the full version of the BT 2020 color space
         */
        BT2020_FULL
    };

    /**
     * @brief store information about the format of the display
     */
    struct PixelFormat {
        /**
         * @brief the amount of bits for the red channel
         * 
         * @warning This value cannot be larger than 64.
         */
        u8 r_Bitcount = 0;
        /**
         * @brief the amount of bits for the green channel
         * 
         * @warning This value may only be 0, 8, 16, 32 or 64. Exceptions exist. 
         */
        u8 g_Bitcount = 0;
        /**
         * @brief the amount of bits for the blue channel
         * 
         * @warning This value may only be 0, 8, 16, 32 or 64. Exceptions exist. 
         */
        u8 b_Bitcount = 0;
        /**
         * @brief the amount of bits for the alpha channel
         * 
         * @warning This value may only be 0, 8, 16, 32 or 64. Exceptions exist. 
         */
        u8 a_Bitcount = 0;

        /**
         * @brief define the order of the channels in the pixel
         */
        enum class Order : u8 {
            /**
             * @brief unknown order
             * 
             * This means that the pixel format is unknown
             */
            UNDEFINED = 0,
            /**
             * @brief the format has 1 channel: only red
             */
            R,
            /**
             * @brief the format has 2 channels: first red, then green. 
             */
            RG,
            /**
             * @brief the format has 3 channels: first red, then green and lastly blue. 
             */
            RGB,
            /**
             * @brief the format has 4 channels: first red, then green, then blue and lastly the alpha channel
             */
            RGBA,
            /**
             * @brief the format has 4 channels: first the alpha channel, then red, then green and lastly blue
             */
            ARGB,
            /**
             * @brief the format has 3 channels: first blue, then green and lastly red
             */
            BGR,
            /**
             * @brief the format has 4 channels: first blue, then green, then red and lastly the alpha channel
             */
            BGRA,
            /**
             * @brief the format has 4 channels: first the alpha channel, then blue, then green and lastly red
             */
            ABGR,
            /**
             * @brief the pixel uses an indexed pallet
             * 
             * This means that only the red channel has a bit count. The bit count of the red channel gives the amount of bits used by the index. 
             */
            INDEXED,

            /**
             * @brief the pixel is used as a depth data (meaning 1 component)
             */
            DEPTH,
            /**
             * @brief the pixel is used as stencil data (meaning 1 component)
             */
            STENCIL,
            /**
             * @brief the pixel stores depth data (first component) and stencil data (second component)
             */
            DEPTH_STENCIL
        };

        /**
         * @brief store the type used by the pixel format
         */
        enum class Type : u8 {
            /**
             * @brief the type is undefined
             * 
             * This is the set type if the pixel format is undefined
             */
            UNDEFINED = 0,
            /**
             * @brief a normalized unsigned integer
             * 
             * The values range from 0 to 1 where 0 is encoded as 0 and 1 as the maximum possible value (2^bitCount - 1)
             */
            UNORM,
            /**
             * @brief a simple unsigned integer
             */
            UINT,
            /**
             * @brief A normal IEEE float
             */
            FLOAT,
        };

        /**
         * @brief store the pixel order used by the pixel format
         */
        Order order = Order::UNDEFINED;
        /**
         * @brief store the type of the channels
         */
        Type type = Type::UNDEFINED;

    };

    /**
     * @brief A pixel format with 1 normalized unsigned integer component with 8 bits
     * 
     * R : 8 bits  
     * G : 0 bits  
     * B : 0 bits  
     * A : 0 bits  
     * 
     * Order : R  
     * Type  : UNORM
     */
    const constexpr PixelFormat PIXEL_FORMAT_R_8_UNORM {
        .r_Bitcount = 8,
        .g_Bitcount = 0,
        .b_Bitcount = 0,
        .a_Bitcount = 0,
        .order = PixelFormat::Order::R,
        .type = PixelFormat::Type::UNORM
    };

    /**
     * @brief A pixel format with 2 normalized unsigned integer components with 8 bits per channel
     * 
     * R : 8 bits  
     * G : 8 bits  
     * B : 0 bits  
     * A : 0 bits  
     * 
     * Order : RG  
     * Type  : UNORM
     */
    const constexpr PixelFormat PIXEL_FORMAT_RG_8_UNORM {
        .r_Bitcount = 8,
        .g_Bitcount = 8,
        .b_Bitcount = 0,
        .a_Bitcount = 0,
        .order = PixelFormat::Order::RG,
        .type = PixelFormat::Type::UNORM
    };

    /**
     * @brief A pixel format with 4 normalized unsigned integer components with 8 bits per channel
     * 
     * R : 8 bits  
     * G : 8 bits  
     * B : 8 bits  
     * A : 8 bits  
     * 
     * Order : RGBA  
     * Type  : UNORM
     */
    const constexpr PixelFormat PIXEL_FORMAT_RGBA_8_UNORM {
        .r_Bitcount = 8,
        .g_Bitcount = 8,
        .b_Bitcount = 8,
        .a_Bitcount = 8,
        .order = PixelFormat::Order::RGBA,
        .type = PixelFormat::Type::UNORM
    };

    /**
     * @brief A pixel format with 4 normalized unsigned integer components with 16 bits per channel
     * 
     * R : 16 bits  
     * G : 16 bits  
     * B : 16 bits  
     * A : 16 bits  
     * 
     * Order : RGBA  
     * Type  : UNORM
     */
    const constexpr PixelFormat PIXEL_FORMAT_RGBA_16_UNORM {
        .r_Bitcount = 16,
        .g_Bitcount = 16,
        .b_Bitcount = 16,
        .a_Bitcount = 16,
        .order = PixelFormat::Order::RGBA,
        .type = PixelFormat::Type::UNORM
    };

    /**
     * @brief A pixel format with 4 normalized unsigned integer components with 32 bits per channel
     * 
     * R : 32 bits  
     * G : 32 bits  
     * B : 32 bits  
     * A : 32 bits  
     * 
     * Order : RGBA  
     * Type  : UNORM
     */
    const constexpr PixelFormat PIXEL_FORMAT_RGBA_32_UNORM {
        .r_Bitcount = 32,
        .g_Bitcount = 32,
        .b_Bitcount = 32,
        .a_Bitcount = 32,
        .order = PixelFormat::Order::RGBA,
        .type = PixelFormat::Type::UNORM
    };

    /**
     * @brief A pixel format with 1 floating-point component with 16 bits
     * 
     * R : 16 bits  
     * G : 0 bits  
     * B : 0 bits  
     * A : 0 bits  
     * 
     * Order : R  
     * Type  : FLOAT
     */
    const constexpr PixelFormat PIXEL_FORMAT_R_16_FLOAT {
        .r_Bitcount = 16,
        .g_Bitcount = 0,
        .b_Bitcount = 0,
        .a_Bitcount = 0,
        .order = PixelFormat::Order::R,
        .type = PixelFormat::Type::FLOAT
    };

    /**
     * @brief A pixel format with 2 floating-point components with 16 bits per channel
     * 
     * R : 16 bits  
     * G : 16 bits  
     * B : 0 bits  
     * A : 0 bits  
     * 
     * Order : RG  
     * Type  : FLOAT
     */
    const constexpr PixelFormat PIXEL_FORMAT_RG_16_FLOAT {
        .r_Bitcount = 16,
        .g_Bitcount = 16,
        .b_Bitcount = 0,
        .a_Bitcount = 0,
        .order = PixelFormat::Order::RG,
        .type = PixelFormat::Type::FLOAT
    };

    /**
     * @brief A pixel format with 4 floating-point components with 16 bits per channel
     * 
     * R : 16 bits  
     * G : 16 bits  
     * B : 16 bits  
     * A : 16 bits  
     * 
     * Order : RGBA  
     * Type  : FLOAT
     */
    const constexpr PixelFormat PIXEL_FORMAT_RGBA_16_FLOAT {
        .r_Bitcount = 16,
        .g_Bitcount = 16,
        .b_Bitcount = 16,
        .a_Bitcount = 16,
        .order = PixelFormat::Order::RGBA,
        .type = PixelFormat::Type::FLOAT
    };

    /**
     * @brief A pixel format with 1 floating-point component with 32 bits
     * 
     * R : 32 bits  
     * G : 0 bits  
     * B : 0 bits  
     * A : 0 bits  
     * 
     * Order : R  
     * Type  : FLOAT
     */
    const constexpr PixelFormat PIXEL_FORMAT_R_32_FLOAT {
        .r_Bitcount = 32,
        .g_Bitcount = 0,
        .b_Bitcount = 0,
        .a_Bitcount = 0,
        .order = PixelFormat::Order::R,
        .type = PixelFormat::Type::FLOAT
    };

    /**
     * @brief A pixel format with 2 floating-point components with 32 bits per channel
     * 
     * R : 32 bits  
     * G : 32 bits  
     * B : 0 bits  
     * A : 0 bits  
     * 
     * Order : RG  
     * Type  : FLOAT
     */
    const constexpr PixelFormat PIXEL_FORMAT_RG_32_FLOAT {
        .r_Bitcount = 32,
        .g_Bitcount = 32,
        .b_Bitcount = 0,
        .a_Bitcount = 0,
        .order = PixelFormat::Order::RG,
        .type = PixelFormat::Type::FLOAT
    };

    /**
     * @brief A pixel format with 3 floating-point components with 32 bits per channel
     * 
     * R : 32 bits  
     * G : 32 bits  
     * B : 32 bits  
     * A : 0 bits  
     * 
     * Order : RGB  
     * Type  : FLOAT
     */
    const constexpr PixelFormat PIXEL_FORMAT_RGB_32_FLOAT {
        .r_Bitcount = 32,
        .g_Bitcount = 32,
        .b_Bitcount = 32,
        .a_Bitcount = 0,
        .order = PixelFormat::Order::RGB,
        .type = PixelFormat::Type::FLOAT
    };

    /**
     * @brief A pixel format with 4 floating-point components with 32 bits per channel
     * 
     * R : 32 bits  
     * G : 32 bits  
     * B : 32 bits  
     * A : 32 bits  
     * 
     * Order : RGBA  
     * Type  : FLOAT
     */
    const constexpr PixelFormat PIXEL_FORMAT_RGBA_32_FLOAT {
        .r_Bitcount = 32,
        .g_Bitcount = 32,
        .b_Bitcount = 32,
        .a_Bitcount = 32,
        .order = PixelFormat::Order::RGBA,
        .type = PixelFormat::Type::FLOAT
    };

    /**
     * @brief A pixel format for a 16 bit depth component
     * 
     * R : 16 bits
     * G : 0 bits
     * B : 0 bits
     * A : 0 bits
     * 
     * Order : DEPTH
     * Type  : UNORM
     */
    const constexpr PixelFormat PIXEL_FORMAT_DEPTH_16_UNORM {
        .r_Bitcount = 16,
        .g_Bitcount = 0,
        .b_Bitcount = 0,
        .a_Bitcount = 0,
        .order = PixelFormat::Order::DEPTH,
        .type = PixelFormat::Type::UNORM
    };

    /**
     * @brief A pixel format for a 24 bit depth component
     * 
     * R : 24 bits
     * G : 0 bits
     * B : 0 bits
     * A : 0 bits
     * 
     * Order : DEPTH
     * Type  : UNORM
     */
    const constexpr PixelFormat PIXEL_FORMAT_DEPTH_24_UNORM {
        .r_Bitcount = 24,
        .g_Bitcount = 0,
        .b_Bitcount = 0,
        .a_Bitcount = 0,
        .order = PixelFormat::Order::DEPTH,
        .type = PixelFormat::Type::UNORM
    };

    /**
     * @brief A pixel format for a 32 bit depth component
     * 
     * R : 32 bits
     * G : 0 bits
     * B : 0 bits
     * A : 0 bits
     * 
     * Order : DEPTH
     * Type  : UNORM
     */
    const constexpr PixelFormat PIXEL_FORMAT_DEPTH_32_UNORM {
        .r_Bitcount = 32,
        .g_Bitcount = 0,
        .b_Bitcount = 0,
        .a_Bitcount = 0,
        .order = PixelFormat::Order::DEPTH,
        .type = PixelFormat::Type::UNORM
    };

    /**
     * @brief A pixel format for a 32 float bit depth component
     * 
     * R : 32 bits
     * G : 0 bits
     * B : 0 bits
     * A : 0 bits
     * 
     * Order : DEPTH
     * Type  : FLOAT
     */
    const constexpr PixelFormat PIXEL_FORMAT_DEPTH_32_FLOAT {
        .r_Bitcount = 32,
        .g_Bitcount = 0,
        .b_Bitcount = 0,
        .a_Bitcount = 0,
        .order = PixelFormat::Order::DEPTH,
        .type = PixelFormat::Type::FLOAT
    };

    /**
     * @brief A pixel format for a 1 bit stencil component
     * 
     * R : 1 bit
     * G : 0 bits
     * B : 0 bits
     * A : 0 bits
     * 
     * Order : STENCIL
     * Type  : UINT
     */
    const constexpr PixelFormat PIXEL_FORMAT_STENCIL_1_UINT {
        .r_Bitcount = 1,
        .g_Bitcount = 0,
        .b_Bitcount = 0,
        .a_Bitcount = 0,
        .order = PixelFormat::Order::STENCIL,
        .type = PixelFormat::Type::UINT
    };

    /**
     * @brief A pixel format for a 4 bit stencil component
     * 
     * R : 4 bits
     * G : 0 bits
     * B : 0 bits
     * A : 0 bits
     * 
     * Order : STENCIL
     * Type  : UINT
     */
    const constexpr PixelFormat PIXEL_FORMAT_STENCIL_4_UINT {
        .r_Bitcount = 4,
        .g_Bitcount = 0,
        .b_Bitcount = 0,
        .a_Bitcount = 0,
        .order = PixelFormat::Order::STENCIL,
        .type = PixelFormat::Type::UINT
    };

    /**
     * @brief A pixel format for a 8 bit stencil component
     * 
     * R : 8 bits
     * G : 0 bits
     * B : 0 bits
     * A : 0 bits
     * 
     * Order : STENCIL
     * Type  : UINT
     */
    const constexpr PixelFormat PIXEL_FORMAT_STENCIL_8_UINT {
        .r_Bitcount = 8,
        .g_Bitcount = 0,
        .b_Bitcount = 0,
        .a_Bitcount = 0,
        .order = PixelFormat::Order::STENCIL,
        .type = PixelFormat::Type::UINT
    };

    /**
     * @brief A pixel format for a 16 bit stencil component
     * 
     * R : 16 bits
     * G : 0 bits
     * B : 0 bits
     * A : 0 bits
     * 
     * Order : STENCIL
     * Type  : UINT
     */
    const constexpr PixelFormat PIXEL_FORMAT_STENCIL_16_UINT {
        .r_Bitcount = 16,
        .g_Bitcount = 0,
        .b_Bitcount = 0,
        .a_Bitcount = 0,
        .order = PixelFormat::Order::STENCIL,
        .type = PixelFormat::Type::UINT
    };

    /**
     * @brief A pixel format for a 24 bit depth and a 8 bit stencil component
     * 
     * R : 24 bits
     * G : 8 bits
     * B : 0 bits
     * A : 0 bits
     * 
     * Order : DEPTH_STENCIL
     * Type  : UNORM
     */
    const constexpr PixelFormat PIXEL_FORMAT_DEPTH24_STENCIL8 {
        .r_Bitcount = 24,
        .g_Bitcount = 8,
        .b_Bitcount = 0,
        .a_Bitcount = 0,
        .order = PixelFormat::Order::DEPTH_STENCIL,
        .type = PixelFormat::Type::UNORM
    };

    /**
     * @brief A pixel format for a 32 bit depth and a 8 bit stencil component
     * 
     * R : 32 bits
     * G : 8 bits
     * B : 0 bits
     * A : 0 bits
     * 
     * Order : DEPTH_STENCIL
     * Type  : FLOAT
     */
    const constexpr PixelFormat PIXEL_FORMAT_DEPTH32F_STENCIL8 {
        .r_Bitcount = 32,
        .g_Bitcount = 8,
        .b_Bitcount = 0,
        .a_Bitcount = 0,
        .order = PixelFormat::Order::DEPTH_STENCIL,
        .type = PixelFormat::Type::FLOAT
    };

}

#endif