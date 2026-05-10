/**
 * @file CommandBuffer.cpp
 * @author DM8AT
 * @brief implement the functionality for the command buffer
 * @version 0.1
 * @date 2026-01-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//include the command buffer
#include "Graphic/Backend/Graphics/CommandBuffer.h"
//add exceptions
#include "Core/Exception.h"

//add profiling
#include "Core/Profiler.h"

//use the library namespace
using namespace GLGE::Graphic::Backend::Graphic;

CommandBuffer::CommandBuffer(GLGE::Graphic::RenderPipeline* pipeline)
 : m_renderPipeline(pipeline)
{m_buffer.reserve(1024);}

void CommandBuffer::playback() {
    GLGE_PROFILER_SCOPE();

    //store the begin and end of the command buffer data storage
    u8* p = m_buffer.data();
    u8* e = p + m_buffer.size();

    //iterate until the end of the buffer is reached
    while (p < e)
    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::CommandBuffer::playback - single command playback");

        //read the size of the current command block
        u16 total = *((u16*)p);
        p += sizeof(u16);
        //read the size of the function pointer
        u8 fptrSize = *p;
        p++;
        //read a pointer to the function pointer
        void* fptr = p;
        p += fptrSize;
        //read a pointer to the invoker function
        using InvPtr = void (*)(void*, void*);
        InvPtr* invP = (InvPtr*)p;
        p += sizeof(InvPtr);

        //arguments at this point are just p

        //call the invoker
        (*invP)(fptr, p);

        //skip the arguments
        p = (u8*)p + (total - (sizeof(u16) + sizeof(u8) + fptrSize + sizeof(InvPtr)));
    }
}

void CommandBuffer::clear() {
    GLGE_PROFILER_SCOPE();

    //remove the recorded flag
    m_flags &= ~FLAG_RECORDED;
    //clean up the buffer
    m_buffer.clear();
}

void CommandBuffer::finalize() {
    GLGE_PROFILER_SCOPE();

    //call the finalization hook
    onFinalize();
    //set the recorded state to true
    m_flags |= FLAG_RECORDED;
}

void CommandBuffer::play() {
    GLGE_PROFILER_SCOPE();

    //if the command buffer is not recorded stop (and throw an error in debug)
    if (!isRecorded()) {
        #if GLGE_DEBUG
        //throw only in debug
        throw GLGE::Exception("Tried to play back a command buffer that is not recorded", "GLGE::Graphic::Backend::Graphic::CommandBuffer::play");
        #endif
        //stop
        return;
    }

    //call the play back hook
    onPlay();
}