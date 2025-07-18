/**
 * @file GLGEMat2Preset.h
 * @author DM8AT
 * @brief a preset for a 2x2 matrix for GLGE. Based on the 2D matrix preset
 * @version 0.1
 * @date 2025-07-17
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

//check for C++ to start a C-Section
#if GLGE_CPP
extern "C" {
#endif

/**
 * @brief store the actual matrix
 */
typedef struct GLGE_MAT_STRUCT_NAME 
{

    /**
     * @brief store all the elements of the matrix
     */
    GLGE_MAT_VEC_TYPE m [2]
    //check for C++ to set a default
    #if GLGE_CPP
     = {GLGE_MAT_VEC_TYPE(1,0), GLGE_MAT_VEC_TYPE(0,1)}
    #endif //end of C++ section
    ;

    //check for C++ to create attribute functions
    #if GLGE_CPP

    /**
     * @brief Construct a new matrix structure
     * this will be initalized as a identity matrix
     */
    GLGE_MAT_STRUCT_NAME() = default;

    /**
     * @brief Construct a new matrix structure object
     * 
     * @param m00 the top left element of the matrix
     * @param m01 the top right element of the matrix
     * @param m10 the bottom left element of the matrix
     * @param m11 the bottom right element of the matrix
     */
    GLGE_MAT_STRUCT_NAME(GLGE_MAT_TYPE m00, GLGE_MAT_TYPE m01, GLGE_MAT_TYPE m10, GLGE_MAT_TYPE m11)
    {
        //just store the values directly
        m[0] = GLGE_MAT_VEC_TYPE(m00, m01);
        m[1] = GLGE_MAT_VEC_TYPE(m10, m11);
    }

    /**
     * @brief Construct a new matrix structure object
     * @warning the matrix must be defined in a row-major order
     * 
     * @param array an array of the matricie's type. The size must match the size of the matrix (in this case 4 elements)
     */
    GLGE_MAT_STRUCT_NAME(GLGE_MAT_TYPE* array)
    {
        //map the values over
        m[0] = GLGE_MAT_VEC_TYPE(array[0], array[1]);
        m[1] = GLGE_MAT_VEC_TYPE(array[2], array[3]);
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
            m[0].x + other.m[0].x,
            m[0].y + other.m[0].y,
            m[1].x + other.m[1].x,
            m[1].y + other.m[1].y
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
            m[0].x + scalar,
            m[0].y + scalar,
            m[1].x + scalar,
            m[1].y + scalar
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
            m[0].x - other.m[0].x,
            m[0].y - other.m[0].y,
            m[1].x - other.m[1].x,
            m[1].y - other.m[1].y
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
            m[0].x - scalar,
            m[0].y - scalar,
            m[1].x - scalar,
            m[1].y - scalar
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
            -m[0].x,
            -m[0].y,
            -m[1].x,
            -m[1].y
        );
    }

    /**
     * @brief multiply two matrices together
     * @warning in contrast to matrix multiplication, this is done with correct matrix multiplication and not element-wise
     * 
     * @param other the other matrix to multiply with
     * @return GLGE_MAT_STRUCT_NAME the product of the two matricies
     */
    inline GLGE_MAT_STRUCT_NAME operator*(const GLGE_MAT_STRUCT_NAME& other) const noexcept
    {
        //return the multiplied matrix
        return GLGE_MAT_STRUCT_NAME(
            m[0].x * other.m[0].x + m[0].y * other.m[1].x,
            m[0].x * other.m[0].y + m[0].y * other.m[1].y,
            m[1].x * other.m[0].x + m[1].y * other.m[1].x,
            m[1].x * other.m[0].y + m[1].y * other.m[1].y
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
            m[0].x * scalar,
            m[0].y * scalar,
            m[1].x * scalar,
            m[1].y * scalar
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
            m[0].x * vector.x + m[0].y * vector.y,
            m[1].x * vector.x + m[1].y * vector.y
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
            (m[0].x == other.m[0].x) &&
            (m[0].y == other.m[0].y) &&
            (m[1].x == other.m[1].x) &&
            (m[1].y == other.m[1].y)
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
    inline double determinant() const noexcept {return (m[0].x*m[1].y - m[0].y*m[1].x);}

    /**
     * @brief compute the inverse matrix
     * 
     * @return GLGE_MAT_STRUCT_NAME the inverted matrix
     */
    inline GLGE_MAT_STRUCT_NAME inverse() const noexcept {return *this * (GLGE_MAT_TYPE)determinant();}

    /**
     * @brief set this to its own inverse matrix
     */
    inline void inverseThis() noexcept {*this *= determinant();}

    /**
     * @brief morror the matrix on its diagonal from top left to bottom right
     * 
     * @return GLGE_MAT_VEC_TYPE the transposed matrix
     */
    inline GLGE_MAT_STRUCT_NAME transpose() const noexcept
    {
        //return the transposed matrix
        return GLGE_MAT_STRUCT_NAME(
            m[0].x, m[1].x,
            m[0].y, m[1].y
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

/**
 * @brief create a simple identity matrix. Usefull for initalization. 
 * 
 * @return GLGE_MAT_NAME a identity matrix of the spezified type
 */
GLGE_MAT_NAME GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _identity))();

/**
 * @brief add two matricies together
 * 
 * @param a the first matrix
 * @param b the second matrix
 * 
 * @return GLGE_MAT_NAME the sum of the matricies
 */
GLGE_MAT_NAME GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _add))(GLGE_MAT_NAME a, GLGE_MAT_NAME b);

