/**
 * @file GLGELimiter.cpp
 * @author DM8AT
 * @brief implement the default functions for the limiter
 * @version 0.1
 * @date 2025-04-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the limiter
#include "GLGELimiter.h"

void Limiter::startTick() noexcept
{
    //store the start time
    m_last = std::chrono::system_clock::now();
}

void Limiter::endTick() noexcept
{
    //store the time difference
    std::chrono::nanoseconds diff = (std::chrono::system_clock::now() - m_last);

    //compute the sleep time in nanoseconds
    std::chrono::nanoseconds sleep((uint64_t)(1E9 / (double)m_iterationRate));

    //check if the difference is less than the sleep time
    if (diff < sleep)
    {
        //compute the time to sleep
        std::chrono::nanoseconds sleepTime = sleep - diff;
        //sleep the rest of the time
        std::this_thread::sleep_for(sleepTime);
    }
}
