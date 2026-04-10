/**
 * @file Common.h
 * @author DM8AT
 * @brief define common type aliases as well as common functions
 * @version 0.1
 * @date 2025-12-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_CORE_COMMON_
#define _GLGE_CORE_COMMON_

//add the settings
#include "Settings.h"
//for specific sized integers
#include <cstdint>
//for simple printing (I'm still using C++20 style printing for backwards compatibility)
#include <iostream>

//add memcpy
#include <cstring>

//add generally usefull C++ library stuff

//for resizable lists
#include <vector>
//for unordered maps
#include <unordered_map>
//for runtime strings
#include <string>
//for constant strings
#include <string_view>

//add GLM for fast math
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>

/**
 * @brief the namespace that contains everything related to GLGE
 */
namespace GLGE {

    //define type aliases for specific sized integers

    /**
     * @brief a type for an unsigned 8 bit integer
     */
    typedef uint8_t  u8;
    /**
     * @brief a type for an unsigned 16 bit integer
     */
    typedef uint16_t u16;
    /**
     * @brief a type for an unsigned 32 bit integer
     */
    typedef uint32_t u32;
    /**
     * @brief a type for an unsigned 64 bit integer
     */
    typedef uint64_t u64;

    /**
     * @brief a type for a signed 8 bit integer
     */
    typedef int8_t  i8;
    /**
     * @brief a type for a signed 16 bit integer
     */
    typedef int16_t i16;
    /**
     * @brief a type for a signed 32 bit integer
     */
    typedef int32_t i32;
    /**
     * @brief a type for a signed 64 bit integer
     */
    typedef int64_t i64;

    /**
     * @brief a type for a 32 bit IEEE float
     */
    typedef float f32;
    /**
     * @brief a type for a 64 bit IEEE double
     */
    typedef double f64;

    /**
     * @brief define an integer type for the version of the library
     */
    typedef u32 Version_t;

    /**
     * @brief define a simple type that is used to serialize and de-serialize version numbers
     */
    struct Version {

        /**
         * @brief combine a major, minor and patch version to form a serialized version
         * 
         * @param major the major version number
         * @param minor the minor version number
         * @param patch the patch version number
         */
        constexpr Version(const Version_t& major, const Version_t& minor, const Version_t& patch)
         : m_value(GLGE_VERSION_COMBINE(major, minor, patch))
        {}

        /**
         * @brief create a version wrapper from a serialized version type value
         */
        constexpr Version(const Version_t& version) noexcept
         : m_value(version)
        {}

        /**
         * @brief Get the version serialized as the version type
         * 
         * @return `const Version_t&` a constant reference to the internal serialized value
         */
        inline constexpr const Version_t& getVersion_t() const noexcept 
        {return m_value;}

        /**
         * @brief get the version serialized as the version type
         * 
         * @return `Version_t` a copy of the internally stored serialized value
         */
        inline constexpr operator Version_t() const noexcept 
        {return getVersion_t();}

        /**
         * @brief Get the major version from the stored version
         * 
         * @return `constexpr Version_t` the major version of the stored version
         */
        inline constexpr Version_t getMajor() const noexcept
        {return GLGE_VERSION_GET_MAJOR(m_value);}

        /**
         * @brief Get the minor version from the stored version
         * 
         * @return `constexpr Version_t` the minor version of the stored version
         */
        inline constexpr Version_t getMinor() const noexcept
        {return GLGE_VERSION_GET_MINOR(m_value);}

        /**
         * @brief Get the patch version from the stored version
         * 
         * @return `constexpr Version_t` the patch version of the stored version
         */
        inline constexpr Version_t getPatch() const noexcept
        {return GLGE_VERSION_GET_PATCH(m_value);}

        /**
         * @brief Set the major version of the stored version
         * 
         * @param major the new value for the serialized value major version
         */
        inline constexpr void setMajor(const Version_t& major) noexcept{
            //simply store the new value
            m_value = (m_value & (GLGE_VERSION_MAJOR_MASK << GLGE_VERSION_MAJOR_OFFSET)) 
                    | ((major & GLGE_VERSION_MAJOR_MASK) << GLGE_VERSION_MAJOR_OFFSET);
        }

        /**
         * @brief Set the minor version of the stored version
         * 
         * @param minor the new value for the serialized value minor version
         */
        inline constexpr void setMinor(const Version_t& minor) noexcept{
            //simply store the new value
            m_value = (m_value & (GLGE_VERSION_MINOR_MASK << GLGE_VERSION_MINOR_OFFSET)) 
                    | ((minor & GLGE_VERSION_MINOR_MASK) << GLGE_VERSION_MINOR_OFFSET);
        }