/**
 * @brief add a scalar to all elements of the matrix
 * 
 * @param a the matrix to increment
 * @param s the scalar to add to all matrix elements
 * 
 * @return GLGE_MAT_NAME the sum of the scalar and the matrix
 */
GLGE_MAT_NAME GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _addScalar))(GLGE_MAT_NAME a, GLGE_MAT_TYPE s);

/**
 * @brief subtract two matricies from each other
 * 
 * @param a the first matrix
 * @param b the matrix to subtract from the first matrix
 * 
 * @return GLGE_MAT_NAME the difference of the matricies
 */
GLGE_MAT_NAME GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _subtract))(GLGE_MAT_NAME a, GLGE_MAT_NAME b);

/**
 * @brief subtract a scalar from a matrix
 * 
 * @param a the matrix to subtract the scalar from
 * @param s the scalar to subtract from the matrix
 * 
 * @return GLGE_MAT_NAME the matrix where all the elements are decremented by the scalar
 */
GLGE_MAT_NAME GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _subtractScalar))(GLGE_MAT_NAME a, GLGE_MAT_TYPE s);

/**
 * @brief inverte all the signes of a elements in a matrix
 * 
 * @param a the matrix to invert all elements of
 * 
 * @return GLGE_MAT_NAME the negated matrix
 */
GLGE_MAT_NAME GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _negate))(GLGE_MAT_NAME a);

/**
 * @brief multiply two matricies together with the correct matrix multiplication
 * @warning order matters
 * 
 * @param a the first matrix
 * @param b the second matrix
 * 
 * @return GLGE_MAT_NAME the product of the two matricies
 */
GLGE_MAT_NAME GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _multiply))(GLGE_MAT_NAME a, GLGE_MAT_NAME b);

/**
 * @brief scale all elements of a matrix by a scalar
 * 
 * @param a the matrix to scale all elements of
 * @param s the scalar to scale the elements by
 * 
 * @return GLGE_MAT_NAME the matrix where all elements are scaled by the scalar
 */
GLGE_MAT_NAME GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _multiplyScalar))(GLGE_MAT_NAME a, GLGE_MAT_TYPE s);

/**
 * @brief multiply a vector by a matrix
 * 
 * @param a the matrix to multiply the vector with
 * @param v the matrix to transform according to the matrix
 * 
 * @return GLGE_MAT_VEC_TYPE the vector transformed according to the matrix
 */
GLGE_MAT_VEC_TYPE GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _multiplyVector))(GLGE_MAT_NAME a, GLGE_MAT_VEC_TYPE v);

/**
 * @brief divide a matrix by a scalar
 * 
 * @param a the matrix to divide by the scalar
 * @param s the scalar to divide all elements of the matrix by
 * 
 * @return GLGE_MAT_NAME the matrix where all the elements are divided by the scalar
 */
GLGE_MAT_NAME GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _divideScalar))(GLGE_MAT_NAME a, GLGE_MAT_TYPE s);

/**
 * @brief add a matrix to another matrix and store the result in the first matrix
 * 
 * @param a a pointer to the first matrix and the matrix to store the result into
 * @param b the matrix to add to the first matrix
 */
void GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _addTo))(GLGE_MAT_NAME* a, GLGE_MAT_NAME b);

