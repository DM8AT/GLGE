/**
 * @file GLGEVec2Preset.h
 * @author DM8AT
 * @brief a preset for a 3D vector
 * @warning DO NOT USE UNLESS YOUR KNOWING HOW TO CORRECTLY USE IT
 * @version 0.1
 * @date 2025-04-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//inclue the utility functions
#include "../../../GLGECore/GLGEUtil.h"
//include the settings
#include "../../../GLGECore/GLGESettings.h"

//check if all defines are set correctly
//check if the type of the vector is set
#ifndef GLGE_VEC_TYPE
//check if compiler errors are enabled
#if GLGE_COMPILER_ERRORS
//print an error
#error [ERROR] Failed to create vector: the type of the vector that should be created was not set
#endif //error
#else
//check if the name of the vector is set
#ifndef GLGE_VEC_NAME
//check if compiler errors are enabled
#if GLGE_COMPILER_ERRORS
//print an error
#error [ERROR] Failed to create vector: the name of the vector that should be created was not set
#endif //error
#else
//check if the name of the struct of the vector is set
#ifndef GLGE_VEC_STRUCT_NAME
//check if compiler errors are enabled
#if GLGE_COMPILER_ERRORS
//print an error
#error [ERROR] Failed to create vector: the name of the struct of the vector that should be created was not set
#endif //error
#else

//also available in C
#if GLGE_CPP
//start a section for C
extern "C"
{
#endif

/**
 * @brief store a three dimensional vector
 */
typedef struct GLGE_VEC_STRUCT_NAME {
    //check for C++ for constructor and operator functions
    #if GLGE_CPP
    /**
     * @brief default constructor
     */
    inline GLGE_VEC_STRUCT_NAME() noexcept : x(0), y(0), z(0) {}

    /**
     * @brief Construct a new three dimensional vector
     * 
     * @param _xyz the value for the x, y and z axis
     */
    inline GLGE_VEC_STRUCT_NAME(const GLGE_VEC_TYPE& _xyz) noexcept : x(_xyz), y(_xyz), z(_xyz) {}

    /**
     * @brief Construct a new three dimensional vector
     * 
     * @param _x the new value for the x axis
     * @param _y the new value for the y axis
     * @param _z the new value for the z axis
     */
    inline GLGE_VEC_STRUCT_NAME(const GLGE_VEC_TYPE& _x, const GLGE_VEC_TYPE& _y, const GLGE_VEC_TYPE& _z) noexcept : x(_x), y(_y), z(_z) {}

    /**
     * @brief invert the sign of the vectors
     * 
     * @return the inversed vector
     */
    inline GLGE_VEC_STRUCT_NAME operator-() const noexcept {return GLGE_VEC_STRUCT_NAME(-x, -y, -z);}

    /**
     * @brief add two 3D vectors together
     * 
     * @param ot a constant reference to the other vector
     * @return the sum of the two vectors
     */
    inline GLGE_VEC_STRUCT_NAME operator+(const GLGE_VEC_STRUCT_NAME& ot) const noexcept {return GLGE_VEC_STRUCT_NAME(ot.x + x, ot.y + y, ot.z + z);}

    /**
     * @brief subtract the other vector from this vector
     * 
     * @param ot a constant reference to the other vector
     * @return the difference of the vectors
     */
    inline GLGE_VEC_STRUCT_NAME operator-(const GLGE_VEC_STRUCT_NAME& ot) const noexcept {return GLGE_VEC_STRUCT_NAME(x - ot.x, y - ot.y, z - ot.z);}

    /**
     * @brief multiply the two vectors per element together
     * 
     * @param ot a constant reference to the other vector
     * @return the per-element product of the two vectors
     */
    inline GLGE_VEC_STRUCT_NAME operator*(const GLGE_VEC_STRUCT_NAME& ot) const noexcept {return GLGE_VEC_STRUCT_NAME(x * ot.x, y * ot.y, z * ot.z);}

    /**
     * @brief divide the two vectors per element
     * 
     * @param ot a constant reference to the other vector
     * @return the per-element fraction of the two vectors
     */
    inline GLGE_VEC_STRUCT_NAME operator/(const GLGE_VEC_STRUCT_NAME& ot) const noexcept {return GLGE_VEC_STRUCT_NAME(x / ot.x, y / ot.y, z / ot.z);}

    /**
     * @brief add two 2D vectors together
     * 
     * @param ot a constant reference to the other vector
     */
    inline void operator+=(const GLGE_VEC_STRUCT_NAME& ot) noexcept {x += ot.x; y += ot.y; z += ot.z;}

    /**
     * @brief subtract the other vector from this vector
     * 
     * @param ot a constant reference to the other vector
     */
    inline void operator-=(const GLGE_VEC_STRUCT_NAME& ot) noexcept {x -= ot.x; y -= ot.y; z -= ot.z;}

    /**
     * @brief multiply the two vectors per element together
     * 
     * @param ot a constant reference to the other vector
     */
    inline void operator*=(const GLGE_VEC_STRUCT_NAME& ot) noexcept {x *= ot.x; y *= ot.y; z *= ot.z;}

    /**
     * @brief divide the two vectors per element
     * 
     * @param ot a constant reference to the other vector
     */
    inline void operator/=(const GLGE_VEC_STRUCT_NAME& ot) noexcept {x /= ot.x; y /= ot.y; z /= ot.z;}

    /**
     * @brief access a specifc element of the vector by its index
     * @warning the index is not sanity-checked
     * 
     * @param idx the index of the element to access
     * @return GLGE_VEC_TYPE& a reference to the element stored at that position
     */
    inline GLGE_VEC_TYPE& operator[](uint8_t idx) noexcept {return *((&x) + idx);}

    /**
     * @brief get the length of the vector
     * 
     * @return float the length of the vector
     */
    inline float length() const noexcept {return sqrt(x*x + y*y + z*z);}

    /**
     * @brief get a vector that points in the same direction as this vector but has a length of 1
     * 
     * @return the vector with a length of 1
     */
    inline GLGE_VEC_STRUCT_NAME normalize() const noexcept {return *this / length();}

    /**
     * @brief set the length of this vector to always be 1
     */
    inline void normalizeThis() noexcept {*this /= length();}

    /**
     * @brief convert a vector into a string
     * 
     * @return the string storing the vector's data
     */
    inline std::string asString() const noexcept {return std::string(GLGE_STRINGIFY_DEFINE(GLGE_VEC_NAME))+ "{" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + "}";}

    #endif
    
    /**
     * @brief store the value for the x axis
     */
    GLGE_VEC_TYPE x;
    /**
     * @brief store the value for the y axis
     */
    GLGE_VEC_TYPE y;
    /**
     * @brief store the value for the z axis
     */
    GLGE_VEC_TYPE z;
} GLGE_VEC_NAME;

