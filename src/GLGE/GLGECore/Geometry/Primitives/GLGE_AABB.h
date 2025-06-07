/**
 * @file GLGE_AABB.h
 * @author DM8AT
 * @brief define what an AABB (Axis alligned bounding box) is
 * @version 0.1
 * @date 2025-06-04
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_AABB_
#define _GLGE_AABB_

//include the settings
#include "../../GLGESettings.h"
//include utilites for the C function definitions
#include "../../GLGEUtil.h"
//include all vector types
#include "../../../GLGEMath/Vectors/GLGEVectors.h"
//include mathematics for the square root
#include <math.h>

//check if this is C++
#if GLGE_CPP

/**
 * @brief store an axis alligned bounding box using its minimum and maximum values on all axis
 * @warning this ignores translation and scaling
 */
class AABB
{
public:

    /**
     * @brief Construct a new AABB
     */
    AABB() = default;

    /**
     * @brief Construct a new AABB
     * 
     * @param positions an array of positions to extract the minimum and maximum from
     * @param posCount the amount of elements in the position array
     */
    AABB(const vec3* positions, uint64_t posCount) noexcept;

    /**
     * @brief Construct a new AABB
     * 
     * @param min the per axis minimum for the bounding box
     * @param max the per axis maximum for the bounding box
     */
    inline AABB(const vec3& min, const vec3& max) noexcept
     : m_min(min), m_max(max) {}

    /**
     * @brief Get the minimum of the bounding box
     * 
     * @return const vec3& the per axis minimum of the bounding box
     */
    inline const vec3& getMin() const noexcept {return m_min;}

    /**
     * @brief Get the maximum of the bounding box
     * 
     * @return const vec3& get the maximum of the bounding box
     */
    inline const vec3& getMax() const noexcept {return m_max;}

    /**
     * @brief Set the minimum value for the AABB
     * 
     * @param min the new minimum value
     */
    inline void setMin(const vec3& min) noexcept {m_min = min;}

    /**
     * @brief Set the maximum value for the AABB
     * 
     * @param max the new maximum value
     */
    inline void setMax(const vec3& max) noexcept {m_max = max;}

    /**
     * @brief move the box in 3D space
     * 
     * @param delta the difference in position to change the box by
     */
    void translate(const vec3& delta) noexcept;

    /**
     * @brief scale the box by some amount
     * 
     * @param scale the per axis scalars for the box
     */
    void scale(const vec3& scale) noexcept;

    /**
     * @brief combine two axis alligned bounding box to a new bounding box 
     * that surrounds both inputed bounding boxes
     * 
     * @param other the bounding box to combine with
     * @return AABB the bounding box encompassing both bounding boxes
     */
    inline AABB combine(const AABB& other) const noexcept
    {
        //return a new axis alligned bounding box
        return AABB(
            //search all minimums from both boxes
            vec3(
                (m_min.x < other.m_min.x) ? m_min.x : other.m_min.x, 
                (m_min.y < other.m_min.y) ? m_min.y : other.m_min.y, 
                (m_min.z < other.m_min.z) ? m_min.z : other.m_min.z
            ), 
            //search all maximums from both boxes
            vec3(
                (m_max.x > other.m_max.x) ? m_max.x : other.m_max.x,
                (m_max.y > other.m_max.y) ? m_max.y : other.m_max.y,
                (m_max.z > other.m_max.z) ? m_max.z : other.m_max.z
            )
        );
    }

    /**
     * @brief return a bounding box that surrounds both bounding boxes
     * 
     * @param other the bounding box to add to the volume
     * @return AABB the other united bounding box
     */
    inline AABB operator|(const AABB& other) const noexcept {return combine(other);}

    /**
     * @brief combine both AABBs so the resulting AABB is the area that both AABBs include
     * 
     * @warning the resulting AABB may be inverted
     * 
     * @param other the other AABB to unite with
     * @return AABB the area enclosed by both AABBs
     */
    inline AABB unite(const AABB& other) const noexcept
    {
        //return an AABB
        return AABB(
            //compute the per-axis maximum of both minimas
            vec3(
                (m_min.x > other.m_min.x) ? m_min.x : other.m_min.x, 
                (m_min.y > other.m_min.y) ? m_min.y : other.m_min.y, 
                (m_min.z > other.m_min.z) ? m_min.z : other.m_min.z
            ),
            //compute the per-axis minimum of both maximas
            vec3(
                (m_max.x < other.m_max.x) ? m_max.x : other.m_max.x,
                (m_max.y < other.m_max.y) ? m_max.y : other.m_max.y,
                (m_max.z < other.m_max.z) ? m_max.z : other.m_max.z
            )
        );
    }

    /**
     * @brief combine both AABBs so the resulting AABB is the area that both AABBs include
     * 
     * @warning the resulting AABB may be inverted
     * 
     * @param other the other AABB to unite with
     * @return AABB the area enclosed by both AABBs
     */
    inline AABB operator&(const AABB& other) const noexcept {return unite(other);}

