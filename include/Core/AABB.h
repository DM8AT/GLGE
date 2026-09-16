/**
 * @file AABB.h
 * @author DM8AT
 * @brief 
 * @version 0.1
 * @date 2026-06-05
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_CORE_AABB_
#define _GLGE_CORE_AABB_

//add common stuff
#include "Common.h"

//use the library namespace
namespace GLGE {

    /**
     * @brief store an axis aligned bounding box
     * 
     * This container is explicitly NOT SIMD-Optimized
     */
    class AABB {
    public:

        /**
         * @brief Construct a new AABB
         */
        AABB() = default;

        /**
         * @brief Construct a new axis aligned bounding box
         * 
         * @param lowerBounds the minimum values on all axis
         * @param upperBounds the upper bounds on all axis
         */
        AABB(const vec3& lowerBounds, const vec3& upperBounds)
         : m_lowest_x(lowerBounds.x), m_lowest_y(lowerBounds.y), m_lowest_z(lowerBounds.z), m_largest_x(upperBounds.x), m_largest_y(upperBounds.y), m_largest_z(upperBounds.z)
        {}

        /**
         * @brief Construct a new axis aligned bounding box
         * 
         * @param positions a span containing all positions to create the axis aligned bounding box from
         */
        AABB(const std::vector<vec3>& positions) {
            //iterate over all elements and store the minimum
            for (const auto& pos : positions) {
                //for all elements extract the minimum and maximum
                m_lowest_x = (m_lowest_x < pos.x) ? m_lowest_x : pos.x;
                m_lowest_y = (m_lowest_y < pos.y) ? m_lowest_y : pos.y;
                m_lowest_z = (m_lowest_z < pos.z) ? m_lowest_z : pos.z;
                m_largest_x = (m_largest_x > pos.x) ? m_largest_x : pos.x;
                m_largest_y = (m_largest_y > pos.y) ? m_largest_y : pos.y;
                m_largest_z = (m_largest_z > pos.z) ? m_largest_z : pos.z;
            }
        }

        /**
         * @brief Get the minimum corner of the axis aligned bounding box
         * 
         * @return `vec3` the lower corner defining the AABB
         */
        inline vec3 getMin() const noexcept
        {return vec3(m_lowest_x, m_lowest_y, m_lowest_z);}

        /**
         * @brief Get the maximum corner of the axis aligned bounding box
         * 
         * @return `vec3` the larger corner defining the AABB
         */
        inline vec3 getMax() const noexcept
        {return vec3(m_largest_x, m_largest_y, m_largest_z);}

        /**
         * @brief Get the Center of the axis aligned bounding box
         * 
         * @return `vec3` the center of the axis aligned bounding box
         */
        inline vec3 getCenter() const noexcept
        {return (getMin() + getMax())*vec3(0.5);}

        /**
         * @brief Get the extent of the axis aligned bounding box
         * 
         * @return `vec3` the extent of the axis aligned bounding box
         */
        inline vec3 getExtent() const noexcept
        {return getMax() - getMin();}

        /**
         * @brief Get the volume of the axis aligned bounding box
         * 
         * @return `float` the volume of the axis aligned bounding box
         */
        inline float getVolume() const noexcept
        {return (m_largest_x - m_lowest_x) * (m_largest_y - m_lowest_y) * (m_largest_z - m_lowest_z);}

    protected:

        /**
         * @brief store the minimum x value
         * 
         * This is not named `m_min_x` because windows defines `min` as a define, making `m_min` invalid because it replaces the min
         */
        float m_lowest_x = std::numeric_limits<f32>::infinity();
        /**
         * @brief store the minimum y value
         * 
         * This is not named `m_min_y` because windows defines `min` as a define, making `m_min` invalid because it replaces the min
         */
        float m_lowest_y = std::numeric_limits<f32>::infinity();
        /**
         * @brief store the minimum z value
         * 
         * This is not named `m_min_z` because windows defines `min` as a define, making `m_min` invalid because it replaces the min
         */
        float m_lowest_z = std::numeric_limits<f32>::infinity();
        /**
         * @brief store the maximum x value
         * 
         * This is not named `m_max_x` because windows defines `max` as a define, making `m_max` invalid because it replaces the max
         */
        float m_largest_x = 0;
        /**
         * @brief store the maximum y value
         * 
         * This is not named `m_max_y` because windows defines `max` as a define, making `m_max` invalid because it replaces the max
         */
        float m_largest_y = 0;
        /**
         * @brief store the maximum z value
         * 
         * This is not named `m_max_z` because windows defines `max` as a define, making `m_max` invalid because it replaces the max
         */
        float m_largest_z = 0;

    };

    //make sure that an AABB is as expected
    static_assert(sizeof(AABB) == 24, "Unexpected size for an AABB, expected 24 bytes, because SIMD optimization for an AABB is not used");

}

#endif