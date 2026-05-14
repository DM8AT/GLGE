/**
 * @file Buffer.h
 * @author DM8AT
 * @brief define the backend API for buffers
 * @version 0.1
 * @date 2026-03-20
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUFFER_
#define _GLGE_GRAPHIC_BACKEND_BUFFER_

//include common stuff
#include "Core/Common.h"
//add references
#include "Core/Reference.h"
//add resources
#include "Graphic/Resource.h"
//add backend instances
#include "Instance.h"

//use the library namespace
namespace GLGE::Graphic::Backend::Graphic {

    //framebuffers are defined later
    class Framebuffer;

    /**
     * @brief a buffers are utilities to store and move data from and to the GPU / CPU
     * 
     * A buffer is used to store, stream and access data that lives on both the CPU and GPU. There may be different access patterns. 
     */
    class Buffer : public Referable {
    public:

        /**
         * @brief define the type of the buffer
         */
        enum class Type : u8 {
            /**
             * @brief the buffer is a shader storage buffer
             */
            STORAGE = 0,
            /**
             * @brief the shader is a uniform buffer
             */
            UNIFORM
        };

        /**
         * @brief define the usage of the buffer
         */
        enum class Usage : u8 {
            /**
             * @brief the data lives on both the CPU and GPU and should be synchronized by the user manually
             * 
             * @warning on some drivers (especially NVIDIA and AMD) the driver may auto-sync the buffers. This is not required behaviour and identified as unsafe to assume. 
             */
            CPU_GPU = 0,
            /**
             * @brief the data lives only on the CPU (why? just use a normal std::vector or something) and cannot be read or written by the GPU
             */
            CPU_ONLY,
            /**
             * @brief the data lives only on the GPU and cannot be read by the CPU, but written by the CPU
             */
            GPU_ONLY,
            /**
             * @brief the data lives shared on the GPU and CPU using shared memory. The data synchronizes automatically, but read / writes may be slower. 
             * 
             * This streaming flag supports streaming to the GPU, but not from the GPU to the CPU. 
             */
            STREAMING_UPLOAD,
            /**
             * @brief the data lives shared on the GPU and CPU using shared memory. The data synchronizes automatically, but read / writes may be slower. 
             * 
             * This streaming flag supports streaming to the CPU, but not from the CPU to the GPU. 
             */
            STREAMING_DOWNLOAD,
            /**
             * @brief the data lives shared on the GPU and CPU using shared memory. The data synchronizes automatically, but read / writes may be slower. 
             * 
             * This streaming flag supports streaming to the GPU and from the GPU. 
             */
            STREAMING,

            /**
             * @brief default fallback to both CPU/GPU buffer without streaming
             */
            UNDEFINED = CPU_GPU
        };

        /**
         * @brief Construct a new Buffer
         * 
         * @param type the type of the buffer
         * @param initial a pointer to a data blob to store initially (if `nullptr` is parsed, it is ignored)
         * @param size the size of the data blob to store
         * @param instance a pointer to the backend instance that will own the buffer
         * @param usage the intended usage of the buffer
         */
        Buffer(Type type, const void* initial, size_t size, GLGE::Graphic::Backend::Graphic::Instance* instance, Usage usage = Usage::UNDEFINED)
         : m_type(type), m_size(size), m_usage(usage), m_instance(instance)
        {
            //if this is a CPU only buffer, store a copy
            if (m_usage == Usage::CPU_ONLY) {
                m_data = new u8[m_size];
                //if the initial data stores data, copy it. Else, set to 0. 
                if (initial) 
                {memcpy(m_data, initial, m_size);}
                else 
                {memset(m_data, 0, m_size);}
            }
        }

        /**
         * @brief Destroy the Buffer
         */
        virtual ~Buffer() {
            //if a buffer is allocated and the usage is not on the GPU or used for streaming, free it
            if ((m_usage == Usage::CPU_ONLY) && m_data) 
            {delete[] reinterpret_cast<u8*>(m_data);}
        }

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
        virtual void write(const void* data, size_t size, size_t offset) = 0;

        /**
         * @brief read a section of the buffer
         * 
         * @warning no bounds checking is guaranteed internally. User checking is the user's responsibility. 
         * 
         * @param data a pointer to the data buffer to write the read result to
         * @param size the size to read. This must be the minimum size of the data buffer
         * @param offset the offset to read from
         */
        virtual void read(void* data, size_t size, size_t offset) = 0;

        /**
         * @brief change the size of the buffer
         * 
         * @param size the new size for the buffer
         * @param preserve `true` This expands the data with 0 or cuts off the data, `false` discards the old data and fills it with 0
         */
        virtual void resize(size_t size, bool preserve = false) = 0;

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
        virtual void fill(const void* pattern, size_t patternSize, size_t n, size_t offset) = 0;

        /**
         * @brief synchronize the CPU and GPU buffer
         * 
         * If the usage is `STREAMING` this function does nothing. If a CPU or GPU buffer does not exist, this function does nothing. 
         * 
         * @param fromGPU `true` to load back the data from the GPU, `false` to push the data from the CPU to the GPU
         */
        virtual void sync(bool fromGPU) = 0;

        /**
         * @brief a function that is called to build a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to bind to
         * @param unit the unit to bind to
         */
        virtual void onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) = 0;

        /**
         * @brief a function that is called to build a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to bind to
         */
        virtual void onDropBinding(GLGE::Graphic::ResourceSet* set) = 0;

        /**
         * @brief Get the type
         * 
         * @return `Type` the type of the stored buffer
         */
        inline Type getType() const noexcept
        {return m_type;}

        /**
         * @brief Get the usage
         * 
         * @return `Usage` the usage of the stored buffer
         */
        inline Usage getUsage() const noexcept
        {return m_usage;}

        /**
         * @brief Get the size
         * 
         * @return `size_t` the size of the stored buffer in bytes
         */
        inline size_t getSize() const noexcept
        {return m_size;}

        /**
         * @brief check if this is some sort of streaming buffer
         * 
         * @return `true` if the buffer uses streaming, `false` if it does not support streaming in any direction
         */
        inline bool isStreamBuffer() const noexcept
        {return (m_usage == Usage::STREAMING) || (m_usage == Usage::STREAMING_UPLOAD) || (m_usage == Usage::STREAMING_DOWNLOAD);}

    protected:

        /**
         * @brief a structure to store a range of a buffer
         */
        struct Range {
            /**
             * @brief offset from the start in bytes
             */
            size_t offset;
            /**
             * @brief size of the range in bytes
             */
            size_t size;
        };

        /**
         * @brief store the type of the buffer
         */
        Type m_type;
        /**
         * @brief store the usage of the buffer
         */
        Usage m_usage;

        /**
         * @brief store the size of the buffer in bytes
         */
        size_t m_size = 0;
        /**
         * @brief store the CPU data copy
         */
        void* m_data = nullptr;

        /**
         * @brief store all ranges of the CPU that are out of date
         */
        std::vector<Range> m_cpu_outOfDate;

        /**
         * @brief store a pointer to the owning instance
         */
        GLGE::Graphic::Backend::Graphic::Instance* m_instance = nullptr;

    };

}

#endif