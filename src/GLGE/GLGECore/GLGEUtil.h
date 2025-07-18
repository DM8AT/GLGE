/**
 * @file GLGEUtil.h
 * @author DM8AT
 * @brief simple utilitys for GLGE
 * @version 0.1
 * @date 2025-04-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_UTIL_
#define _GLGE_UTIL_

//include the settings
#include "GLGESettings.h"

//check if this is C++
#if GLGE_CPP
//create a const reference
#define GLGE_CONST_REF(var) const var& 
//else, this is C
#else
//just parse the var
#define GLGE_CONST_REF(var) var
#endif //GLGE_CONST_REF define

/**
 * @brief convert something into a string
 */
#define GLGE_STRINGIFY(val) #val

/**
 * @brief convert the value of a define into a string
 */
#define GLGE_STRINGIFY_DEFINE(def) GLGE_STRINGIFY(def)

/**
 * @brief concatinate two elements
 * 
 * @param a the front element
 * @param b the second element
 */
#define GLGE_CONCATINATE(a, b) a ## b

/**
 * @brief concatinate a define and a normal element
 * 
 * @param def the define to concatinate with
 * @param b the second element
 */
#define GLGE_DEFINE_CONCATINATE(def, b) GLGE_CONCATINATE(def, b)

/**
 * @brief concatinate a define and a normal element
 * 
 * @param a the front element
 * @param def the define to concatinate the front element
 */
#define GLGE_CONCATINATE_DEFINE(a, def) GLGE_CONCATINATE(a, def)

/**
 * @brief concatinate two defines
 * 
 * @param defa the front define
 * @param defb the second define
 */
#define GLGE_DEFINE_CONCATINATE_DEFINE(defa, defb) GLGE_CONCATINATE(defa, defb)

/**
 * @brief concatinate 3 elements
 * 
 * @param a the first element
 * @param b the element in the middle
 * @param c the element at the back
 */
#define GLGE_CONCATINATE_TRIPPLE(a, b, c) a##b##c

/**
 * @brief concatinate 3 elements where any of the elements may be defines or macros
 * 
 * @param a the first element
 * @param b the element in the middle
 * @param c the element at the back
 */
#define GLGE_CONCATINATE_TRIPPLE_ANY_DEFINE(a, b, c) GLGE_CONCATINATE_TRIPPLE(a, b, c)

//check if this is C++
#if GLGE_CPP
//enable inline for functions
#define GLGE_FUNCTION_INLINE inline
//enable noexcept qualifier
#define GLGE_FUNC_NOEXCEPT noexcept
//else, this is C
#else
//disable inline functions
#define GLGE_FUNCTION_INLINE
//disable the noexcept qualifier
#define GLGE_FUNC_NOEXCEPT
#endif //C / C++ section for inline functoins

//check if a prefix should be used for functions
#if GLGE_PREFIX_C_FUNCTIONS
//define the actual function prefix as the specified prefix
#define GLGE_FUNC_PREFIX GLGE_PREFIX
//else, no prefixing should occour
#else
//set the prefix to empty
#define GLGE_FUNC_PREFIX
#endif //function prefixer

//this macro helps to prefix a C function
#define GLGE_C_FUNC(func) GLGE_DEFINE_CONCATINATE(GLGE_FUNC_PREFIX, func)

//check if debugging is enabled
#if GLGE_ENABLE_DEBUGGING
//say that debugging is enabled
#define GLGE_DEBUG_WRAPPER(debug) debug
#else
//say that debugging is disabled
#define GLGE_DEBUG_WRAPPER(debug)
#endif //debug check

/**
 * @brief allign some variable to a specific byte boundry
 * 
 * @param bytes how many bytes to allign to
 */
#define GLGE_ALIGN_BYTE(bytes) __attribute__ ((aligned (bytes)))

#endif