    /**
     * @brief check if this AABB is inverted
     * 
     * @return true : the AABB is inverted
     * @return false : the AABB is not inverted
     */
    inline bool isInverted() const noexcept 
    {
        //the AABB is inverted if some minimum value is less than the maximum corresponding maximum value
        return (m_max.x < m_min.x) || (m_max.y < m_min.y) || (m_max.z < m_max.z);
    }

    /**
     * @brief create a new AABB that is this AABB, but inverted
     * 
     * @return AABB the new inverted AABB
     */
    inline AABB invert() const noexcept
    {
        //to invert the AABB, simply swap minima and maxima
        return AABB(m_max, m_min);
    }

    /**
     * @brief create a new AABB that is this AABB, but inverted
     * 
     * @return AABB the new inverted AABB
     */
    inline AABB operator-() const noexcept {return invert();}

    /**
     * @brief invert this AABB by flipping the minimum and maximum around
     */
    void invertThis() noexcept;

    /**
     * @brief combine two axis alligned bounding box and store the resulting
     * bounding box as this bounding box
     * 
     * @param other the bounding box to combine with
     */
    void combineToThis(const AABB& other) noexcept;

    /**
     * @brief combine two axis alligned bounding box and store the resulting
     * bounding box as this bounding box
     * 
     * @param other the bounding box to combine with
     */
    inline void operator|=(const AABB& other) noexcept {combineToThis(other);}

    /**
     * @brief combine both AABBs so the this AABB is the area that both AABBs include
     * 
     * @warning this AABB may be inverted after the operation
     * 
     * @param other the other AABB to unite with
     */
    void uniteToThis(const AABB& other) noexcept;

    /**
     * @brief combine both AABBs so the this AABB is the area that both AABBs include
     * 
     * @warning this AABB may be inverted after the operation
     * 
     * @param other the other AABB to unite with
     */
    inline void operator&=(const AABB& other) noexcept {uniteToThis(other);}

    /**
     * @brief Get the squared distance between the inputed point and the AABB
     * 
     * @param point the point to get the distance to
     * @return float the squared distance between the AABB and the point
     */
    float getSqrDistanceFrom(const vec3& point) const noexcept;

    /**
     * @brief Get the euclidian distance between the AABB and the inputed point
     * @warning This uses the square root operation. Just for comparisons it's faster to use the squared distance
     * 
     * @param point the point to calculate the distance to
     * @return float the euclidian distance between the AABB and the inputed point
     */
    inline float getDistanceFrom(const vec3& point) const noexcept {return sqrt(getSqrDistanceFrom(point));};

    /**
     * @brief check if the axis alligned bounding box contains a specifc point
     * 
     * @param position the position of the point to check
     * @return true : the point is contained in the box
     * @return false : the point is not in the box
     */
    inline bool contains(const vec3& position) const noexcept {
        //seperatly check for each axis if the point is in the box
        return (
            (m_min.x <= position.x && m_max.x >= position.x) &&
            (m_min.y <= position.y && m_max.y >= position.y) &&
            (m_min.z <= position.z && m_max.z >= position.z)
        );
    }

    /**
     * @brief add the collision function as a friend
     * 
     * @param a the first bounding box to check for collision
     * @param b the second bounding box to check for collision
     * @return true : the bounding boxes overlap somewhere
     * @return false : the bonding boxes don't overlap
     */
    friend bool collide(const AABB& a, const AABB& b) noexcept;

    /**
     * @brief print the axis alligned bounding box into an output stream
     * 
     * @param os the output stream to print to
     * @param a the axis alligned bounding box to print
     * @return std::ostream& the filled output stream
     */
    inline friend std::ostream& operator<<(std::ostream& os, const AABB& a) noexcept 
    {return os << "AABB{min: " << a.m_min << ", max: " << a.m_max << "}";}

protected:

    /**
     * @brief store the per-axis minimum of the axis alligned bounding box
     * This can also be interpreted as one corner
     */
    vec3 m_min;
    /**
     * @brief store ther per-axis maximum of the axis alligned bounding box
     * This can also be interpreted as the corner on the diagonaly other side of the box 
     * from the corner represented by the minimum
     */
    vec3 m_max;

};

