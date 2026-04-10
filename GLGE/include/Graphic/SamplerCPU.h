/**
 * @file SamplerCPU.h
 * @author DM8AT
 * @brief define a class that is used to define how textures and images are sampled
 * @version 0.1
 * @date 2026-01-03
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_SAMPLER_CPU_
#define _GLGE_GRAPHIC_SAMPLER_CPU_

//add common stuff
#include "Core/Common.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief define a class that defines how images and textures are sampled
     */
    class SamplerCPU {
    public:

        /**
         * @brief store an enum to name some anisotropy levels
         * 
         * This are NOT all anisotropy levels supported by the engine. It supports all integers from 0 to 255 as anisotropy levels. 
         * But the underlying backend determines the actual maximum supported anisotropy. 
         * 
         * All anisotropy levels parsed through this enum are actually one lower than the corresponding name as 0 is not a valid anisotropy level
         * and so one more level can be encoded. 
         */
        enum Anisotropic : u8 {
            /**
             * @brief a setting to disable anisotropic sampling
             * 
             * This is the same as only taking a single anisotropic sample
             */
            ANISOTROPY_DISABLED = 0,
            /**
             * @brief a setting to only take a single texture sample
             * 
             * This is the same as disabling anisotropic sampling
             */
            ANISOTROPY_X1  = 0x0,
            /**
             * @brief a setting to take two anisotropic samples per texture sample
             * 
             * This often results in a small visual improvement over no samples. 
             */
            ANISOTROPY_X2  = 0x1,
            /**
             * @brief a setting to take four anisotropic samples per texture sample
             * 
             * This results in a significant visual improvement over no samples and above only two samples. 
             */
            ANISOTROPY_X4  = 0x3,
            /**
             * @brief a setting to take eight anisotropic samples per texture sample
             * 
             * This results in a very significant improvement over no samples and is often a good balance between performance visual fidelity and performance. 
             */
            ANISOTROPY_X8  = 0x7,
            /**
             * @brief a setting to take sixteen anisotropic samples per texture sample
             * 
             * This is often the best setting supported by GPUs as higher sample counts result in diminishing returns. As anisotropic filtering is not too expensive 
             * this is the suggested setting for modern desktops with discrete GPUs. 
             */
            ANISOTROPY_X16 = 0xf
        };

        /**
         * @brief define how the texture is filtered when a sample does not exactly align with the texels
         */
        enum Filter : u8 {
            /**
             * @brief for each sample look at the four surrounding texels and use the color of the closest one
             */
            FILTER_NEAREST = 0,
            /**
             * @brief use bilinear interpolation to interpolate the color of the four closest texels
             * 
             * For each sample look at the four surrounding texels and linearly interpolate the color of the bottom two and to two. 
             * Then, linearly interpolate between the two interpolation colors. The interpolation strength are determined by how far the elements
             * are apart on a one dimensional projected line. 
             */
            FILTER_LINEAR = 1
        };

        /**
         * @brief define how the texture behaves when it is sampled outside the UV space of 0 to 1 
         */
        enum Wrap : u8 {
            /**
             * @brief Only the fractional part (and 1) are used as UV values
             */
            WRAP_REPEAT = 0,
            /**
             * @brief Every time 1 is surpassed, the u or v value will go back from one down to 0 and then back up to 1
             */
            WRAP_MIRROR_REPEAT,
            /**
             * @brief if the value is less than 0 it is set to 0 and if it is larger than 1 it is set to 1
             */
            WRAP_CLAMP_TO_EDGE,
            /**
             * @brief when the value is outside the 0 to 1 range black is reported (actual black, alpha is 0 too)
             */
            WRAP_CLAMP_TO_BLACK
        };

        /**
         * @brief define comparison operations used optionally for e.g. depth texture sampling
         */
        enum CompareOp : u8 {
            /**
             * @brief the comparison always succeeds
             */
            COMPARE_OP_ALWAYS = 0,
            /**
             * @brief the comparison never succeeds
             */
            COMPARE_OP_NEVER,
            /**
             * @brief the comparison will succeed if the reference value is less than the texture value
             */
            COMPARE_OP_LESS,
            /**
             * @brief the comparison will succeed if the reference value is less or equal to the texture value
             */
            COMPARE_OP_LESS_EQUALS,
            /**
             * @brief the comparison will succeed if the reference value is greater than the texture value
             */
            COMPARE_OP_GREATER,
            /**
             * @brief the comparison will succeed if the reference value is greater or equal to the texture value
             */
            COMPARE_OP_GREATER_EQUALS,
            /**
             * @brief the comparison will succeed if the reference value is equal to the texture value
             */
            COMPARE_OP_EQUAL,
            /**
             * @brief the comparison will succeed if the reference value is not equal to the texture value
             */
            COMPARE_OP_NOT_EQUAL
        };

        /**
         * @brief Construct a new CPU Sampler
         */
        SamplerCPU() = default;

        /**
         * @brief Construct a new CPU Sampler
         * 
         * @param anisotropy the mode of anisotropic sampling to use (0 = undefined behaviour)
         * @param minification the filtering to use when downsampling the texture
         * @param magnification the filtering to use when upsampling the texture
         * @param wrap the wrap mode to use
         * @param mipChain the filter to use for mip chain interpolation
         * @param enableMipChain `true` to enable mip mapping, `false` to disable it
         * @param enableComparisons `true` to enable comparisons, `false` to disable comparisons
         * @param compareOp the operation to use for comparisons
         * @param minLod the minimum allowed level of detail
         * @param maxLod the maximum allowed level of detail
         */
        SamplerCPU(u8 anisotropy, Filter minification, Filter magnification, Wrap wrap, Filter mipChain = FILTER_LINEAR, bool enableMipChain = true, 
                   bool enableComparisons = false, CompareOp compareOp = COMPARE_OP_ALWAYS, f32 minLod = -1000.f, f32 maxLod = 1000.f)
         : m_anisotropy(anisotropy-1), m_filter_minification(minification), m_filter_magnification(magnification), m_filter_mipchain(mipChain), 
           m_mipchain_enabled(enableMipChain), m_wrap(wrap), m_compare_op(compareOp), m_compare_enabled(enableComparisons), m_lod_minimum(minLod), 
           m_lod_maximum(maxLod)
        {}

        /**
         * @brief Construct a new CPU Sampler
         * 
         * @param anisotropy the mode of anisotropic sampling to use
         * @param minification the filtering to use when downsampling the texture
         * @param magnification the filtering to use when upsampling the texture
         * @param mipChain the filter to use for mip chain interpolation
         * @param enableMipChain `true` to enable mip mapping, `false` to disable it
         * @param wrap the wrap mode to use
         * @param enableComparisons `true` to enable comparisons, `false` to disable comparisons
         * @param compareOp the operation to use for comparisons
         * @param minLod the minimum allowed level of detail
         * @param maxLod the maximum allowed level of detail
         */
        SamplerCPU(Anisotropic anisotropy, Filter minification, Filter magnification, Filter mipChain, bool enableMipChain, Wrap wrap, 
                   bool enableComparisons = false, CompareOp compareOp = COMPARE_OP_ALWAYS, f32 minLod = -1000.f, f32 maxLod = 1000.f)
         : m_anisotropy(static_cast<u8>(anisotropy)), m_filter_minification(minification), m_filter_magnification(magnification), m_filter_mipchain(mipChain), 
           m_mipchain_enabled(enableMipChain), m_wrap(wrap), m_compare_op(compareOp), m_compare_enabled(enableComparisons), m_lod_minimum(minLod), 
           m_lod_maximum(maxLod)
        {}

        /**
         * @brief Set the anisotropy level of the sampler
         * 
         * @param anisotropy the new anisotropy level for the sampler
         */
        inline void setAnisotropy(Anisotropic anisotropy) noexcept
        {m_anisotropy = anisotropy;}

        /**
         * @brief Get the anisotropy level of the sampler
         * 
         * @return `u8` the current anisotropy level
         */
        inline u8 getAnisotropy() const noexcept
        {return m_anisotropy;}

        /**
         * @brief Set the wrap mode of the sampler
         * 
         * @param wrap the wrap mode for the sampler
         */
        inline void setWrap(Wrap wrap) noexcept 
        {m_wrap = wrap;}

        /**
         * @brief Set the filter used for downsampling
         * 
         * @param filter the filter to use for downsampling
         */
        inline void setFilerMinification(Filter filter) noexcept
        {m_filter_minification = filter;}

        /**
         * @brief Get the current filter used for downsampling
         * 
         * @return `Filter` the filter that is used for downsampling
         */
        inline Filter getFilterMinification() const noexcept
        {return m_filter_minification;}

        /**
         * @brief Set the filter used for upsampling
         * 
         * @param filter the filter to use for upsampling
         */
        inline void setFilterMagnification(Filter filter) noexcept
        {m_filter_magnification = filter;}

        /**
         * @brief Get the current filter used for upsampling
         * 
         * @return `Filter` the filter that is used for upsampling
         */
        inline Filter getFilterMagnification() const noexcept
        {return m_filter_magnification;}

        /**
         * @brief Set the filter used for mip chain interpolation
         * 
         * @param filter the filter to use to interpolate on the mip chain
         */
        inline void setFilterMipChain(Filter filter) noexcept
        {m_filter_mipchain = filter;}

        /**
         * @brief Get the current filter used for interpolation along the mip chain
         * 
         * @return `Filter` the filter used for interpolation along the mip chain
         */
        inline Filter getFilterMipChain() const noexcept
        {return m_filter_mipchain;}

        /**
         * @brief Set if the mip chain is enabled
         * 
         * @param enabled `true` to enable mip chain interpolation, `false` to disable mip chain interpolation
         */
        inline void setMipChainEnabled(bool enabled) noexcept
        {m_mipchain_enabled = enabled;}

        /**
         * @brief Get if mip mapping is enabled
         * 
         * @return `true` if mipmapping is enabled, `false` otherwise
         */
        inline bool getMipChainEnabled() const noexcept
        {return m_mipchain_enabled;}

        /**
         * @brief Get the wrap mode used by the sampler
         * 
         * @return `Wrap` the current wrap mode used by the sampler
         */
        inline Wrap getWrap() const noexcept
        {return m_wrap;}

        /**
         * @brief Set if the compare operation is enabled
         * 
         * @param enabled `true` to enable the compare operation, `false` to disable the compare operation
         */
        inline void setCompareOpEnabled(bool enabled) noexcept
        {m_compare_enabled = enabled;}

        /**
         * @brief Get if the compare operation is currently enabled
         * 
         * @return `true` if the compare operation is enabled, `false` if it is disabled
         */
        inline bool getCompareOpEnabled() const noexcept
        {return m_compare_enabled;}

        /**
         * @brief Set the operation function to use for comparisons
         * 
         * @param compareOp the function to use for comparisons
         */
        inline void setCompareOp(CompareOp compareOp) noexcept
        {m_compare_op = compareOp;}

        /**
         * @brief Get the current function to use for comparisons
         * 
         * @return `CompareOp` the function used for comparisons
         */
        inline CompareOp getCompareOp() const noexcept
        {return m_compare_op;}

        /**
         * @brief Set the minimum level of detail
         * 
         * @param minimum the minimum level of detail value
         */
        inline void setLodMinimum(f32 minimum) noexcept
        {m_lod_minimum = minimum;}

        /**
         * @brief Get the minimum level of detail value
         * 
         * @return `f32` the minimum level of detail
         */
        inline f32 getLodMinimum() const noexcept
        {return m_lod_minimum;}

        /**
         * @brief Set the maximum level of detail value
         * 
         * @param maximum the maximum level of detail value
         */
        inline void setLodMaximum(f32 maximum) noexcept
        {m_lod_maximum = maximum;}

        /**
         * @brief Get the maximum level of detail value
         * 
         * @return `f32` the maximum level of detail value
         */
        inline f32 getLodMaximum() const noexcept
        {return m_lod_maximum;}

    protected:

        /**
         * @brief store the selected anisotropy mode
         */
        u8 m_anisotropy = ANISOTROPY_X8;
        /**
         * @brief store the selected filtering mode when downsampling
         */
        Filter m_filter_minification = FILTER_LINEAR;
        /**
         * @brief store the selected filtering mode when upsampling
         */
        Filter m_filter_magnification = FILTER_LINEAR;
        /**
         * @brief store the selected filtering mode for mip chain interpolation
         */
        Filter m_filter_mipchain = FILTER_LINEAR;
        /**
         * @brief `true` to enable the mip chain, `false` to disable mip mapping
         */
        bool m_mipchain_enabled = true;
        /**
         * @brief store the selected wrapping mode
         */
        Wrap m_wrap = WRAP_REPEAT;
        /**
         * @brief store the mode to use for comparisons
         */
        CompareOp m_compare_op = COMPARE_OP_ALWAYS;
        /**
         * @brief store if comparisons are enabled
         */
        bool m_compare_enabled = false;
        /**
         * @brief store the minimum allowed level of detail value
         * 
         * Default gotten by quarrying in NVidia NSight Graphics from OpenGL 4.6 Core default
         */
        f32 m_lod_minimum = -1000.f;
        /**
         * @brief store the maximal allowed level of detail value
         * 
         * Default gotten by quarrying in NVidia NSight Graphics from OpenGL 4.6 Core default
         */
        f32 m_lod_maximum = 1000.f;

    };

}

#endif