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

void Limiter::start() noexcept
{
    //store the start time
    m_last = std::chrono::system_clock::now();
}

void Limiter::endTick() noexcept
{
    //store the new last
    auto newLast = std::chrono::system_clock::now();
    //store the time difference
    std::chrono::nanoseconds diff = (newLast - m_last);
    std::chrono::nanoseconds sleepDiff = (newLast - m_sleepLast);
    //update the last
    m_sleepLast = newLast;

    //shift the buffer register up
    memcpy(m_samples+1, m_samples, sizeof(*m_samples)*(GLGE_LIMITER_IPS_SAMPLES-1));
    //store the new element
    m_samples[0] = 1E9 / (float)sleepDiff.count();
    //set the sample count
    ++m_smoothCount;
    m_smoothCount = (m_smoothCount > GLGE_LIMITER_IPS_SAMPLES) ? GLGE_LIMITER_IPS_SAMPLES : m_smoothCount;
    //calculate the smoothed middle
    float accum = 0;
    for (uint32_t i = 0; i < m_smoothCount; ++i)
    {
        //add to the accumulator
        accum += m_samples[i];
    }
    //store the smoothed sum
    m_smoothed = accum / (float)m_smoothCount;

    //check if the limiter is unlimited
    if (m_iterationRate != GLGE_LIMITER_UNLIMITED)
    {
        //compute the sleep time in nanoseconds
        std::chrono::nanoseconds sleep((uint64_t)(1E9 / (float)m_iterationRate));

        //check if the difference is less than the sleep time
        if (diff < sleep)
        {
            //compute the time to sleep
            std::chrono::nanoseconds sleepTime = sleep - diff;
            //sleep the rest of the time
            std::this_thread::sleep_for(sleepTime);
        }
    }

    //update the last tick time
    m_last = std::chrono::system_clock::now();
}
