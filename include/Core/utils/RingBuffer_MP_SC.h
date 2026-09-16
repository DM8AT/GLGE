/**
 * @file RingBuffer_MP_SC.h
 * @author DM8AT
 * @brief define a multiple producer, single consumer heap-free, lock-free ring buffer
 * @version 0.1
 * @date 2025-12-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_CORE_RING_MP_SP_
#define _GLGE_CORE_RING_MP_SP_

//add common stuff
#include "Core/Common.h"
//add math
#include <cmath>

//use the library namespace
namespace GLGE {

    /**
     * @brief a class that contains a generic ring buffer for multiple producers and a single consumer
     * 
     * @tparam T the type to store in the ring buffer
     * @tparam MaxSize the maximum amount of elements the ring buffer may contain
     */
    template <typename T, size_t MaxSize>
    class RingBuffer_MP_SC {
    public:

        //for performance max size must be a power of two
        static_assert((MaxSize & (MaxSize - 1)) == 0, "For performance reasons the maximum size of a ring buffer must be a power of two (1, 2, 4, 8, 16, 32, ...)");

        /**
         * @brief Construct a new ring buffer
         */
        RingBuffer_MP_SC() {
            //prepare all the queue slots
            for (size_t i = 0; i < MaxSize; ++i) 
            {m_slots[i].seq.store(i, std::memory_order_relaxed);}
        }

        /**
         * @brief Construct a new ring buffer
         * 
         * Explicitly deleted
         */
        RingBuffer_MP_SC(const RingBuffer_MP_SC&) = delete;
        /**
         * @brief Copy assignment
         * 
         * Explicitly deleted
         * 
         * @return `RingBuffer&` a reference to the ring buffer
         */
        RingBuffer_MP_SC& operator=(const RingBuffer_MP_SC&) = delete;
        /**
         * @brief Construct a new ring buffer
         * 
         * Explicitly deleted
         */
        RingBuffer_MP_SC(RingBuffer_MP_SC&&) = delete;
        /**
         * @brief Move assignment
         * 
         * Explicitly deleted
         * 
         * @return `RingBuffer&` a reference to the ring buffer
         */
        RingBuffer_MP_SC& operator=(RingBuffer_MP_SC&&) = delete;

        /**
         * @brief check if the ring buffer is full
         * 
         * @return `true` if the queue is marked as full, `false` otherwise
         */
        inline bool isFull() const noexcept {
            //get the next slot to push into
            const QueueSlot& slot = m_slots[m_head.load(std::memory_order_acquire) & IndexMask];
            //load the sequence of the slot
            uint64_t seq = slot.seq.load(std::memory_order_acquire);

            //the producer expects seq = head when slot is free
            return seq != m_head.load(std::memory_order_acquire);
        }

        /**
         * @brief check if the ring buffer is empty
         * 
         * @return `true` if the ring buffer is empty, `false` otherwise
         */
        inline bool isEmpty() const noexcept {
            //safe for single-consumer: empty if the next slot to consume is not ready
            const QueueSlot& slot = m_slots[m_tail.load(std::memory_order_acquire) & IndexMask];
            //read the sequence of the slot
            uint64_t seq = slot.seq.load(std::memory_order_acquire);

            //the consumer expects seq = tail + 1 when slot is ready
            return seq != (m_tail.load(std::memory_order_acquire) + 1);
        }

        /**
         * @brief Get the amount of stored elements
         * 
         * @return u64 the amount of currently stored elements
         */
        inline u64 count() const noexcept {
            //return the size with correct respect to wrap-arounds
            return (m_head.load() - m_tail.load());
        }

        /**
         * @brief push a new T to the queue
         * 
         * @param t a constant reference to the instance of T to add
         * 
         * @return `true` when the instance was successfully pushed, `false` otherwise
         */
        bool push(const T& t) noexcept {
            //store the current and next position
            uint64_t pos, next;
            //store a pointer to the selected slot
            QueueSlot* slot;
            //store the sequence value of the slot
            uint64_t seq;

            //try to obtain a slot in the queue
            while (true)
            {
                //get the position and slot currently at the back
                pos = m_head.load(std::memory_order_acquire);
                slot = &m_slots[pos & IndexMask];

                //get the sequence value of the slot
                seq = slot->seq.load(std::memory_order_acquire);

                //sanity check if the slot is currently in use
                //if it is, the queue is full
                if (seq < pos) 
                {return false;}

                //else, test if the slot is still free
                if (seq == pos)
                {
                    //compute the next position
                    next = pos + 1;

                    //try to reserve this position atomically
                    if (m_head.compare_exchange_weak(
                            pos, next,
                            std::memory_order_acq_rel,
                            std::memory_order_acquire))
                    {
                        //successfully reserved the slot
                        break; 
                    }

                    //CAS failed -> another producer won, retry
                }
                else
                {
                    //Slot sequence ahead of expected -> full queue
                    return false;
                }
            }

            //acquired the slot ownership
            slot->t = t;

            //mark slot ready for consumer
            slot->seq.store(pos + 1, std::memory_order_release);

            //success
            return true;
        }

        /**
         * @brief get a reference to the end of the list
         * 
         * @warning only valid when the ring buffer is not empty
         * 
         * @return T& a reference to the end
         */
        inline T& back() noexcept
        {return m_slots[m_tail.load(std::memory_order_relaxed) & IndexMask].t;}

        /**
         * @brief pop a single instance from the queue
         * 
         * @param back a reference to a `T` to copy the value of the last most element into
         * @return `true` if a instance was popped, `false` otherwise
         */
        bool pop(T& back) noexcept {
            //consume the tail
            uint64_t pos = m_tail.load(std::memory_order_acquire);
            //get the back most slot
            QueueSlot& slot = m_slots[pos & IndexMask];

            //check if the slot is ready
            if (slot.seq.load(std::memory_order_acquire) != (pos + 1))
            {return false;}

            //return the value
            back = slot.t;

            //mark the slot as free
            slot.seq.store(pos + MaxSize, std::memory_order_release);

            //advance the tail
            m_tail.store(pos + 1, std::memory_order_release);

            //successful pop
            return true;
        }

    protected:

        /**
         * @brief store the structure of a single queue slot
         */
        struct QueueSlot {
            /**
             * @brief the currently stored type
             */
            T t;
            /**
             * @brief store the sequence number (UINT64_MAX means that the slot is unused (it is very unlikely to be a valid index) )
             */
            alignas(alignof(u64)) std::atomic_uint64_t seq{UINT64_MAX};
            /**
             * @brief store the size of a single slot (without padding)
             */
            static constexpr size_t SlotSize = sizeof(T) + sizeof(std::atomic_uint64_t);
            /**
             * @brief store the required amount of padding to apply
             */
            static constexpr size_t Padding = (64 - (SlotSize % 64)) % 64;
            /**
             * @brief add padding for memory read speed optimizations (padds the size to always be the closest multiple of 64)
             */
            u8 padding[Padding] = { 0 };
        };

        /**
         * @brief store a mask that can be used to make the index a valid index
         */
        inline static constexpr uint64_t IndexMask = MaxSize - 1;

        /**
         * @brief store slots for the queue tasks
         */
        QueueSlot m_slots[MaxSize];
        /**
         * @brief store the position to add tasks to
         */
        alignas(alignof(u64)) std::atomic_uint64_t m_head{0};
        /**
         * @brief store the position to remove tasks from
         */
        alignas(alignof(u64)) std::atomic_uint64_t m_tail{0};

    };

}

#endif