/**
 * @file Profiler.h
 * @author DM8AT
 * @brief define some profiler helpers that, below the hood, use Tracy
 * 
 * The actual profiling functions are only available in debug builds. In release builds they will automatically remove themselves. 
 * This can be overridden by defining `GLGE_PROFILER_ENABLE` to 1 before including the file. By defining it to 0 the profiler will be disabled. 
 * 
 * Everything is wrapped using this file to minimize code change if the profiler is changed. 
 * 
 * @version 0.1
 * @date 2026-01-02
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_CORE_PROFILER_
#define _GLGE_CORE_PROFILER_

//only continue if the profiler is enabled
#if GLGE_PROFILER_ENABLE

//include tracy and enable it
#ifndef TRACY_ENABLE
#define TRACY_ENABLE
#endif
#include "Tracy.hpp"

/**
 * @brief begin a recording scope for the profiler
 */
#define GLGE_PROFILER_SCOPE() ZoneScoped

/**
 * @brief begin a recording scope for the profiler with a specific color
 * 
 * @param color the color for the scope
 */
#define GLGE_PROFILER_SCOPE_COLOR(color) ZoneScopedC(color)

/**
 * @brief begin a recording scope for the profiler with a specific name
 * 
 * @param name the name for the scope
 */
#define GLGE_PROFILER_SCOPE_NAMED(name) ZoneScopedN(name)

/**
 * @brief begin a recording scope for the profiler with a specific name and color
 * 
 * @param name the name for the scope
 * @param color the color for the scope
 */
#define GLGE_PROFILER_SCOPE_COLOR_NAMED(name, color) ZoneScopedNC(name, color)

/**
 * @brief mark the start of a new frame
 * 
 * @param name the name of the frame to start
 */
#define GLGE_PROFILER_FRAME_MARKER_START(name) FrameMarkStart(name)

/**
 * @brief mark the end of a frame
 * 
 * @param name the name of the frame to end
 */
#define GLGE_PROFILER_FRAME_MARKER_END(name) FrameMarkEnd(name); FrameMark

/**
 * @brief send a message to the profiler
 * 
 * @param message the message to send
 * @param size the text size of the message
 */
#define GLGE_PROFILER_SEND_MESSAGE(message, size) TracyMessage(message, size)

/**
 * @brief send a message to the profiler with a specific color
 * 
 * @param message the message to send
 * @param size the text size of the message
 * @param color the color of the message
 */
#define GLGE_PROFILER_SEND_MESSAGE_COLOR(message, size, color) TracyMessageC(message, size, color)

/**
 * @brief set the name of a specific thread
 * 
 * @param name the name of the thread
 */
#define GLGE_PROFILER_SET_THREAD_NAME(name) tracy::SetThreadName(name)

/**
 * @brief set the name for the application
 * 
 * @param name the name for the application
 */
#define GLGE_PROFILER_SET_APP_NAME(name) TracySetProgramName(name)

#else //else, define only dummies

/**
 * @brief begin a recording scope for the profiler
 */
#define GLGE_PROFILER_SCOPE()

/**
 * @brief begin a recording scope for the profiler with a specific color
 * 
 * @param color the color for the scope
 */
#define GLGE_PROFILER_SCOPE_COLOR(color)

/**
 * @brief begin a recording scope for the profiler with a specific name
 * 
 * @param name the name for the scope
 */
#define GLGE_PROFILER_SCOPE_NAMED(name)

/**
 * @brief begin a recording scope for the profiler with a specific name and color
 * 
 * @param name the name for the scope
 * @param color the color for the scope
 */
#define GLGE_PROFILER_SCOPE_COLOR_NAMED(name, color)

/**
 * @brief mark the start of a new frame
 * 
 * @param name the name of the frame to start
 */
#define GLGE_PROFILER_FRAME_MARKER_START(name)

/**
 * @brief mark the end of a frame
 * 
 * @param name the name of the frame to end
 */
#define GLGE_PROFILER_FRAME_MARKER_END(name)

/**
 * @brief send a message to the profiler
 * 
 * @param message the message to send
 * @param size the text size of the message
 */
#define GLGE_PROFILER_SEND_MESSAGE(message, size)

/**
 * @brief send a message to the profiler with a specific color
 * 
 * @param message the message to send
 * @param size the text size of the message
 * @param color the color of the message
 */
#define GLGE_PROFILER_SEND_MESSAGE_COLOR(message, size, color)

/**
 * @brief set the name of a specific thread
 * 
 * @param name the name of the thread
 */
#define GLGE_PROFILER_SET_THREAD_NAME(name)

/**
 * @brief set the name for the application
 * 
 * @param name the name for the application
 */
#define GLGE_PROFILER_SET_APP_NAME(name)

#endif

#endif