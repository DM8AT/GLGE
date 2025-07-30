/**
 * @file GLGETime.cpp
 * @author DM8AT
 * @brief implement the time stuff for GLGE
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//incldue the header
#include "GLGETime.h"

Time GLGE_C_FUNC(getLocalTime)() GLGE_FUNC_NOEXCEPT
{
    //get the current time
    time_t now = time(0);
    //convert the time to a tm structure
    struct tm dt = *localtime(&now);
    //return a new structure with all parameters set
    return {(uint8_t)dt.tm_hour, (uint8_t)dt.tm_min, (uint8_t)dt.tm_sec};
}

/**
 * @brief get the current time in UTC
 */
Time GLGE_C_FUNC(getUTCTime)() GLGE_FUNC_NOEXCEPT
{
    //get the current time
    time_t now = time(0);
    //convert the time to a tm structure
    struct tm dt = *gmtime(&now);
    //return a new structure with all parameters set
    return {(uint8_t)dt.tm_hour, (uint8_t)dt.tm_min, (uint8_t)dt.tm_sec};
}