        /**
         * @brief Set the patch version of the stored version
         * 
         * @param patch the new value for the serialized value patch version
         */
        inline constexpr void setPatch(const Version_t& patch) noexcept{
            //simply store the new value
            m_value = (m_value & (GLGE_VERSION_PATCH_MASK << GLGE_VERSION_PATCH_OFFSET)) 
                    | ((patch & GLGE_VERSION_PATCH_MASK) << GLGE_VERSION_PATCH_OFFSET);
        }

        /**
         * @brief print the version into an output stream
         * 
         * @param os the output stream to print the version to
         * @param v the version to print to the output stream
         * @return std::ostream& the filled output stream
         */
        inline friend std::ostream& operator<<(std::ostream& os, const Version& v) noexcept {
            return os << v.getMajor() << "." << v.getMinor() << "." << v.getPatch();
        }

    protected:

        /**
         * @brief store the serialized interpretation of the version
         */
        Version_t m_value = 0;

    };

    /**
     * @brief define a vector type that is of type `f32` with 1 axis (just a vector wrapper for an `f32`) and packed tightly
     */
    typedef glm::vec<1, f32, glm::packed_highp> f32vec1_p;
    /**
     * @brief define a vector type that is of type `f32` with 1 axis (just a vector wrapper for an `f32`) and uses default SIMD aware alignment
     */
    typedef glm::vec<1, f32, glm::defaultp> f32vec1_a;
    /**
     * @brief define a vector type that is of type `f32` with 2 axis and packed tightly
     */
    typedef glm::vec<2, f32, glm::packed_highp> f32vec2_p;
    /**
     * @brief define a vector type that is of type `f32` with 2 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<2, f32, glm::defaultp> f32vec2_a;
    /**
     * @brief define a vector type that is of type `f32` with 3 axis and packed tightly
     */
    typedef glm::vec<3, f32, glm::packed_highp> f32vec3_p;
    /**
     * @brief define a vector type that is of type `f32` with 3 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<3, f32, glm::defaultp> f32vec3_a;
    /**
     * @brief define a vector type that is of type `f32` with 4 axis and packed tightly
     */
    typedef glm::vec<4, f32, glm::packed_highp> f32vec4_p;
    /**
     * @brief define a vector type that is of type `f32` with 4 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<4, f32, glm::defaultp> f32vec4_a;

    /**
     * @brief define a vector type that is of type `f64` with 1 axis (just a vector wrapper for an `f64`) and packed tightly
     */
    typedef glm::vec<1, f64, glm::packed_highp> f64vec1_p;
    /**
     * @brief define a vector type that is of type `f64` with 1 axis (just a vector wrapper for an `f64`) and uses default SIMD aware alignment
     */
    typedef glm::vec<1, f64, glm::defaultp> f64vec1_a;
    /**
     * @brief define a vector type that is of type `f64` with 2 axis and packed tightly
     */
    typedef glm::vec<2, f64, glm::packed_highp> f64vec2_p;
    /**
     * @brief define a vector type that is of type `f64` with 2 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<2, f64, glm::defaultp> f64vec2_a;
    /**
     * @brief define a vector type that is of type `f64` with 3 axis and packed tightly
     */
    typedef glm::vec<3, f64, glm::packed_highp> f64vec3_p;
    /**
     * @brief define a vector type that is of type `f64` with 3 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<3, f64, glm::defaultp> f64vec3_a;
    /**
     * @brief define a vector type that is of type `f64` with 4 axis and packed tightly
     */
    typedef glm::vec<4, f64, glm::packed_highp> f64vec4_p;
    /**
     * @brief define a vector type that is of type `f64` with 4 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<4, f64, glm::defaultp> f64vec4_a;

    /**
     * @brief define a vector type that is of type `i8` with 1 axis (just a vector wrapper for an `i8`) and packed tightly
     */
    typedef glm::vec<1, i8, glm::packed_highp> i8vec1_p;
    /**
     * @brief define a vector type that is of type `i8` with 1 axis (just a vector wrapper for an `i8`) and uses default SIMD aware alignment
     */
    typedef glm::vec<1, i8, glm::defaultp> i8vec1_a;
    /**
     * @brief define a vector type that is of type `i8` with 2 axis and packed tightly
     */
    typedef glm::vec<2, i8, glm::packed_highp> i8vec2_p;
    /**
     * @brief define a vector type that is of type `i8` with 2 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<2, i8, glm::defaultp> i8vec2_a;
    /**
     * @brief define a vector type that is of type `i8` with 3 axis and packed tightly
     */
    typedef glm::vec<3, i8, glm::packed_highp> i8vec3_p;
    /**
     * @brief define a vector type that is of type `i8` with 3 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<3, i8, glm::defaultp> i8vec3_a;
    /**
     * @brief define a vector type that is of type `i8` with 4 axis and packed tightly
     */
    typedef glm::vec<4, i8, glm::packed_highp> i8vec4_p;
    /**
     * @brief define a vector type that is of type `i8` with 4 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<4, i8, glm::defaultp> i8vec4_a;

    /**
     * @brief define a vector type that is of type `i16` with 1 axis (just a vector wrapper for an `i16`) and packed tightly
     */
    typedef glm::vec<1, i16, glm::packed_highp> i16vec1_p;
    /**
     * @brief define a vector type that is of type `i16` with 1 axis (just a vector wrapper for an `i16`) and uses default SIMD aware alignment
     */
    typedef glm::vec<1, i16, glm::defaultp> i16vec1_a;
    /**
     * @brief define a vector type that is of type `i16` with 2 axis and packed tightly
     */
    typedef glm::vec<2, i16, glm::packed_highp> i16vec2_p;
    /**
     * @brief define a vector type that is of type `i16` with 2 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<2, i16, glm::defaultp> i16vec2_a;
    /**
     * @brief define a vector type that is of type `i16` with 3 axis and packed tightly
     */
    typedef glm::vec<3, i16, glm::packed_highp> i16vec3_p;
    /**
     * @brief define a vector type that is of type `i16` with 3 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<3, i16, glm::defaultp> i16vec3_a;
    /**
     * @brief define a vector type that is of type `i16` with 4 axis and packed tightly
     */
    typedef glm::vec<4, i16, glm::packed_highp> i16vec4_p;
    /**
     * @brief define a vector type that is of type `i16` with 4 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<4, i16, glm::defaultp> i16vec4_a;

    /**
     * @brief define a vector type that is of type `i32` with 1 axis (just a vector wrapper for an `i32`) and packed tightly
     */
    typedef glm::vec<1, i32, glm::packed_highp> i32vec1_p;
    /**
     * @brief define a vector type that is of type `i32` with 1 axis (just a vector wrapper for an `i32`) and uses default SIMD aware alignment
     */
    typedef glm::vec<1, i32, glm::defaultp> i32vec1_a;
    /**
     * @brief define a vector type that is of type `i32` with 2 axis and packed tightly
     */
    typedef glm::vec<2, i32, glm::packed_highp> i32vec2_p;
    /**
     * @brief define a vector type that is of type `i32` with 2 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<2, i32, glm::defaultp> i32vec2_a;
    /**
     * @brief define a vector type that is of type `i32` with 3 axis and packed tightly
     */
    typedef glm::vec<3, i32, glm::packed_highp> i32vec3_p;
    /**
     * @brief define a vector type that is of type `i32` with 3 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<3, i32, glm::defaultp> i32vec3_a;
    /**
     * @brief define a vector type that is of type `i32` with 4 axis and packed tightly
     */
    typedef glm::vec<4, i32, glm::packed_highp> i32vec4_p;
    /**
     * @brief define a vector type that is of type `i32` with 4 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<4, i32, glm::defaultp> i32vec4_a;

    /**
     * @brief define a vector type that is of type `i64` with 1 axis (just a vector wrapper for an `i64`) and packed tightly
     */
    typedef glm::vec<1, i64, glm::packed_highp> i64vec1_p;
    /**
     * @brief define a vector type that is of type `i64` with 1 axis (just a vector wrapper for an `i64`) and uses default SIMD aware alignment
     */
    typedef glm::vec<1, i64, glm::defaultp> i64vec1_a;
    /**
     * @brief define a vector type that is of type `i64` with 2 axis and packed tightly
     */
    typedef glm::vec<2, i64, glm::packed_highp> i64vec2_p;
    /**
     * @brief define a vector type that is of type `i64` with 2 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<2, i64, glm::defaultp> i64vec2_a;
    /**
     * @brief define a vector type that is of type `i64` with 3 axis and packed tightly
     */
    typedef glm::vec<3, i64, glm::packed_highp> i64vec3_p;
    /**
     * @brief define a vector type that is of type `i64` with 3 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<3, i64, glm::defaultp> i64vec3_a;
    /**
     * @brief define a vector type that is of type `i64` with 4 axis and packed tightly
     */
    typedef glm::vec<4, i64, glm::packed_highp> i64vec4_p;
    /**
     * @brief define a vector type that is of type `i64` with 4 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<4, i64, glm::defaultp> i64vec4_a;

    /**
     * @brief define a vector type that is of type `u8` with 1 axis (just a vector wrapper for an `u8`) and packed tightly
     */
    typedef glm::vec<1, u8, glm::packed_highp> u8vec1_p;
    /**
     * @brief define a vector type that is of type `u8` with 1 axis (just a vector wrapper for an `u8`) and uses default SIMD aware alignment
     */
    typedef glm::vec<1, u8, glm::defaultp> u8vec1_a;
    /**
     * @brief define a vector type that is of type `u8` with 2 axis and packed tightly
     */
    typedef glm::vec<2, u8, glm::packed_highp> u8vec2_p;
    /**
     * @brief define a vector type that is of type `u8` with 2 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<2, u8, glm::defaultp> u8vec2_a;
    /**
     * @brief define a vector type that is of type `u8` with 3 axis and packed tightly
     */
    typedef glm::vec<3, u8, glm::packed_highp> u8vec3_p;
    /**
     * @brief define a vector type that is of type `u8` with 3 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<3, u8, glm::defaultp> u8vec3_a;
    /**
     * @brief define a vector type that is of type `u8` with 4 axis and packed tightly
     */
    typedef glm::vec<4, u8, glm::packed_highp> u8vec4_p;
    /**
     * @brief define a vector type that is of type `u8` with 4 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<4, u8, glm::defaultp> u8vec4_a;

    /**
     * @brief define a vector type that is of type `u16` with 1 axis (just a vector wrapper for an `u16`) and packed tightly
     */
    typedef glm::vec<1, u16, glm::packed_highp> u16vec1_p;
    /**
     * @brief define a vector type that is of type `u16` with 1 axis (just a vector wrapper for an `u16`) and uses default SIMD aware alignment
     */
    typedef glm::vec<1, u16, glm::defaultp> u16vec1_a;
    /**
     * @brief define a vector type that is of type `u16` with 2 axis and packed tightly
     */
    typedef glm::vec<2, u16, glm::packed_highp> u16vec2_p;
    /**
     * @brief define a vector type that is of type `u16` with 2 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<2, u16, glm::defaultp> u16vec2_a;
    /**
     * @brief define a vector type that is of type `u16` with 3 axis and packed tightly
     */
    typedef glm::vec<3, u16, glm::packed_highp> u16vec3_p;
    /**
     * @brief define a vector type that is of type `u16` with 3 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<3, u16, glm::defaultp> u16vec3_a;
    /**
     * @brief define a vector type that is of type `u16` with 4 axis and packed tightly
     */
    typedef glm::vec<4, u16, glm::packed_highp> u16vec4_p;
    /**
     * @brief define a vector type that is of type `u16` with 4 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<4, u16, glm::defaultp> u16vec4_a;

    /**
     * @brief define a vector type that is of type `u32` with 1 axis (just a vector wrapper for an `u32`) and packed tightly
     */
    typedef glm::vec<1, u32, glm::packed_highp> u32vec1_p;
    /**
     * @brief define a vector type that is of type `u32` with 1 axis (just a vector wrapper for an `u32`) and uses default SIMD aware alignment
     */
    typedef glm::vec<1, u32, glm::defaultp> u32vec1_a;
    /**
     * @brief define a vector type that is of type `u32` with 2 axis and packed tightly
     */
    typedef glm::vec<2, u32, glm::packed_highp> u32vec2_p;
    /**
     * @brief define a vector type that is of type `u32` with 2 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<2, u32, glm::defaultp> u32vec2_a;
    /**
     * @brief define a vector type that is of type `u32` with 3 axis and packed tightly
     */
    typedef glm::vec<3, u32, glm::packed_highp> u32vec3_p;
    /**
     * @brief define a vector type that is of type `u32` with 3 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<3, u32, glm::defaultp> u32vec3_a;
    /**
     * @brief define a vector type that is of type `u32` with 4 axis and packed tightly
     */
    typedef glm::vec<4, u32, glm::packed_highp> u32vec4_p;
    /**
     * @brief define a vector type that is of type `u32` with 4 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<4, u32, glm::defaultp> u32vec4_a;

    /**
     * @brief define a vector type that is of type `u64` with 1 axis (just a vector wrapper for an `u64`) and packed tightly
     */
    typedef glm::vec<1, u64, glm::packed_highp> u64vec1_p;
    /**
     * @brief define a vector type that is of type `u64` with 1 axis (just a vector wrapper for an `u64`) and uses default SIMD aware alignment
     */
    typedef glm::vec<1, u64, glm::defaultp> u64vec1_a;
    /**
     * @brief define a vector type that is of type `u64` with 2 axis and packed tightly
     */
    typedef glm::vec<2, u64, glm::packed_highp> u64vec2_p;
    /**
     * @brief define a vector type that is of type `u64` with 2 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<2, u64, glm::defaultp> u64vec2_a;
    /**
     * @brief define a vector type that is of type `u64` with 3 axis and packed tightly
     */
    typedef glm::vec<3, u64, glm::packed_highp> u64vec3_p;
    /**
     * @brief define a vector type that is of type `u64` with 3 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<3, u64, glm::defaultp> u64vec3_a;
    /**
     * @brief define a vector type that is of type `u64` with 4 axis and packed tightly
     */
    typedef glm::vec<4, u64, glm::packed_highp> u64vec4_p;
    /**
     * @brief define a vector type that is of type `u64` with 4 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<4, u64, glm::defaultp> u64vec4_a;

    //define aliases for the most common vector types

    /**
     * @brief an alias for a vector of type `f32` with 2 axis
     */
    typedef f32vec2_a vec2;

