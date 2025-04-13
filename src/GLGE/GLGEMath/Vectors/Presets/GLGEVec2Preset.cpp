/**
 * @file GLGEVec2Preset.cpp
 * @author DM8AT
 * @brief implement the functions for the 2D vector preset
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//inclue the utility functions
#include "../../../GLGECore/GLGEUtil.h"
//include the settings
#include "../../../GLGECore/GLGESettings.h"

const char* GLGE_C_FUNC(GLGE_DEFINE_CONCATINATE(GLGE_VEC_NAME, _asString))(GLGE_CONST_REF(GLGE_VEC_NAME) vec) GLGE_FUNC_NOEXCEPT
{
    //check if this is C++
    #if GLGE_CPP
    //get the C++ string
    std::string str = vec.asString().c_str();
    //allocate the output string
    char* outstr = (char*)malloc(str.size()+1);
    //clear the buffer
    bzero(outstr, str.size()+1);
    //copy the data
    memcpy(outstr, str.data(), str.size());
    //return the string
    return (const char*)outstr;
    //else, this is C
    #else
    //store the format prefix
    const char* fmtPref = GLGE_STRINGIFY_DEFINE(GLGE_VEC_NAME);
    //store the format suffix
    const char* fmtSuff = "{%f, %f}";
    //concatinate both
    char fmt[16] = { 0 };
    //copy the prefix
    memcpy(fmt, fmtPref, strlen(fmtPref));
    //copy the suffix
    memcpy(fmt + strlen(fmtPref), fmtSuff, strlen(fmtSuff));
    //get the size of the output
    uint64_t outs = snprintf(NULL, 0, fmt, vec.x, vec.y);
    //create the output
    const char* str = (const char*)malloc(outs+1);
    //clear the output
    bzero((void*)str, outs+1);
    //fill the output buffer
    snprintf((char*)str, outs, fmt, vec.x, vec.y);
    //return the string
    return str;
    #endif
}

//clean up: undefine the defines
#undef GLGE_VEC_TYPE
#undef GLGE_VEC_NAME
#undef GLGE_VEC_STRUCT_NAME
