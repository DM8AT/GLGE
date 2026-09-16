/**
 * @file F16.h
 * @author DM8AT
 * @brief define a 16 bit float type
 * @version 0.1
 * @date 2025-12-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_CORE_F16_
#define _GLGE_CORE_F16_

//the common stuff is required
#include "Common.h"
//add the profiler
#include "Profiler.h"

/**
 * @brief use the libraries namespace
 */
namespace GLGE {

    /**
     * @brief define a structure to simulate 16 bit IEEE floats (also called halfs)
     * 
     * The sizes are as follows:
     * | Section  | Size in bits |
     * |----------|--------------|
     * | Sign     | 1            |
     * | Exponent | 5            |
     * | Mantissa | 10           |
     */
    struct f16 {

        /**
         * @brief store the amount of bits the exponent takes up
         */
        inline static constexpr const u8 EXPONENT_BITS = 5u;

        /**
         * @brief store the amount of bits the mantissa takes up
         */
        inline static constexpr const u8 MANTISSA_BITS = 10u;

        /**
         * @brief Construct a new 16 bit float
         * 
         * @param value the 32 bit float to create the 16 bit float from
         */
        inline constexpr f16(f32 value) noexcept {
            GLGE_PROFILER_SCOPE();

            //raw bit access
            u32 v = *((u32*)&value);

            //extract the different elements
            u32 sign = (v >> 0x1F) & 0x01u;
            u32 exp  = (v >> 0x17) & 0xFFu;
            u32 mant = (v >> 0x00) & 0x7FFFFFu;

            //store the different parts for the half float
            u16 h_sign = sign << 0xFu;
            u16 h_exp  = 0u;
            u16 h_mant = 0u;

            //handle inf and nan
            if (exp == 0xFFu) {
                //float is inf or nan
                h_exp = 0x1Fu << 0xAu;
                //preserve nan value (quiet canonical)
                h_mant = (mant ? 0x200u : 0u);
                //half is finished
                data = h_sign | h_exp | h_mant;
            }

            //normalized float
            i32 new_exp = (i32)exp - 0x70u;
            //overflow detection
            if (new_exp >= 0x1Fu) {
                //convert to infinity
                h_exp = 0x1Fu << 0xAu;
                data = h_sign | h_exp;
            }
            //underflow / subnormal half detection
            if (new_exp <= 0u) {
                //too small check
                if (new_exp < -0xA) {
                    //store 0
                    data = h_sign;
                }
                //convert to subnormal half
                mant |= 0x800000u;
                int shift = 0xEu - new_exp;

                //compute rounding: round-to-nearest-even
                u32 round_bit  = 1u << (shift - 1u);            //the 'round' bit
                u32 shifted    = (u16)(mant >> shift);    //candidate mantissa

                //sticky: any bits below round_bit
                u32 sticky = mant & (round_bit - 1u);

                //apply tie-to-even rounding
                if ((mant & round_bit) && (sticky || (shifted & 1u))) {
                    ++shifted;
                }

                //if rounding produced a carry into the implicit leading 1 (i.e. becomes 0x400)
                //for subnormal -> this means value normalized to exponent = 1
                if (shifted & (1u << (0xAu))) {
                    //shifted == 0x400
                    h_exp = 1u << 0xAu;
                    h_mant = 0u;
                    //store the finished half
                    data = h_sign | h_exp | h_mant;
                }

                h_mant = shifted & 0x3FFu;
                //store the finished half
                data = h_sign | h_mant;
            }

            //normal half
            h_exp = ((u16)new_exp) << 0xAu;
            h_mant = (u16)(mant >> 0xDu);

            //round-to-nearest-even:
            {
                //bit 12 (0x1000)
                u32 round_mask = 1u << (0xDu - 1u);
                //bits below round bit
                u32 lower_mask = round_mask - 1u;
                u32 round_bit  = (mant & round_mask) ? 1u : 0u;
                u32 sticky     = mant & lower_mask;
                u16 mant_candidate = h_mant;

                if (round_bit && (sticky || (mant_candidate & 1u))) {
                    ++mant_candidate;
                }

                //handle mantissa overflow (carry into exponent)
                if (mant_candidate & (1u << (0xAu))) { 
                    //mant_candidate == 0x400
                    //increment exponent, mantissa becomes 0
                    h_exp += 1u << 0xAu;
                    mant_candidate = 0u;

                    //check exponent overflow -> infinity
                    if (h_exp >= (0x1Fu << 0xAu)) {
                        h_exp = 0x1Fu << 0xAu;
                        h_mant = 0u;
                        data = h_sign | h_exp | h_mant;
                    }
                }
                h_mant = mant_candidate & 0x3FFu;
            }

            //store the finished half
            data = h_sign | h_exp | (h_mant & 0x3FFu);
        }

        /**
         * @brief a function that converts the 16 bit float to a 32 bit float
         * 
         * @return `f32` the 32 bit floating point value created from the 16 bit float value
         */
        inline constexpr operator f32() const noexcept {
            GLGE_PROFILER_SCOPE();

            //extract the different elements
            u32 sign = (data >> 0xFu) & 0x01u;
            u32 exp  = (data >> 0xAu) & 0x1Fu;
            u32 mant = (data >> 0x0u) & 0x3FFu;

            //the resulting float bit pattern
            u32 f_sign = sign << 0x1Fu;
            u32 f_exp  = 0u;
            u32 f_mant = 0u;

            //handle inf or nan
            if (exp == 0x1Fu) {
                //half is inf or nan
                f_exp = 0xFFu << 0x17u;
                //preserve nan
                f_mant = (mant ? 0x7FFFFFu : 0u);
                //assemble float
                u32 out = f_sign | f_exp | f_mant;
                return *((f32*)&out);
            }

            //handle zero and subnormal
            if (exp == 0) {
                if (mant == 0) {
                    //zero (signed)
                    u32 out = f_sign;
                    return *((float*)&out);
                }

                //subnormal half: renormalize
                int shift = 0;
                while ((mant & 0x400) == 0) {
                    mant <<= 1;
                    ++shift;
                }

                //remove the leading 1 again
                mant &= 0x3FFu;

                //compute float exponent
                //   127 (float bias) - 14 (half bias offset) - shift
                i32 new_exp = 0x7Fu - 0xEu - shift;

                f_exp  = (u32)new_exp << 0x17u;
                f_mant = mant << 0xDu;

                //assemble float
                u32 out = f_sign | f_exp | f_mant;
                return *((float*)&out);
            }

            //normal half
            //convert exponent: exp_half - 15 + 127
            i32 new_exp = (i32)exp - 0xFu + 0x7Fu;

            f_exp  = (u32)new_exp << 0x17u;
            f_mant = mant << 0xDu;

            //assemble float
            u32 out = f_sign | f_exp | f_mant;
            return *((float*)&out);
        }

    //the raw binary data is protected to make the structure look more like a built-in type
    protected:

        /**
         * @brief store the raw binary data for the 16 bit float
         */
        u16 data;
    };

    //sanity check the size of the float 16 implementation
    static_assert(sizeof(f16) == 2, "The implementation of the f16 (float 16, half) structure is not 16 bits (2 bytes)");

}

#endif