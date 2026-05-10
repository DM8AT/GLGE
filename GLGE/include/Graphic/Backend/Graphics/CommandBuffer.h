/**
 * @file CommandBuffer.h
 * @author DM8AT
 * @brief define the backend API for a command buffer
 * 
 * Each backend may have different formats on how the actual GPU commands are stored. 
 * 
 * @version 0.1
 * @date 2025-12-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHIC_COMMAND_BUFFER_
#define _GLGE_GRAPHIC_BACKEND_GRAPHIC_COMMAND_BUFFER_

//include common stuff
#include "Core/Common.h"
//add references
#include "Core/Reference.h"

namespace GLGE::Graphic {
    /**
     * @brief render pipelines are defined somewhere else
     */
    class RenderPipeline;
}

//use the library namespace
namespace GLGE::Graphic::Backend::Graphic {

    /**
     * @brief store the abstract structure of a command buffer
     * 
     * This buffer holds a list of actual GPU functions with the corresponding parameters. 
     * During recording the commands with parameters are gathered. After recording is done they may be
     * converted to a backend-specific format for quick playback. During playback the actual commands
     * are executed on the GPU. The playback function should NOT block execution, this should be done
     * by a separate function. 
     */
    class CommandBuffer : public Referable {
    public:

        /**
         * @brief Construct a new Command Buffer
         * 
         * All command buffers are initialized in an empty state
         */
        CommandBuffer(GLGE::Graphic::RenderPipeline* renderPipeline);

        /**
         * @brief Destroy the Command Buffer
         */
        virtual ~CommandBuffer() {}

        /**
         * @brief add a new command to the CPU side command buffer
         */
        template <typename Ret, typename... Args>
        requires (std::is_trivially_copyable_v<std::decay_t<Args>> && ...)
        void addCommand(Ret (*fn)(Args...), Args... args) {
            //store the function pointer type
            using Fn = Ret (*)(Args...);
            //store the type of the invoker function pointer
            using InvPtr = void (*)(void*, void*);
            //create a tuple for the arguments
            using Tup = std::tuple<std::decay_t<Args>...>;
            Tup tup = std::make_tuple(std::forward<Args>(args)...);

            //store the sizes
            constexpr size_t fnPtrSize = sizeof(Fn);
            constexpr size_t invPtrSize = sizeof(InvPtr);
            constexpr size_t argSize = sizeof(Tup);
            //store the total size
            constexpr u16 total = sizeof(u16) + 
                                  sizeof(u8)  + 
                                  fnPtrSize   + 
                                  invPtrSize  +
                                  argSize;

            //sanity check the total size (cannot be larger than 2^16 - 1)
            static_assert(total < UINT16_MAX, "Tried to store a function that is too large for a command buffer inside a command buffer (cmd buff data overflow)");

            //allocate new space in the buffer if needed
            //if the buffer is full, resize to the next power of 2
            size_t offs = m_buffer.size();
            size_t required = offs + total;
            if (required > m_buffer.capacity()) {
                //reserve size aligned to the next power of 2
                m_buffer.reserve(nextPowerOf2(required));
            }
            //resize only to the actually required size
            m_buffer.resize(required);

            //get the actual u8 buffer to write to
            u8* p = m_buffer.data() + offs;

            //store the total size
            *((u16*)p) = total;
            p += sizeof(u16);

            //store the function pointer size
            *p = fnPtrSize;
            p++;

            //store the actual function pointer
            void* vPtr = reinterpret_cast<void*>(fn);
            memcpy(p, &vPtr, fnPtrSize);
            p += fnPtrSize;

            //store the invoker pointer
            InvPtr inv = &invoker<Fn, Args...>;
            memcpy(p, &inv, invPtrSize);
            p += invPtrSize;

            //store the raw argument tuple
            memcpy(p, &tup, argSize);
        }

        /**
         * @brief clean up the command buffer
         */
        void clear();

        /**
         * @brief a function used to say that a recording should start
         */
        virtual void onBegin() {}

        /**
         * @brief finalize the recorded command buffer
         */
        virtual void finalize() final;

        /**
         * @brief check if the command buffer is recorded
         * 
         * @return `true` if the command buffer is recorded, `false` otherwise
         */
        inline bool isRecorded() const noexcept
        {return (m_flags & FLAG_RECORDED) ? true : false;}

        /**
         * @brief execute the stored commands
         */
        virtual void play() final;

    protected:

        /**
         * @brief a function used to finalize the recorded command buffer
         */
        virtual void onFinalize() {}

        /**
         * @brief a function used to play back the recorded command buffer
         */
        virtual void onPlay() {playback();}

        /**
         * @brief play back all CPU functions stored in the command buffer
         */
        void playback();

        /**
         * @brief Get the Render Pipeline
         * 
         * @return `GLGE::Graphic::RenderPipeline*` a pointer to the render pipeline
         */
        inline GLGE::Graphic::RenderPipeline* getRenderPipeline() const noexcept
        {return m_renderPipeline;}

    private:

        /**
         * @brief a helper function to compute the next power of 2 of an integer in a fast way
         * 
         * @param n the value to compute the next power of 2 for
         * @return `constexpr uint64_t` the next power of 2 for the input
         */
        constexpr uint64_t nextPowerOf2(uint64_t n) {
            //subtraction for exact powers of 2
            n--;
            //propagate the highest bit to lower bits using bitshift or
            n |= n >> 1;
            n |= n >> 2;
            n |= n >> 4;
            n |= n >> 8;
            n |= n >> 16;
            n |= n >> 32;
            //increment to get the NEXT power of 2
            n++;
            //return the found power of 2
            return n;
        }

        /**
         * @brief a static function that can call a type-erased storage for both a function pointer and argument list
         * 
         * @tparam Fn the type of the function pointer to call
         * @tparam Args the types for the arguments
         * @param fnptr a pointer to the storage for the function pointer
         * @param argptr a pointer to the storage for the 
         */
        template <typename Fn, typename... Args>
        static void invoker(void* fnptr, void* argptr) {
            //set a type that is for the correct tuple
            using Tup = std::tuple<std::decay_t<Args>...>;

            //get the function pointer
            Fn f;
            memcpy(&f, fnptr, sizeof(Fn));

            //recover the arguments
            auto* tup = reinterpret_cast<Tup*>(argptr);

            //call the function
            (void) std::apply(f, *tup);
        }

        /**
         * @brief store a flag to determine if the command buffer is recorded
         */
        inline constexpr static u8 FLAG_RECORDED = 0b1;

        /**
         * @brief store flags about the current state
         */
        u8 m_flags = 0;
        /**
         * @brief store the actual commands in a consecutive pool
         * 
         */
        std::vector<u8> m_buffer;

        /**
         * @brief store a pointer to the render pipeline
         */
        GLGE::Graphic::RenderPipeline* m_renderPipeline = nullptr;

    };

}

#endif