/**
 * @brief add a scalar to a matrix and store the resulting matrix in the matrix
 * 
 * @param a a pointer to the matrix to increment and to store the result into
 * @param s the scalar to increment the matrix by
 */
void GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _addScalarTo))(GLGE_MAT_NAME* a, GLGE_MAT_TYPE s);

/**
 * @brief subtract a matrix from another matrix and store the result in the first matrix
 * 
 * @param a a pointer to the first matrix and to store the resulting matrix into
 * @param b the matrix to subtract from the first matrix
 */
void GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _subtractFrom))(GLGE_MAT_NAME* a, GLGE_MAT_NAME b);

/**
 * @brief subtract a scalar from a matrix and store the resulting matrix in the matrix
 * 
 * @param a a pointer to the matrix to subtract the scalar from and to store the result into
 * @param s the scalar to decrement all elements of the matrix by
 */
void GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _subtractScalarFrom))(GLGE_MAT_NAME* a, GLGE_MAT_TYPE s);

/**
 * @brief invert the sign of all elements of a matrix and store the resulting matrix in the matrix
 * 
 * @param a a pointer to the matrix to negate and to store the resulting matrix into
 */
void GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _negateThis))(GLGE_MAT_NAME* a);

/**
 * @brief multiply a matrix by another matrix and store the resulting matrix in the first matrix
 * 
 * @param a a pointer to the first matrix to multiply and the matrix to store the resulting matrix into
 * @param b the matrix to multiply with the first matrix
 */
void GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _multiplyTo))(GLGE_MAT_NAME* a, GLGE_MAT_NAME b);

/**
 * @brief scale all elements of a matrix by a scalar and store the resulting matrix in the matrix
 * 
 * @param a a pointer to the matrix to scale and to store the resulting matrix into
 * @param s the scalar to scale the matrix by
 */
void GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _multiplyScalarTo))(GLGE_MAT_NAME* a, GLGE_MAT_TYPE s);

/**
 * @brief divide a matrix by a scalar and store the resulting matrix in the matrix
 * 
 * @param a a pointer to the matrix to divide and to store the resulting matrix into
 * @param s the scalar to divide the matrix by
 */
void GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _divideScalarFrom))(GLGE_MAT_NAME* a, GLGE_MAT_TYPE s);

/**
 * @brief compute the determinant of a matrix
 * 
 * @param mat the matrix to compute the determinant of
 * 
 * @return double the determinant of the matrix
 */
double GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _determinant))(GLGE_MAT_NAME mat);

/**
 * @brief calculate the inverse matrix of a matrix
 * 
 * @param mat the matrix to compute the invers eof
 * 
 * @return GLGE_MAT_NAME the inverse matrix of the inputed matrix
 */
GLGE_MAT_NAME GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _invert))(GLGE_MAT_NAME mat);

/**
 * @brief invert a matrix and store the inverse in the matrix
 * 
 * @param mat a pointer to the matrix to invert and to store the resulting inverse matrix into
 */
void GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _invertThis))(GLGE_MAT_NAME* mat);

/**
 * @brief calculate the transpose of a matrix
 * 
 * @param mat the matrix to compute the transpose of
 * 
 * @return GLGE_MAT_NAME the transpose of the inputed matrix
 */
GLGE_MAT_NAME GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _transpose))(GLGE_MAT_NAME mat);

/**
 * @brief calcualte the transpose of a matrix and store the resulting matrix in the inputed matrix
 * 
 * @param mat a pointer to the matrix to transpose and to store the matrix into
 */
void GLGE_C_FUNC(GLGE_CONCATINATE_DEFINE(GLGE_MAT_NAME, _transposeThis))(GLGE_MAT_NAME* mat);

//check for C++ to end a C-Section
#if GLGE_CPP
} //end of C-Section

/**
 * @brief print a 2x2 matrix into an output stream
 * 
 * @param os the output stream to print to
 * @param mat a constant reference to the matrix to print
 * @return std::ostream& the filled output stream
 */
inline std::ostream& operator<<(std::ostream& os, const GLGE_MAT_STRUCT_NAME& mat) noexcept
{
    //print all elements to the output stream with a bit of formatting
    return os << GLGE_STRINGIFY_DEFINE(GLGE_MAT_NAME) << "{{" << mat.m[0].x << ", " << mat.m[0].y << "}, {" << mat.m[1].x << ", " << mat.m[1].y << "}}";
}

#endif

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