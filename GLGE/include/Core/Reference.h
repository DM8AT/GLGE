/**
 * @file Reference.h
 * @author DM8AT
 * @brief define a system for intrinsic reference counting
 * @version 0.1
 * @date 2026-03-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_CORE_REFERENCE_
#define _GLGE_CORE_REFERENCE_

//add common stuff
#include "Common.h"
//add the profiler
#include "Profiler.h"
//for thread safe counting
#include <atomic>

//use the library namespace
namespace GLGE {

    /**
     * @brief this is the base class required to make something referable to
     */
    class Referable {
    public:

        /**
         * @brief Get the reference count
         * 
         * @return `u32` the amount of live references
         */
        inline u32 getReferenceCount() const noexcept
        {return m_referenceCount.load(std::memory_order_relaxed);}

    protected:

        /**
         * @brief Construct a new Referable
         * 
         * @param other the referable to move from
         */
        Referable(Referable&& other)
         : m_referenceCount(other.m_referenceCount.load(std::memory_order_acquire))
        {other.m_referenceCount.store(0, std::memory_order_acquire);}

        /**
         * @brief Construct a new Referable
         */
        Referable() = default;

        /**
         * @brief Destroy the Referable
         */
        virtual ~Referable() = default;

    private:

        //give access to references
        template <typename T>
        requires std::is_base_of_v<Referable, T>
        friend class Reference;

        /**
         * @brief increase the reference count
         */
        inline void add_ref() noexcept
        {m_referenceCount.fetch_add(1, std::memory_order_relaxed);}

        /**
         * @brief decrement the reference count
         */
        void release_ref() noexcept {
            //decrement the count with conditional delete
            if (m_referenceCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
            {delete this;}
        }

        /**
         * @brief store the amount of alive references
         */
        std::atomic_uint32_t m_referenceCount{1};

    };

    /**
     * @brief a system that uses the referable class for intrusive reference counting
     * 
     * @tparam T the type to reference count
     */
    template <typename T>
    requires std::is_base_of_v<Referable, T>
    class Reference {
    public:

        /**
         * @brief Construct a new Reference
         */
        constexpr Reference() noexcept = default;

        /**
         * @brief Construct a new Reference
         * 
         * @param ptr the pointer to reference count
         * @param add `true` to add to the reference count, `false` to just wrap it
         */
        Reference(T* ptr, bool add = true) noexcept 
         : m_ptr(ptr) {
            GLGE_PROFILER_SCOPE_NAMED("GLGE::Reference<T>::Reference")

            //only add a reference if requested
            if (ptr && add)
            {m_ptr->add_ref();}
        }

        /**
         * @brief Construct a new Reference
         * 
         * @param other the reference to copy
         */
        Reference(const Reference& other) noexcept
         : m_ptr(other.m_ptr) {
            GLGE_PROFILER_SCOPE_NAMED("GLGE::Reference<T>::Reference")

            //increase the reference count
            if (m_ptr) {m_ptr->add_ref();}
        }

        /**
         * @brief Construct a new Referable
         * 
         * @param other the reference to move from 
         */
        Reference(Reference&& other) noexcept
         : m_ptr(other.m_ptr) {
            GLGE_PROFILER_SCOPE_NAMED("GLGE::Reference<T>::Reference")

            //clear the other pointer
            other.m_ptr = nullptr;
        }

        /**
         * @brief Destroy the Reference
         */
        ~Reference() {
            GLGE_PROFILER_SCOPE_NAMED("GLGE::Reference<T>::~Reference")

            //if a pointer is held, release the reference
            if (m_ptr) {
                m_ptr->release_ref(); 
                m_ptr = nullptr;
            }
        }

        /**
         * @brief the copy asign operator
         * 
         * @param other a constant reference to the reference to copy
         * @return `Reference&` a reference to this after copying
         */
        Reference& operator=(const Reference& other) noexcept {
            GLGE_PROFILER_SCOPE_NAMED("GLGE::Reference<T>::operator=")

            //don't copy to self
            if (this != &other) 
            {reset(other.m_ptr);}
            //return a reference to self
            return *this;
        }

        /**
         * @brief the copy move operator
         * 
         * @param other a move reference to the reference to move
         * @return `Reference&` a reference to this after moving
         */
        Reference& operator=(Reference&& other) noexcept {
            GLGE_PROFILER_SCOPE_NAMED("GLGE::Reference<T>::operator=")

            //don't move to self
            if (this != &other) {
                //if a pointer exists, release the reference
                if (m_ptr) 
                {m_ptr->release_ref();}
                //then, store the new one and clear other
                m_ptr = other.m_ptr;
                other.m_ptr = nullptr;
            }
            //return a reference to this
            return *this;
        }

        /**
         * @brief get the raw pointer
         * 
         * @return `T*` the stored pointer
         */
        T* get() const noexcept
        {return m_ptr;}

        /**
         * @brief dereference this reference
         * 
         * @return `T&` a reference to the stored value
         */
        T& operator*() const noexcept
        {return m_ptr;}

        /**
         * @brief get the stored pointer
         * 
         * @return `T*` the stored pointer
         */
        T* operator->() const noexcept
        {return m_ptr;}

    private:

        /**
         * @brief reset the reference
         * 
         * @param p a pointer to the instance to reset to
         */
        void reset(T* p = nullptr) noexcept {
            //if a pointer is held, release it
            if (m_ptr)
            {m_ptr->release_ref();}
            //then, store the new pointer
            m_ptr = p;
            //if the new pointer is valid, increment the reference
            if (m_ptr)
            {m_ptr->add_ref();}
        }

        /**
         * @brief store the pointer to reference count
         */
        T* m_ptr = nullptr;

    };

    /**
     * @brief create a new reference
     * 
     * @tparam T the type to reference
     * @tparam Args the argument types for the constructor
     * @param args the arguments for the constructor
     * @return `Reference<T>` the new reference 
     */
    template <typename T, typename... Args>
    requires std::is_base_of_v<Referable, T>
    Reference<T> make_reference(Args&&... args) 
    {return Reference<T>(new T(std::forward<Args>(args)...), false);}

}

#endif