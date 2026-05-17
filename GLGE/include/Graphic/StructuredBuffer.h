/**
 * @file StructuredBuffer.h
 * @author DM8AT
 * @brief define what a structured buffer ist
 * 
 * A structured buffer is a buffer that contains an array
 * 
 * @version 0.1
 * @date 2026-03-22
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_STRUCTURED_BUFFER_
#define _GLGE_GRAPHIC_STRUCTURED_BUFFER_

//include common stuff
#include "Core/Common.h"
//add the base class
#include "Core/BaseClass.h"
//add instances
#include "Core/Instance.h"
//add graphic instances
#include "Instance.h"

//add resources
#include "Resource.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief define a structured buffer
     * 
     * A structured buffer is like a C++ vector of structures
     * 
     * @warning in some storage layouts specific alignments are required. If the structure does NOT satisfy the alignment the access in the shader may be wrong. 
     * 
     * @tparam T the structures stored in the structured buffer
     */
    template <typename T>
    requires (std::is_trivially_constructible_v<T> && std::is_trivially_copyable_v<T>)
    class StructuredBuffer : public BaseClass, public Resource {
    public:

        /**
         * @brief store usage flags for the raw buffer
         */
        using Usage = GLGE::Graphic::Backend::Graphic::Buffer::Usage;

        /**
         * @brief store type flags for the raw buffer
         */
        using Type = GLGE::Graphic::Backend::Graphic::Buffer::Type;

        /**
         * @brief Construct a new Structured Buffer
         * 
         * @param data the initial data of the structured buffer
         * @param count the amount of ELEMENTS in the buffer
         * @param type the type of buffer to create
         * @param usage the usage of the buffer
         */
        StructuredBuffer(T* data, size_t count, Type type, Usage usage = Usage::CPU_GPU)
         : BaseClass(), Resource(), m_backend(getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicDescription()->createBuffer(type, data, count*sizeof(T), usage, getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicBackendInstance().get()))
        {}

        /**
         * @brief Construct a new Structured Buffer
         * 
         * @param initial the initial values for the structured buffer
         * @param type the type of buffer to create
         * @param usage the usage of the buffer
         */
        StructuredBuffer(std::initializer_list<T> initial, Type type, Usage usage = Usage::CPU_GPU)
         : BaseClass(), Resource(), m_backend(getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicDescription()->createBuffer(type, initial.begin(), initial.size()*sizeof(T), usage, getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicBackendInstance().get()))
        {}

        /**
         * @brief get a specific element from the buffer
         * 
         * @param idx the index to quarry
         * @return `T` a copy of the entry at that index
         */
        inline T get(size_t idx) noexcept {
            //read back the data into a buffer
            T out;
            m_backend->read(&out, sizeof(T), idx*sizeof(T));
            //return the data
            return out;
        }

        /**
         * @brief set an element of the structured buffer
         * 
         * @param value a constant reference to the value
         * @param idx the index of the value to set
         */
        inline void set(const T& value, size_t idx) noexcept 
        {m_backend->write(&value, sizeof(T), sizeof(T)*idx);}

        /**
         * @brief write an element of some structure
         * 
         * @tparam M the type of the member
         * @param member a pointer to the member in the structure
         * @param value a constant reference to the value to set
         * @param index the index of the element to set the member for
         */
        template <typename M>
        inline void set(M T::* member, const M& value, size_t index = 0)
        {write(&value, sizeof(M), reinterpret_cast<size_t>(&(reinterpret_cast<T const volatile*>(0)->*member)) + (index * sizeof(T)));}

        /**
         * @brief get an element of the structured buffer
         * 
         * @param idx the index of the element to get
         * @return `T` a copy of the element at the specific index
         */
        inline T operator[](size_t idx) const noexcept
        {return get(idx);}

        /**
         * @brief synchronize the CPU and GPU buffer
         * 
         * If the usage is `STREAMING` this function does nothing. If a CPU or GPU buffer does not exist, this function does nothing. 
         * 
         * @param fromGPU `true` to load back the data from the GPU, `false` to push the data from the CPU to the GPU
         */
        inline void sync(bool fromGPU)
        {m_backend->sync(fromGPU);}

        /**
         * @brief a function that is called to build a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to bind to
         * @param unit the unit to bind to
         */
        virtual void onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) override
        {m_backend->onBuildBinding(set, unit);}

        /**
         * @brief a function that is called when a resource set holding a reference to this resource is destroyed
         * 
         * @param set a pointer to the resource set that was destroyed
         */
        virtual void onRemoveBinding(ResourceSet* set) override
        {m_backend->onDropBinding(set);}

        /**
         * @brief Get the type of resource this is
         * 
         * @return `ResourceType` the type to identify the resource
         */
        virtual ResourceType getType() const noexcept override
        {return (m_backend->getType() == Type::STORAGE) ? ResourceType::STORAGE_BUFFER : ResourceType::UNIFORM_BUFFER;}

        /**
         * @brief Get the hash of the actual resource type
         * 
         * @return `u64` the hash of the type
         */
        virtual u64 getTypeHash() const noexcept override
        {return getTypeHash64<StructuredBuffer<T>>();}
        
        /**
         * @brief Get the type of the buffer
         * 
         * @return `Type` the type of the stored buffer
         */
        inline Type getBufferType() const noexcept
        {return m_backend->getType();}

        /**
         * @brief Get the usage
         * 
         * @return `Usage` the usage of the stored buffer
         */
        inline Usage getUsage() const noexcept
        {return m_backend->getUsage();}

        /**
         * @brief Get the size
         * 
         * @return `size_t` the size of the stored buffer in bytes
         */
        inline size_t getSize() const noexcept
        {return m_backend->getSize();}

        /**
         * @brief Get the amount of stored elements
         * 
         * @return `size_t` the size of the stored elements in the buffer
         */
        inline size_t size() const noexcept
        {return m_backend->getSize() / sizeof(T);}

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
        inline void write(const void *data, size_t size, size_t offset)
        {m_backend->write(data, size, offset);}

        /**
         * @brief read a section of the buffer
         * 
         * @warning no bounds checking is guaranteed internally. User checking is the user's responsibility. 
         * 
         * @param data a pointer to the data buffer to write the read result to
         * @param size the size to read. This must be the minimum size of the data buffer
         * @param offset the offset to read from
         */
        inline void read(void* data, size_t size, size_t offset)
        {m_backend->read(data, size, offset);}

        /**
         * @brief change the size of the buffer
         * 
         * @param size the new size for the buffer in elements
         * @param preserve `true` This expands the data with 0 or cuts off the data, `false` discards the old data and fills it with 0
         */
        inline void resize(size_t size, bool preserve = false)
        {m_backend->resize(size*sizeof(T), preserve);}

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
        inline void fill(const void* pattern, size_t patternSize, size_t n, size_t offset)
        {m_backend->fill(pattern, patternSize, n, offset);}

        /**
         * @brief get if this is a valid resource
         * 
         * @return `const char*` `nullptr` if the resource is valid, a human-readable error string otherwise
         */
        virtual const char* onValidation() override {
            //always valid
            return nullptr;
        }

    protected:

        /**
         * @brief store the backend buffer
         */
        Reference<GLGE::Graphic::Backend::Graphic::Buffer> m_backend;

    };

}

#endif