    /**
     * @brief an alias for a vector of type `f32` with 3 axis
     */
    typedef f32vec3_a vec3;

    /**
     * @brief an alias for a vector of type `f32` with 4 axis
     */
    typedef f32vec4_a vec4;

    /**
     * @brief an alias for a vector of type `i32` with 2 axis
     */
    typedef i32vec2_a ivec2;

    /**
     * @brief an alias for a vector of type `i32` with 3 axis
     */
    typedef i32vec3_a ivec3;

    /**
     * @brief an alias for a vector of type `i32` with 4 axis
     */
    typedef i32vec4_a ivec4;

    /**
     * @brief an alias for a vector of type `u32` with 2 axis
     */
    typedef u32vec2_a uvec2;

    /**
     * @brief an alias for a vector of type `u32` with 3 axis
     */
    typedef u32vec3_a uvec3;

    /**
     * @brief an alias for a vector of type `u32` with 4 axis
     */
    typedef u32vec4_a uvec4;

    //sanity check

    /**
     * @brief check the size of a single precision IEEE float
     */
    static_assert(sizeof(f32) == 4, "A float is not 32 bits (4 bytes) in size, but GLGE requires floats to be 4 bytes in size to work correctly");
    /**
     * @brief check the size of a double precision IEEE float
     */
    static_assert(sizeof(f64) == 8, "A double is not 64 bits (8 bytes) in size, but GLGE requires doubles to be 8 bytes in size to work correctly");

