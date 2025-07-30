/**
 * @file GLGEMat3Preset.h
 * @author DM8AT
 * @brief define the preset for a 3x3 matrix
 * @version 0.1
 * @date 2025-07-19
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//inclue the utility functions
#include "../../../GLGECore/GLGEUtil.h"
//include the settings
#include "../../../GLGECore/GLGESettings.h"

//check if all defines are set correctly
//check if the type of the matrix is set
#ifndef GLGE_MAT_TYPE
//check if compiler errors are enabled
#if GLGE_COMPILER_ERRORS
//print an error
#error [ERROR] Failed to create matrix: the type of the matrix that should be created was not set
#endif //error
#else
//check if the name of the matrix is set
#ifndef GLGE_MAT_NAME
//check if compiler errors are enabled
#if GLGE_COMPILER_ERRORS
//print an error
#error [ERROR] Failed to create matrix: the name of the matrix that should be created was not set
#endif //error
#else
//check if the name of the struct of the matrix is set
#ifndef GLGE_MAT_STRUCT_NAME
//check if compiler errors are enabled
#if GLGE_COMPILER_ERRORS
//print an error
#error [ERROR] Failed to create matrix: the name of the struct of the matrix that should be created was not set
#endif //error
#else
//check if the vector that corresponds to the matrix is defined
#ifndef GLGE_MAT_VEC_TYPE
//check if compiler errors are enabled
#if GLGE_COMPILER_ERRORS
//print an error
#error [ERROR] Failed to create matrix: the vector type belonging to the matrix was not set
#endif //error
#else

/**
 * @brief store the 3x33 matrix
 */
