/**
 * @file GLGETime.h
 * @author DM8AT
 * @brief time management for GLGE
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_CLOCK_
#define _GLGE_CLOCK_

//include the settings
#include "../GLGESettings.h"
//include the utilities
#include "../GLGEUtil.h"

//check if this is C++
#if GLGE_CPP
extern "C" {
#endif

/**
 * @brief store a timestamp
 */
typedef struct s_Time {
    /**
     * @brief the our of the timestap
     */
    uint8_t hour;
    /**
     * @brief the minute of the timestamp
     */
    uint8_t minute;
    /**
     * @brief the second of the timestamp
     */
    uint8_t second;
} Time;

/**
 * @brief get the current time
 */
Time GLGE_C_FUNC(getLocalTime)() GLGE_FUNC_NOEXCEPT;

/**
 * @brief get the current time in UTC
 */
Time GLGE_C_FUNC(getUTCTime)() GLGE_FUNC_NOEXCEPT;

//check if this is C++
#if GLGE_CPP
}
#endif

//check if this is C++
#if GLGE_CPP

/**
 * @brief print a time stap to an output stream
 * 
 * @param os the output stream to print to
 * @param t a constant reference to the time point to print
 * @return std::ostream& a reference to the filled output stream
 */
inline std::ostream& operator<<(std::ostream& os, const Time& t) noexcept {return os << (uint32_t)t.hour << ":" << (uint32_t)t.minute << ":" << (uint32_t)t.second;}

#endif //C++ section

#endif