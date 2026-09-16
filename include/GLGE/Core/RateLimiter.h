/**
 * @file RateLimiter.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-01-02
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_CORE_RATE_LIMITER_
#define _GLGE_CORE_RATE_LIMITER_

//include common stuff
#include "Common.h"
//for timing stuff
#include <chrono>
//for thread sleeping
#include <thread>
//include profiling
#include "Profiler.h"

//use the library namespace
namespace GLGE {

    /**
     * @brief this class is usefull to limit the frequency of which a code section can be run. This helps conserving resources for were they are required. 
     */
    template <size_t SMOOTHING_ACCUM_COUNT = 10>
    class RateLimitBase
    {
    public:

        /**
         * @brief Construct a new Rate Limiter instance
         */
        RateLimitBase() = default;

        /**
         * @brief Construct a new Rate Limiter instance
         * 
         * @param limit the maximum tick rate of the rate limiter
         */
        RateLimitBase(u32 limit) : m_limit(limit) {}

        /**
         * @brief Set a new limit for the rate limiter
         * 
         * @param limit the new limit in ticks per seconds
         */
        inline void setLimit(u32 limit) noexcept {m_limit = limit;}

        /**
         * @brief Get the current rate limit
         * 
         * @return u32 the current rate limit in ticks per seconds
         */
        inline u32 getLimit() const noexcept {return m_limit;}

        /**
         * @brief start the tick
         */
        inline void startTick() noexcept {
            GLGE_PROFILER_SCOPE();

            m_tickStart = std::chrono::system_clock::now();
        }

        /**
         * @brief end the tick
         */
        void endTick() noexcept {
            GLGE_PROFILER_SCOPE();

            //store delta and sleep delta in the global scope
            std::chrono::nanoseconds delta;
            std::chrono::nanoseconds sleepDelta;
            
            {
                GLGE_PROFILER_SCOPE_NAMED("Computing delta time");

                //get the current time
                std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
                //calculate the delta time
                delta = now - m_tickStart;
                sleepDelta = now - m_sleepTickStart;
                //update the sleep start
                m_sleepTickStart = now;
                //store the current delta time
                m_delta = std::chrono::duration_cast<std::chrono::microseconds>(sleepDelta).count() / 1000.;
            }

            {
                GLGE_PROFILER_SCOPE_NAMED("Computing smoothed delta time / smoothed FPS");

                //store the sample
                memcpy(m_smoothBuffer+1, m_smoothBuffer, sizeof(*m_smoothBuffer)*(SMOOTHING_ACCUM_COUNT-1));
                m_smoothBuffer[0] = std::chrono::duration_cast<std::chrono::microseconds>(sleepDelta).count();
                //store the new amount of accumulated items
                ++m_smoothCount;
                m_smoothCount = (m_smoothCount < SMOOTHING_ACCUM_COUNT) ? m_smoothCount : SMOOTHING_ACCUM_COUNT;

                //store the accumulated delta time
                f64 accum = 0;
                //iterate over the samples
                for (uint32_t i = 0; i < m_smoothCount; ++i)
                {
                    //accumulate the sample
                    accum += m_smoothBuffer[i] / 1000.;
                }
                //store the delta time
                m_smoothDelta = accum / ((f64)m_smoothCount);
            }

            {
                GLGE_PROFILER_SCOPE_NAMED("Waiting to stabilize iteration rate");

                //calculate the frame rate limit
                std::chrono::nanoseconds limit = std::chrono::nanoseconds((u64)(1000000000. / (double)m_limit));
                //check if there is time to sleep
                if ((delta < limit) && (m_limit != UNLIMITED))
                {
                    //sleep the missing time
                    std::this_thread::sleep_for(limit - delta);
                }
            }
        }

        /**
         * @brief Get the Current, unsmoothed delta time in milliseconds
         * 
         * @return f64 the current delta time in milliseconds
         */
        f64 getCurrentDeltaTime() const noexcept {return m_delta;}

        /**
         * @brief Get the current, unsmoothed delta time in seconds
         * 
         * @return f64 the current delta time in seconds
         */
        f64 getCurrentSecondDelta() const noexcept {return m_delta / 1000.;}

        /**
         * @brief Get the Current, unsmoothed ticks per second
         * 
         * @return f64 the current unsmoothed ticks per second
         */
        f64 getCurrentTPS() const noexcept {return 1000. / m_delta;}

        /**
         * @brief Get the current smoothed Delta Time in milliseconds
         * 
         * @return f64 the current smoothed delta time in milliseconds
         */
        f64 getSmoothDeltaTime() const noexcept {return m_smoothDelta;}

        /**
         * @brief Get the current smoothed delta time in seconds
         * 
         * @return f64 the current smoothed delta time in seconds
         */
        f64 getSmoothSecondDelta() const noexcept {return m_smoothDelta / 1000.;}

        /**
         * @brief Get the current smoothed ticks per seconds
         * 
         * @return f64 the current smoothed ticks per seconds
         */
        f64 getSmoothTPS() const noexcept {return 1000. / m_smoothDelta;}

        /**
         * @brief store the default iteration rate
         */
        inline static constexpr u32 DEFAULT = 60;

        /**
         * @brief store a marker for an unlimited iteration rate
         */
        inline static constexpr u32 UNLIMITED = 0;

    private:

        /**
         * @brief store the own rate limit in ticks per second
         */
        u32 m_limit = DEFAULT;

        /**
         * @brief the time at the start of the tick
         */
        std::chrono::system_clock::time_point m_tickStart = std::chrono::system_clock::now();

        /**
         * @brief the time of the start of the tick, but including the sleep time of the last tick
         */
        std::chrono::system_clock::time_point m_sleepTickStart = std::chrono::system_clock::now();

        /**
         * @brief the delta time of the last tick in milliseconds
         */
        f64 m_delta = 0;

        /**
         * @brief the amount of elements in the smooth frame array
         */
        u32 m_smoothCount = 0;

        /**
         * @brief store the delta times to smooth out the delta time
         */
        u32 m_smoothBuffer[SMOOTHING_ACCUM_COUNT] = { 0 };

        /**
         * @brief the smooth delta time smoothed over GLGE_RATE_SMOOTHING_ACCUMULATION_COUNT ticks in milliseconds
         */
        f64 m_smoothDelta = 0;
    };

    /**
     * @brief identify a rate limit as the default value for the rate limit base
     */
    typedef RateLimitBase<> RateLimit;

}

#endif