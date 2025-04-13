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
     * @brief start a tick
     */
    void startTick() noexcept;

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

protected:

    /**
     * @brief store the amount of iterations per second
     */
    uint64_t m_iterationRate = 60;
    /**
     * @brief store the time of the last start
     */
    std::chrono::system_clock::time_point m_last;
};

#endif

#endif