    /**
     * @brief a quaternion is a 4D imaginary number. They are commonly used to store rotations. 
     */
    typedef glm::quat Quaternion;

}

//add 16 bit floats
#include "F16.h"

/**
 * @brief re-enter the namespace of the library to define the missing vector types for 16 bit floats
 */
namespace GLGE {

    /**
     * @brief define a vector type that is of type `f16` with 1 axis (just a vector wrapper for an `f16`) and packed tightly
     */
    typedef glm::vec<1, f16, glm::packed_highp> f16vec1_p;
    /**
     * @brief define a vector type that is of type `f16` with 1 axis (just a vector wrapper for an `f16`) and uses default SIMD aware alignment
     */
    typedef glm::vec<1, f16, glm::defaultp> f16vec1_a;
    /**
     * @brief define a vector type that is of type `f16` with 2 axis and packed tightly
     */
    typedef glm::vec<2, f16, glm::packed_highp> f16vec2_p;
    /**
     * @brief define a vector type that is of type `f16` with 2 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<2, f16, glm::defaultp> f16vec2_a;
    /**
     * @brief define a vector type that is of type `f16` with 3 axis and packed tightly
     */
    typedef glm::vec<3, f16, glm::packed_highp> f16vec3_p;
    /**
     * @brief define a vector type that is of type `f16` with 3 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<3, f16, glm::defaultp> f16vec3_a;
    /**
     * @brief define a vector type that is of type `f16` with 4 axis and packed tightly
     */
    typedef glm::vec<4, f16, glm::packed_highp> f16vec4_p;
    /**
     * @brief define a vector type that is of type `f16` with 4 axis and uses default SIMD aware alignment
     */
    typedef glm::vec<4, f16, glm::defaultp> f16vec4_a;

};

#endif