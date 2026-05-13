/**
 * @file Buffer.h
 * @author DM8AT
 * @brief overload the buffer backend for OpenGL
 * @version 0.1
 * @date 2026-03-21
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BUILTIN_GRAPHICS_OGL_BUFFER_
#define _GLGE_GRAPHIC_BUILTIN_GRAPHICS_OGL_BUFFER_

//include the buffer backend
#include "Graphic/Backend/Graphics/Buffer.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::OpenGL {

    /**
     * @brief overload of the buffer backend for OpenGL
     */
    class Buffer : public GLGE::Graphic::Backend::Graphic::Buffer {
    public:

        /**
         * @brief Construct a new Buffer
         * 
         * @param type the type of the buffer
         * @param initial a pointer to a data blob to store initially (if `nullptr` is parsed, it is ignored)
         * @param size the size of the data blob to store
         * @param usage the intended usage of the buffer
         */
        Buffer(Type type, const void* initial, size_t size, Usage usage = Usage::UNDEFINED);

        /**
         * @brief Destroy the Buffer
         */
        virtual ~Buffer();

        /**
         * @brief write to a section of the buffer
         * 
         * This requires user-provided synchronization
         * 
         * @warning no bounds checking is guaranteed internally. User checking is the user's responsibility. 
         * 
         * @param data a pointer to the data to write
         * @param size the size to write, the data must be at least this size
         * @param offset the offset to write to
         */
        virtual void write(const void* data, size_t size, size_t offset) override;

        /**
         * @brief read a section of the buffer
         * 
         * @warning no bounds checking is guaranteed internally. User checking is the user's responsibility. 
         * 
         * @param data a pointer to the data buffer to write the read result to
         * @param size the size to read. This must be the minimum size of the data buffer
         * @param offset the offset to read from
         */
        virtual void read(void* data, size_t size, size_t offset) override;

        /**
         * @brief change the size of the buffer
         * 
         * @param size the new size for the buffer
         * @param preserve `true` This expands the data with 0 or cuts off the data, `false` discards the old data and fills it with 0
         */
        virtual void resize(size_t size, bool preserve = false) override;

        /**
         * @brief fill the buffer with a pattern
         * 
         * If the size is not cleanly divisible by the pattern size, then the end is skipped. 
         * 
         * The performance of this function is not always performant. Depending on the backend and the pattern's size slower fallbacks may be used. 
         * To quickly fill a buffer with zero, just set pattern to `nullptr` and the pattern size to 0. Most backends implement a fast fallback route
         * for filling with zero. 
         * 
         * @param pattern a pointer to the pattern
         * @param patternSize the size of the pattern in bytes
         * @param n the amount to past the pattern, 0 means automatically fit for the buffer
         * @param offset the byte offset from the start to past the pattern
         */
        virtual void fill(const void* pattern, size_t patternSize, size_t n, size_t offset) override;

        /**
         * @brief synchronize the CPU and GPU buffer
         * 
         * If the usage is `STREAMING` this function does nothing. If a CPU or GPU buffer does not exist, this function does nothing. 
         * 
         * @param fromGPU `true` to load back the data from the GPU, `false` to push the data from the CPU to the GPU
         */
        virtual void sync(bool fromGPU) override;

        /**
         * @brief a function that is called to build a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to bind to
         * @param unit the unit to bind to
         */
        virtual void onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) override;

        /**
         * @brief a function that is called to build a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to bind to
         */
        virtual void onDropBinding(GLGE::Graphic::ResourceSet* set) override {}

        /**
         * @brief Get the handle
         * 
         * @return `u32` the OpenGL buffer
         */
        inline u32 getHandle() const noexcept
        {return m_handle;}

    protected:

        /**
         * @brief store the OpenGL buffer
         */
        u32 m_handle = 0;
        /**
         * @brief store the flags
         */
        u32 m_flags = 0;

        /**
         * @brief store a counter for name generation
         */
        inline static u32 ms_nameCounter = 0;
        /**
         * @brief store the name enumerator
         */
        u32 m_name = ++ms_nameCounter;

    };

}

#endif