typedef struct GLGE_MAT_STRUCT_NAME {

    /**
     * @brief store the rows of the vector
     */
    GLGE_MAT_VEC_TYPE m[3]
    //check for C++ to define a default
    #if GLGE_CPP
     = {GLGE_MAT_VEC_TYPE(1,0,0), GLGE_MAT_VEC_TYPE(0,1,0), GLGE_MAT_VEC_TYPE(0,0,1)}
    #endif //end of C++ section
    ;

    //check for C++ to add functions
    #if GLGE_CPP

    /**
     * @brief Construct a new 3x3 matrix
     * the instance will be initalized to the identity matrix
     */
    GLGE_MAT_STRUCT_NAME() = default;

    /**
     * @brief Construct a new 3x3 matrix
     * 
     * @param m00 the top left element of the matrix
     * @param m01 the top middle element of the matrix
     * @param m02 the top right element of the matrix
     * @param m10 the middle left element of the matrix
     * @param m11 the middle element of the matrix
     * @param m12 the middle right element of the matrix
     * @param m20 the bottom left element of the matrix
     * @param m21 the bottom middle element of the matrix
     * @param m22 the bottom right element of the matrix
     */
    GLGE_MAT_STRUCT_NAME(GLGE_MAT_TYPE m00, GLGE_MAT_TYPE m01, GLGE_MAT_TYPE m02, 
                         GLGE_MAT_TYPE m10, GLGE_MAT_TYPE m11, GLGE_MAT_TYPE m12, 
                         GLGE_MAT_TYPE m20, GLGE_MAT_TYPE m21, GLGE_MAT_TYPE m22)
    {
        //store the elements as vectors
        m[0] = GLGE_MAT_VEC_TYPE(m00, m01, m02);
        m[1] = GLGE_MAT_VEC_TYPE(m10, m11, m12);
        m[2] = GLGE_MAT_VEC_TYPE(m20, m21, m22);
    }

    /**
     * @brief Construct a new 3x3 matrix
     * 
     * @param row0 the top row of the matrix
     * @param row1 the middle row of the matrix
     * @param row2 the bottom row of the matrix
     */
    GLGE_MAT_STRUCT_NAME(GLGE_MAT_VEC_TYPE row0, GLGE_MAT_VEC_TYPE row1, GLGE_MAT_VEC_TYPE row2)
    {
        //store the rows of the matrix
        m[0] = row0;
        m[1] = row1;
        m[2] = row2;
    }

    /**
     * @brief Construct a new 3x3 matrix
     * @warning the array is not sanity-checked
     * 
     * @param array an array of data to load the matrix from. Must be 9 elements long. 
     */
    GLGE_MAT_STRUCT_NAME(GLGE_MAT_TYPE* array)
    {
        //store all the elements of the array
        m[0] = GLGE_MAT_VEC_TYPE(array[0], array[1], array[2]);
        m[1] = GLGE_MAT_VEC_TYPE(array[3], array[4], array[5]);
        m[2] = GLGE_MAT_VEC_TYPE(array[6], array[7], array[8]);
    }

    /**
     * @brief combine two matricies by adding them element-wise
     * 
     * @param other the other matrix to add to this one
     * @return GLGE_MAT_STRUCT_NAME the element wise sum of all the element of the matrix
     */
    inline GLGE_MAT_STRUCT_NAME operator+(const GLGE_MAT_STRUCT_NAME& other) const noexcept
    {
        //element wise addition on the matrix
        return GLGE_MAT_STRUCT_NAME(
            m[0].x + other.m[0].x, m[0].y + other.m[0].y, m[0].z + other.m[0].z,
            m[1].x + other.m[1].x, m[1].y + other.m[1].y, m[1].z + other.m[1].z,
            m[2].x + other.m[2].x, m[2].y + other.m[2].y, m[2].z + other.m[2].z
        );
    }

    /**
     * @brief add a scalar type to the matrix. This will increase all elements by the specified value. 
     * 
     * @param scalar the amount to increment all elements by
     * @return GLGE_MAT_STRUCT_NAME the matrix where all elements are incremented by the amount spezified by the scalar
     */
    inline GLGE_MAT_STRUCT_NAME operator+(GLGE_MAT_TYPE scalar) const noexcept
    {
        //add the scalar to all elements
        return GLGE_MAT_STRUCT_NAME(
            m[0].x + scalar, m[0].y + scalar, m[0].z + scalar,
            m[1].x + scalar, m[1].y + scalar, m[1].z + scalar,
            m[2].x + scalar, m[2].y + scalar, m[2].z + scalar
        );
    }

    /**
     * @brief combine two matrices by computing the element wise difference
     * 
     * @param other the matrix to subtract from this matrix element-wise
     * @return GLGE_MAT_STRUCT_NAME the matrix with the element wise difference computed
     */
    inline GLGE_MAT_STRUCT_NAME operator-(const GLGE_MAT_STRUCT_NAME& other) const noexcept
    {
        //element wise subtraction on the matrix
        return GLGE_MAT_STRUCT_NAME(
            m[0].x - other.m[0].x, m[0].y - other.m[0].y, m[0].z - other.m[0].z,
            m[1].x - other.m[1].x, m[1].y - other.m[1].y, m[1].z - other.m[1].z,
            m[2].x - other.m[2].x, m[2].y - other.m[2].y, m[2].z - other.m[2].z
        );
    }

    /**
     * @brief decrement all the elements in the matrix by a spezified scalar
     * 
     * @param scalar the amount to decrement all elements in the matrix by
     * @return GLGE_MAT_STRUCT_NAME the matrix where all elements are decremented by the spezified amount
     */
    inline GLGE_MAT_STRUCT_NAME operator-(GLGE_MAT_TYPE scalar) const noexcept
    {
        //subtract the scalar from all elements
        return GLGE_MAT_STRUCT_NAME(
            m[0].x - scalar, m[0].y - scalar, m[0].z - scalar,
            m[1].x - scalar, m[1].y - scalar, m[1].z - scalar,
            m[2].x - scalar, m[2].y - scalar, m[2].z - scalar
        );
    }

    /**
     * @brief invert all the elements in the matrix
     * 
     * @return GLGE_MAT_STRUCT_NAME the matrix with all elements inverted
     */
    inline GLGE_MAT_STRUCT_NAME operator-() const noexcept
    {
        //negate all elements
        return GLGE_MAT_STRUCT_NAME(
            -m[0].x, -m[0].y, -m[0].z,
            -m[1].x, -m[1].y, -m[1].z,
            -m[2].x, -m[2].y, -m[2].z
        );
    }

    /**
     * @brief multiply two matrices together
     * @warning in contrast to vector multiplication, this is done with correct matrix multiplication and not element-wise
     * 
     * @param other the other matrix to multiply with
     * @return GLGE_MAT_STRUCT_NAME the product of the two matricies
     */
    inline GLGE_MAT_STRUCT_NAME operator*(const GLGE_MAT_STRUCT_NAME& other) const noexcept
    {
        //return the multiplied matrix
        return GLGE_MAT_STRUCT_NAME(
            m[0].x * other.m[0].x + m[0].y * other.m[1].x + m[0].z * other.m[2].x, m[0].x * other.m[0].y + m[0].y * other.m[1].y + m[0].z * other.m[2].y, m[0].x * other.m[0].z + m[0].y * other.m[1].z + m[0].z * other.m[2].z, 
            m[1].x * other.m[0].x + m[1].y * other.m[1].x + m[1].z * other.m[2].x, m[1].x * other.m[0].y + m[1].y * other.m[1].y + m[1].z * other.m[2].y, m[1].x * other.m[0].z + m[1].y * other.m[1].z + m[1].z * other.m[2].z, 
            m[2].x * other.m[0].x + m[2].y * other.m[1].x + m[2].z * other.m[2].x, m[2].x * other.m[0].y + m[2].y * other.m[1].y + m[2].z * other.m[2].y, m[2].x * other.m[0].z + m[2].y * other.m[1].z + m[2].z * other.m[2].z
        );
    }

    /**
     * @brief scale the matrix by a scalar type
     * 
     * @param scalar the scalar to scale all elements in the matrix by
     * @return GLGE_MAT_STRUCT_NAME the scaled matrix
     */
    inline GLGE_MAT_STRUCT_NAME operator*(GLGE_MAT_TYPE scalar) const noexcept
    {
        //multiply all element by the scalar
        return GLGE_MAT_STRUCT_NAME(
            m[0].x * scalar, m[0].y * scalar, m[0].z * scalar,
            m[1].x * scalar, m[1].y * scalar, m[1].z * scalar,
            m[2].x * scalar, m[2].y * scalar, m[2].z * scalar
        );
    }

    /**
     * @brief multiply a vector and a matrix. This will transform the vector according to the matrix
     * 
     * @param vector the vector to multiply with
     * @return GLGE_MAT_VEC_TYPE the vector transformed by the matrix
     */
    inline GLGE_MAT_VEC_TYPE operator*(const GLGE_MAT_VEC_TYPE& vector) const noexcept
    {
        //return the vector
        return GLGE_MAT_VEC_TYPE(
            vector.x * m[0].x + vector.y * m[0].y + vector.z * m[0].z,
            vector.x * m[1].x + vector.y * m[1].y + vector.z * m[1].z,
            vector.x * m[2].x + vector.y * m[2].y + vector.z * m[2].z
        );
    }

    /**
     * @brief divides the matrix by a scalar. This is the counter opperation to multiplication with a scalar. 
     * 
     * @param scalar the scalar to divide the matrix by
     * @return GLGE_MAT_STRUCT_NAME the divided matrix
     */
    inline GLGE_MAT_STRUCT_NAME operator/(GLGE_MAT_TYPE scalar) const noexcept {return *this * (GLGE_MAT_TYPE)(((GLGE_MAT_TYPE)1.) / scalar);}

    /**
     * @brief add a matrix to this matrix
     * 
     * @param other the matrix to add to this matrix
     */
    void operator+=(const GLGE_MAT_STRUCT_NAME& other) noexcept
    {
        //increment all elements by the according other element
        m[0] += other.m[0];
        m[1] += other.m[1];
        m[2] += other.m[2];
    }

    /**
     * @brief increment all elements of this matrix by a scalar
     * 
     * @param scalar the scalar to increment all elements with
     */
    void operator+=(GLGE_MAT_TYPE scalar) noexcept
    {
        //increment all elements by the scalar
        m[0] += scalar;
        m[1] += scalar;
        m[2] += scalar;
    }

    /**
     * @brief subtract a matrix from this matrix
     * 
     * @param other the matrix to subtract from this matrix
     */
    void operator-=(const GLGE_MAT_STRUCT_NAME& other) noexcept
    {
        //decrement the elements by the corresponding elements from the other vector
        m[0] -= other.m[0];
        m[1] -= other.m[1];
        m[2] -= other.m[2];
    }

    /**
     * @brief decrement all elements of the matrix by a scalar
     * 
     * @param scalar the scalar to decrement all elements by
     */
    void operator-=(GLGE_MAT_TYPE scalar) noexcept
    {
        //decrement all the elements by the scalar
        m[0] -= scalar;
        m[1] -= scalar;
        m[2] -= scalar;
    }

    /**
     * @brief multiply a matrix to this matrix. The product will be stored in this one. 
     * 
     * @param other the other matrix to multiply to this one
     */
    inline void operator*=(const GLGE_MAT_STRUCT_NAME& other) noexcept { *this = *this * other; }

    /**
     * @brief scale this matrix by a scalar
     * 
     * @param scalar the scalar to scale the whole matrix by
     */
    void operator*=(GLGE_MAT_TYPE scalar) noexcept
    {
        //scale all elements by the scalar
        m[0] *= scalar;
        m[1] *= scalar;
        m[2] *= scalar;
    }

    /**
     * @brief divide the matrix by a scalar
     * 
     * @param scalar the scalar to divide the matrix by
     */
    void operator/=(GLGE_MAT_TYPE scalar) noexcept {*this *= (GLGE_MAT_TYPE)(((GLGE_MAT_TYPE)1.) / scalar);}

    /**
     * @brief check if two matricies are the same
     * 
     * @param other the other matrix to check with
     * @return true : the matricies are exactly the same
     * @return false : the matricies are not the same
     */
    inline bool operator==(const GLGE_MAT_STRUCT_NAME& other) const noexcept
    {
        //check if all elements match exactly
        return (
            (m[0].x == other.m[0].x) && (m[0].y == other.m[0].y) && (m[0].z == other.m[0].z) &&
            (m[1].x == other.m[1].x) && (m[1].y == other.m[1].y) && (m[1].z == other.m[1].z) &&
            (m[2].x == other.m[2].x) && (m[2].y == other.m[2].y) && (m[2].z == other.m[2].z)
        );
    }

    /**
     * @brief check if two matricies are not the same
     * 
     * @param other the other matrix to check with
     * @return true : the matricies are not the same
     * @return false : the matricies are the same
     */
    inline bool operator!=(const GLGE_MAT_STRUCT_NAME& other) const noexcept {return !(*this == other);}

    /**
     * @brief access a specific row of the matrix
     * @warning the index of the row is not sanity-checked
     * 
     * @param idx the index of the row to access
     * @return GLGE_MAT_VEC_TYPE& a reference to the vector representing the requested row
     */
    inline GLGE_MAT_VEC_TYPE& operator[](uint8_t idx) noexcept {return m[idx];}

    /**
     * @brief compute the determinant of the matrix
     * 
     * @return double the determinant of the matrix
     */
    inline double determinant() const noexcept {return m[0].x * (m[1].y * m[2].z - m[1].z * m[2].y) - m[0].y * (m[1].x * m[2].z - m[1].z * m[2].x) + m[0].z * (m[1].x * m[2].y - m[1].y * m[2].x);}

    /**
     * @brief check if the matrix is invertable
     * the matrix is invertiable if the determinant is not zero
     * 
     * @return true : the matrix has an inverse
     * @return false : the matrix has no inverse
     */
    inline bool hasInverse() const noexcept {return determinant() != 0;}

    /**
     * @brief compute the cofactor matrix of the 3x3 matrix
     * 
     * @return GLGE_MAT_STRUCT_NAME the cofactor matrix of the 3x3 matrix
     */
    inline GLGE_MAT_STRUCT_NAME cofactors() const noexcept
    {
        /**
         * A = {
         *  {a,b,c},
         *  {d,e,f},
         *  {g,h,i}
         * }
         * 
         * cof(A) = {
         *  {  ei-fh, -(di-fg),  dh-eg},
         *  {-(bi-ch),  ai-cg, -(ah-bg)},
         *  {  bf-ce, -(af-cd),  ae-bd}
         * }
         * 
         * Made to fit row-column access of the matrix
         * cof(a) = {
         *  {  1y * 2z - 1z * 2y, -(1x * 2z - 1z * 2x),  1x * 2y - 1y * 2x},
         *  {-(0y * 2z - 0z * 2y),  0x * 2z - 0z * 2x, -(0x * 2y - 0y * 2x)},
         *  {  0y * 1z - 0z * 1y, -(0x * 1z - 0z * 1x),  0x * 1y - 0y * 1x},
         * }
         */
        return GLGE_MAT_STRUCT_NAME(
              m[1].y * m[2].z - m[1].z * m[2].y, -(m[1].x * m[2].z - m[1].z * m[2].x),  m[1].x * m[2].y - m[1].y * m[2].x,
            -(m[0].y * m[2].z - m[0].z * m[2].y),  m[0].x * m[2].z - m[0].z * m[2].x, -(m[0].x * m[2].y - m[0].y * m[2].x),
              m[0].y * m[1].z - m[0].z * m[1].y, -(m[0].x * m[1].z - m[0].z * m[1].x),  m[0].x * m[1].y - m[0].y * m[1].x
        );
    }

    /**
     * @brief set this matrix to its own cofactor matrix
     */
    inline void cofactorThis() noexcept {*this = cofactors();}

    /**
     * @brief compute the adjoint matrix of this matrix
     * Simply the transpose of the cofactor matrix
     * 
     * Faster because it is directly computed, not cofactors().transpose()
     * 
     * @return GLGE_MAT_STRUCT_NAME the adjoint matrix
     */
    inline GLGE_MAT_STRUCT_NAME adjoint() const noexcept
    {
        //same as the cofactor computation, but rows and columns are swapped
        return GLGE_MAT_STRUCT_NAME(
              m[1].y * m[2].z - m[1].z * m[2].y, -(m[0].y * m[2].z - m[0].z * m[2].y),  m[0].y * m[1].z - m[0].z * m[1].y,
            -(m[1].x * m[2].z - m[1].z * m[2].x),  m[0].x * m[2].z - m[0].z * m[2].x, -(m[0].x * m[1].z - m[0].z * m[1].x),
              m[1].x * m[2].y - m[1].y * m[2].x, -(m[0].x * m[2].y - m[0].y * m[2].x),  m[0].x * m[1].y - m[0].y * m[1].x
        );
    }

    /**
     * @brief set this matrix to it's own adjoint
     */
    inline void adjointThis() noexcept
    {
        //set the matrix to its adjoint
        *this = adjoint();
    }

    /**
     * @brief compute the inverse matrix
     * 
     * @return GLGE_MAT_STRUCT_NAME the inverted matrix
     */
    inline GLGE_MAT_STRUCT_NAME inverse() const noexcept {return adjoint() / (GLGE_MAT_TYPE)determinant();}

    /**
     * @brief set this to its own inverse matrix
     */
    inline void inverseThis() noexcept {*this = inverse();}

    /**
     * @brief morror the matrix on its diagonal from top left to bottom right
     * 
     * @return GLGE_MAT_VEC_TYPE the transposed matrix
     */
    inline GLGE_MAT_STRUCT_NAME transpose() const noexcept
    {
        //return the transposed matrix
        return GLGE_MAT_STRUCT_NAME(
            m[0].x, m[1].x, m[2].x,
            m[0].y, m[1].y, m[2].y,
            m[0].z, m[1].z, m[2].z
        );
    }

    /**
     * @brief set this matrix to its own transpose (for more info see the transpose function)
     */
    inline void transposeThis() noexcept
    {
        //swap the order around
        *this = transpose();
    }

    #endif //end of C++ section

} GLGE_MAT_NAME;