//start a section for C
extern "C"
{
#endif //C++ section

/**
 * @brief store an instance of the axis alligned bounding box class
 */
typedef struct s_AABB AABB_t;

/**
 * @brief create a new axis alligned bounding box from a minimum and maximum
 * 
 * @param min the minimum on all axis for the bounding box
 * @param max the maximum on all axis for the bounding box
 * @return AABB_t* a new instance of the bounding box class or 0 on an error
 */
AABB_t* GLGE_C_FUNC(aabb_Create)(const vec3* min, const vec3* max);

/**
 * @brief create a new axis alligned bounding box from a point cloude
 * 
 * @param points the list of points to create the axis alligned bounding box from
 * @param pointCount the amount of points in the point list
 * @return AABB_t* a new instance of the bounding box class or 0 on an error
 */
AABB_t* GLGE_C_FUNC(aabb_FromPoints)(const vec3* points, uint64_t pointCount);

/**
 * @brief delete an AABB object. 
 * @warning this invalidates all pointers to this object
 * 
 * @param aabb the AABB to destroy
 */
void GLGE_C_FUNC(aabb_Destroy)(AABB_t* aabb);

/**
 * @brief querry the value of the minimum on all axis from an AABB
 * 
 * @param aabb the AABB to querry the data from
 * @return const vec3* a constant pointer to the minimum value. Valid as long as the object isn't moved or deleted. 
 */
const vec3* GLGE_C_FUNC(aabb_getMin)(const AABB_t* aabb);

/**
 * @brief querry the value of the maximum on all axis from an AABB
 * 
 * @param aabb the AABB to querry the data from
 * @return const vec3* a constant pointer to the maximum value. Valid as long as the object isn't moved or deleted. 
 */
const vec3* GLGE_C_FUNC(aabb_getMax)(const AABB_t* aabb);

/**
 * @brief override the value for the minimum on all axis of an AABB
 * 
 * @param min the new minimum value on all axis
 * @param aabb a pointer to the axis alligned bounding box to set the new minimum for
 */
void GLGE_C_FUNC(aabb_setMin)(const vec3* min, AABB_t* aabb);

/**
 * @brief override the value for the maximum on all axis of an AABB
 * 
 * @param max the new maximum value on all axis
 * @param aabb a pointer to the axis alligned bounding box to set the new maximum for
 */
void GLGE_C_FUNC(aabb_setMax)(const vec3* max, AABB_t* aabb);

/**
 * @brief move an aabb in 3D space
 * 
 * @param delta the difference in position to change the box by
 * @param aabb a pointer to the aabb to move
 */
void GLGE_C_FUNC(aabb_translate)(const vec3* delta, AABB_t* aabb);


/**
 * @brief scale an aabb by some amount
 * 
 * @param scale the per axis scalars for the box
 * @param aabb a pointer to the axis alligned bounding box to scale
 */
void GLGE_C_FUNC(aabb_scale)(const vec3* scale, AABB_t* aabb);

/**
 * @brief combine two axis alligned bounding boxes togheter and create a new bounding box from them
 * 
 * @param first one of the bounding boxes to use for combination
 * @param other the other bounding box to use for combination
 * @return AABB_t* all the area that lies in the bounding box surrounding both sub-boxes
 */
AABB_t* GLGE_C_FUNC(aabb_combine)(const AABB_t* first, const AABB_t* other);

/**
 * @brief combine two axis alligned bounding boxes and store the resulting bounding box in the first box
 * 
 * @param first one of the bounding boxes to use for combination and the bounding box to store the resulting box in
 * @param other the other bounding box to use for combination. It won't be changed. 
 */
void GLGE_C_FUNC(aabb_combineToFirst)(AABB_t* first, const AABB_t* other);

/**
 * @brief calculate the area both bounding boxes cover. 
 * @warning if they have no common area, the resulting AABB will enclose the area between both boxes and be inverted. 
 * 
 * @param first the first box to use for the unition
 * @param other the other box to use for the unition
 * @return AABB_t* the new aabb resulting from the union of AABBs
 */
AABB_t* GLGE_C_FUNC(aabb_unite)(const AABB_t* first, const AABB_t* other);

/**
 * @brief combine two axis aligned bounding boxes and store the result in the first bounding box
 * 
 * @param first the first bounding box to use for the unition and the bounding box the result will be stored in
 * @param other the other bounding box to use for the unition. This bounding box won't be changed
 */
void GLGE_C_FUNC(aabb_uniteToFirst)(AABB_t* first, const AABB_t* other);

/**
 * @brief check if an axis alligned bounding box is inverted
 * 
 * @param aabb the axis alligned bounding box to check for inversion
 * @return true : the minimum and maximum value of the bounding box are inverted
 * @return false : the minimum and maximum value of the bounding box are not inverted
 */
bool GLGE_C_FUNC(aabb_isInverted)(const AABB_t* aabb);

/**
 * @brief create a new bounding box by inverting an allready existing one
 * 
 * @param aabb the axis alligned bounding box to invert
 * @return AABB_t* the new bounding box that is a replica of the input, but minimum and maximum are swapped
 */
AABB_t* GLGE_C_FUNC(aabb_invert)(const AABB_t* aabb);

/**
 * @brief inverts the inputed axis alligned bounding box
 * 
 * @param aabb the bounding box to invert
 */
void GLGE_C_FUNC(aabb_invertThis)(AABB_t* aabb);

/**
 * @brief check if the axis alligned bounding box contains a specified position
 * 
 * @param pos the position to check if it is inside
 * @param aabb the bounding box to check if the point is inside
 */
bool GLGE_C_FUNC(aabb_contains)(const vec3* pos, AABB_t* aabb);

//check for C++ to end a potential C-Section
#if GLGE_CPP
}
#endif //ended C-Section

#endif