/**
 * @brief create a new 3D vector
 * 
 * @param x the value for the x axis
 * @param y the value for the y axis
 * @param z the value for the z axis
 */
GLGE_FUNCTION_INLINE GLGE_VEC_NAME GLGE_C_FUNC(GLGE_DEFINE_CONCATINATE(GLGE_VEC_NAME, _create))(GLGE_CONST_REF(GLGE_VEC_TYPE) x, GLGE_CONST_REF(GLGE_VEC_TYPE) y, GLGE_CONST_REF(GLGE_VEC_TYPE) z) GLGE_FUNC_NOEXCEPT
{
    //check if this is C++
    #if GLGE_CPP
    //use the C++ constructor 
    return GLGE_VEC_NAME(x, y, z);
    //else, this is C
    #else
    //use the C compound structure
    return (GLGE_VEC_NAME){x, y, z};
    #endif
}

/**
 * @brief create a new 3D vector
 * 
 * @param xyz the value for the x, y and z axis
 */
GLGE_FUNCTION_INLINE GLGE_VEC_NAME GLGE_C_FUNC(GLGE_DEFINE_CONCATINATE(GLGE_VEC_NAME, _createSingle))(GLGE_CONST_REF(GLGE_VEC_TYPE) xyz) GLGE_FUNC_NOEXCEPT
{
    //check if this is C++
    #if GLGE_CPP
    //use the C++ constructor 
    return GLGE_VEC_NAME(xyz);
    //else, this is C
    #else
    //use the C compound structure
    return (GLGE_VEC_NAME){xyz, xyz, xyz};
    #endif
}

/**
 * @brief convert a vector into a string
 * 
 * @warning the string is allocated with malloc. Make sure to free it if it is no longer needed
 * 
 * @param vec the vector to convert to a string
 * @return const char* the vector as a string
 */
const char* GLGE_C_FUNC(GLGE_DEFINE_CONCATINATE(GLGE_VEC_NAME, _asString))(GLGE_CONST_REF(GLGE_VEC_NAME) vec) GLGE_FUNC_NOEXCEPT;

/**
 * @brief add two 3D vectors together
 * 
 * @param a the first vector
 * @param b the second vector
 * 
 * @return the per-element sum of the two vectors
 */
GLGE_FUNCTION_INLINE GLGE_VEC_NAME GLGE_C_FUNC(GLGE_DEFINE_CONCATINATE(GLGE_VEC_NAME, _add))(GLGE_CONST_REF(GLGE_VEC_NAME) a, GLGE_CONST_REF(GLGE_VEC_NAME) b) GLGE_FUNC_NOEXCEPT
{
    //check if this is C++
    #if GLGE_CPP
    //use the overloaded operator
    return a + b;
    //else, this is C
    #else
    //add the elements and use the C compound structure
    return (GLGE_VEC_NAME){a.x+b.x, a.y+b.y, a.z+b.z};
    #endif
}