//check for C++ to start a C section
#if GLGE_CPP
extern "C" {
#endif //end of C++ section, start of C section

//check for C++ to end a C section
#if GLGE_CPP
}

/**
 * @brief print a 3x3 matrix into a output stream
 * 
 * @param os the output stream to fill with the matrix
 * @param mat the matrix to print to the output stream
 * @return std::ostream& the filled output stream
 */
inline std::ostream& operator<<(std::ostream& os, const GLGE_MAT_NAME& mat)
{return os << GLGE_STRINGIFY_DEFINE(GLGE_MAT_NAME) << "{{" << mat.m[0].x << ", " << mat.m[0].y << ", " << mat.m[0].z << "}, {" 
                                                           << mat.m[1].x << ", " << mat.m[1].y << ", " << mat.m[1].z << "}, {"
                                                           << mat.m[2].x << ", " << mat.m[2].y << ", " << mat.m[2].z << "}}";}

#endif //end of C++ section and end of C section

//undefine the vector type
#undef GLGE_MAT_VEC_TYPE
#endif
//undefine the structure name
#undef GLGE_MAT_STRUCT_NAME
#endif
//undefine the matrix name
#undef GLGE_MAT_NAME
#endif
//undefine the type
#undef GLGE_MAT_TYPE
#endif //end of definition