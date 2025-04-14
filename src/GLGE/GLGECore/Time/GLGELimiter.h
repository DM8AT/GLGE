/**
 * @file GLGELimiter.h
 * @author DM8AT
 * @brief define a limiter class that can be used to limit the iteration rate of loops
 * @version 0.1
 * @date 2025-04-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_LIMITER_
#define _GLGE_LIMITER_

//include the settings and utilities
#include "../GLGEUtil.h"

/**
 * @brief define the value that a turns off iteration limitation for a limiter
 */
#define GLGE_LIMITER_UNLIMITED 0

//check if this is C++
#if GLGE_CPP

/**
 * @brief a limiter can limit the speed at wich a loop is run
 */
class Limiter
{
public:

    /**
     * @brief Construct a new Limiter
     */
    inline Limiter() = default;

    /**
     * @brief Construct a new Limiter
     * 
     * @param iterationRate the iteration rate for the limiter
     */
    inline Limiter(uint64_t iterationRate) noexcept : m_iterationRate(iterationRate) {}

    /**
     * @brief start the limiter
     */
    void start() noexcept;

    /**
     * @brief end a tick and limit to the iteration rate
     */
    void endTick() noexcept;

    /**
     * @brief Set the Iteration Rate
     * 
     * @param iterationRate the new iteration rate in iterations per second
     */
    inline void setIterationRate(uint64_t iterationRate) noexcept {m_iterationRate = iterationRate;}

    /**
     * @brief Get the Iteration Rate
     * 
     * @return uint64_t the iteration rate in iterations per second
     */
    inline uint64_t getIterationRate() const noexcept {return m_iterationRate;}

    /**
     * @brief get the smoothed iteration rate
     * 
     * @return double the smoothed iteration rate
     */
    inline float getIPS() const noexcept {return m_smoothed;}

    /**
     * @brief Get the Raw iteration rate
     * 
     * @return double the raw iteration rate
     */
    inline float getRawIPS() const noexcept {return m_samples[0];}

protected:

    /**
     * @brief store the amount of iterations per second
     */
    uint64_t m_iterationRate = 60;
    /**
     * @brief store the samples used for FPS smoothing
     */
    float m_samples[GLGE_LIMITER_IPS_SAMPLES] = { 0 };
    /**
     * @brief store the smoothed FPS
     */
    float m_smoothed = 0;
    /**
     * @brief store the amount of samples in the smoothing buffer
     */
    uint32_t m_smoothCount = 0;
    /**
     * @brief store the time of the last start
     */
    std::chrono::system_clock::time_point m_last;
    /**
     * @brief the last tick time including the sleeping
     */
    std::chrono::system_clock::time_point m_sleepLast;
};

#endif

#endif