/**
 * @brief subtract a vector from another bector
 * 
 * @param a the first vector
 * @param b the second vector
 * 
 * @return the per-element difference of the first and the second vector
 */
GLGE_FUNCTION_INLINE GLGE_VEC_NAME GLGE_C_FUNC(GLGE_DEFINE_CONCATINATE(GLGE_VEC_NAME, _subtract))(GLGE_CONST_REF(GLGE_VEC_NAME) a, GLGE_CONST_REF(GLGE_VEC_NAME) b) GLGE_FUNC_NOEXCEPT
{
    //check if this is C++
    #if GLGE_CPP
    //use the overloaded operator
    return a - b;
    //else, this is C
    #else
    //subtract the elements and use the C compound structure
    return (GLGE_VEC_NAME){a.x-b.x, a.y-b.y, a.z-b.z};
    #endif
}

/**
 * @brief multiply two 3D vectors
 * 
 * @param a the first vector
 * @param b the second vector
 * 
 * @return the per-element product of the two vectors
 */
GLGE_FUNCTION_INLINE GLGE_VEC_NAME GLGE_C_FUNC(GLGE_DEFINE_CONCATINATE(GLGE_VEC_NAME, _multiply))(GLGE_CONST_REF(GLGE_VEC_NAME) a, GLGE_CONST_REF(GLGE_VEC_NAME) b) GLGE_FUNC_NOEXCEPT
{
    //check if this is C++
    #if GLGE_CPP
    //use the overloaded operator
    return a * b;
    //else, this is C
    #else
    //multiply the elements and use the C compound structure
    return (GLGE_VEC_NAME){a.x*b.x, a.y*b.y, a.z*b.z};
    #endif
}

/**
 * @brief divide two vectors per-element
 * 
 * @param a the nominator vector
 * @param b the denominator vector
 * 
 * @return the per-element fraction of the first and the second vector
 */
GLGE_FUNCTION_INLINE GLGE_VEC_NAME GLGE_C_FUNC(GLGE_DEFINE_CONCATINATE(GLGE_VEC_NAME, _divide))(GLGE_CONST_REF(GLGE_VEC_NAME) a, GLGE_CONST_REF(GLGE_VEC_NAME) b) GLGE_FUNC_NOEXCEPT
{
    //check if this is C++
    #if GLGE_CPP
    //use the overloaded operator
    return a / b;
    //else, this is C
    #else
    //subtract the elements and use the C compound structure
    return (GLGE_VEC_NAME){a.x / b.x, a.y / b.y, a.z / b.z};
    #endif
}

/**
 * @brief add two 3D vectors together
 * @warning inputting NULL as a resultrs in a segmentation fault
 * 
 * @param a the first vector
 * @param b the second vector
 */
GLGE_FUNCTION_INLINE void GLGE_C_FUNC(GLGE_DEFINE_CONCATINATE(GLGE_VEC_NAME, _addTo))(GLGE_VEC_NAME* a, GLGE_CONST_REF(GLGE_VEC_NAME) b) GLGE_FUNC_NOEXCEPT
{
    //check if this is C++
    #if GLGE_CPP
    //use the overloaded operator
    *a += b;
    //else, this is C
    #else
    //add to the a element
    a->x += b.x; a->y += b.y; a->z += b.z;
    #endif
}

/**
 * @brief subtract a vector from another vector
 * 
 * @param a the first vector to subtract from
 * @param b the second vector
 */
GLGE_FUNCTION_INLINE void GLGE_C_FUNC(GLGE_DEFINE_CONCATINATE(GLGE_VEC_NAME, _subtractFrom))(GLGE_VEC_NAME* a, GLGE_CONST_REF(GLGE_VEC_NAME) b) GLGE_FUNC_NOEXCEPT
{
    //check if this is C++
    #if GLGE_CPP
    //use the overloaded operator
    *a -= b;
    //else, this is C
    #else
    //subtract from the a element
    a->x -= b.x; a->y -= b.y; a->z -= b.z;
    #endif
}

/**
 * @brief multiply another vector to the a element
 * 
 * @param a the first vector to multiply to
 * @param b the second vector
 */
GLGE_FUNCTION_INLINE void GLGE_C_FUNC(GLGE_DEFINE_CONCATINATE(GLGE_VEC_NAME, _multiplyTo))(GLGE_VEC_NAME* a, GLGE_CONST_REF(GLGE_VEC_NAME) b) GLGE_FUNC_NOEXCEPT
{
    //check if this is C++
    #if GLGE_CPP
    //use the overloaded operator
    *a *= b;
    //else, this is C
    #else
    //multiply to the a element
    a->x *= b.x; a->y *= b.y; a->z *= b.z;
    #endif
}

/**
 * @brief divide two vectors per-element
 * 
 * @param a the nominator vector to divide from
 * @param b the denominator vector
 */
GLGE_FUNCTION_INLINE void GLGE_C_FUNC(GLGE_DEFINE_CONCATINATE(GLGE_VEC_NAME, _divideFrom))(GLGE_VEC_NAME* a, GLGE_CONST_REF(GLGE_VEC_NAME) b) GLGE_FUNC_NOEXCEPT
{
    //check if this is C++
    #if GLGE_CPP
    //use the overloaded operator
    *a /= b;
    //else, this is C
    #else
    //divide the elements from a
    a->x /= b.x; a->y /= b.y; a->z /= b.z;
    #endif
}

/**
 * @brief negate the elements of a vector
 * @warning using this funciton on unsigned vectors results in undefined behaviour
 * 
 * @param vec the vector to negate
 * @return the negatet vector
 */
GLGE_FUNCTION_INLINE GLGE_VEC_NAME GLGE_C_FUNC(GLGE_DEFINE_CONCATINATE(GLGE_VEC_NAME, _negate))(GLGE_CONST_REF(GLGE_VEC_NAME) vec) GLGE_FUNC_NOEXCEPT
{
    //check if this is C++
    #if GLGE_CPP
    //use the overloaded operator
    return -vec;
    //else, this is C
    #else
    //negate per element and use the C compound structure
    return (GLGE_VEC_NAME){-vec.x, -vec.y, -vec.z};
    #endif
}

/**
 * @brief negate all the elements of a vector
 * @warning parsing NULL as a pointer results in undefined behaviour
 * @warning using this funciton on unsigned vectors results in undefined behaviour
 * 
 * @param vec a pointer of the vector to negate
 */
GLGE_FUNCTION_INLINE void GLGE_C_FUNC(GLGE_DEFINE_CONCATINATE(GLGE_VEC_NAME, _negateThis))(GLGE_VEC_NAME* vec) GLGE_FUNC_NOEXCEPT
{
    //negate all the elements in the vector
    vec->x = -vec->x; vec->y = -vec->y; vec->z = -vec->z;
}

/**
 * @brief compute the dot product of two vectors
 * 
 * @param a the first vector
 * @param b the second vector
 * 
 * @return the dot product of the two vectors
 */
GLGE_FUNCTION_INLINE GLGE_VEC_TYPE GLGE_C_FUNC(GLGE_DEFINE_CONCATINATE(GLGE_VEC_NAME, _dot))(GLGE_CONST_REF(GLGE_VEC_NAME) a, GLGE_CONST_REF(GLGE_VEC_NAME) b) GLGE_FUNC_NOEXCEPT
{
    //return the dot product
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

/**
 * @brief compute the cross product of two vectors
 * 
 * @param a the first vector
 * @param b the second vector
 * 
 * @return the cross product of the two vectors
 */
GLGE_FUNCTION_INLINE GLGE_VEC_NAME GLGE_C_FUNC(GLGE_DEFINE_CONCATINATE(GLGE_VEC_NAME, _cross))(GLGE_CONST_REF(GLGE_VEC_NAME) a, GLGE_CONST_REF(GLGE_VEC_NAME) b) GLGE_FUNC_NOEXCEPT
{
    //check if this is C++
    #if GLGE_CPP
    //use the overloaded operator
    return GLGE_VEC_NAME(a.y*b.z - a.z*b.y, a.x*b.z - a.z*b.x, a.x*b.y - a.y*b.x);
    //else, this is C
    #else
    //create the cross product and use the C compound structure
    return (GLGE_VEC_NAME){a.y*b.z - a.z*b.y, a.x*b.z - a.z*b.x, a.x*b.y - a.y*b.x};
    #endif
}

/**
 * @brief calculate the length of the vector
 * 
 * @param v the vector to calculate the length from
 * 
 * @return the length of the vector
 */
GLGE_FUNCTION_INLINE float GLGE_C_FUNC(GLGE_DEFINE_CONCATINATE(GLGE_VEC_NAME, _length))(GLGE_CONST_REF(GLGE_VEC_NAME) v)
{
    //return the length of the vector
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

//stop the C-Section
#if GLGE_CPP
}
#endif

//clean up: undefine the defines
#undef GLGE_VEC_TYPE
#undef GLGE_VEC_NAME
#undef GLGE_VEC_STRUCT_NAME

#endif //error check for vector struct name
#endif //error check for vector name
#endif //error check for vector type