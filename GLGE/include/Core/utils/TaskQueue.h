/**
 * @file TaskQueue.h
 * @author DM8AT
 * @brief define a queue where multiple threads may insert commands and a single thread may execute them
 * @version 0.1
 * @date 2025-12-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_CORE_TASK_QUEUE_
#define _GLGE_CORE_TASK_QUEUE_

//add tasks
#include "Task.h"
//include ring buffers
#include "RingBuffer_MP_SC.h"

//add the profiler
#include "Core/Profiler.h"

//use the library namespace
namespace GLGE {

    /**
     * @brief define a queue where tasks can be pushed from everywhere and are popped and executed from a centralized thread
     * 
     * @tparam MaxSize the maximum amount of tasks the queue may hold
     */
    template <size_t MaxSize>
    class TaskQueue : public RingBuffer_MP_SC<Task, MaxSize> {
    public:

        /**
         * @brief Construct a new Task Queue
         */
        TaskQueue() = default;

        /**
         * @brief Destroy the Task Queue
         */
        ~TaskQueue() = default;

        /**
         * @brief execute a single task from the end of the list
         * 
         * @return `true` if a task was executed, `false` otherwise
         */
        bool executeSingle() {
            GLGE_PROFILER_SCOPE();

            //consume the tail
            uint64_t pos = this->m_tail.load(std::memory_order_relaxed);
            //get the back most slot
            auto& slot = this->m_slots[pos & this->IndexMask];

            //check if the slot is ready
            if (slot.seq.load(std::memory_order_acquire) != (pos + 1))
            {return false;}

            //execute the task
            slot.t.call();

            //mark the slot as free
            slot.seq.store(pos + MaxSize, std::memory_order_release);

            //advance the tail
            this->m_tail.store(pos + 1, std::memory_order_relaxed);

            //successful pop
            return true;
        }

        /**
         * @brief execute all tasks from the queue
         */
        void execute() {
            GLGE_PROFILER_SCOPE();

            //execute a single task until none is left
            while (executeSingle()) {}
        }

    };

    //old implementation
    //left here for safety reasons as i do not trust the new one. 
    #if 0
    /**
     * @brief define a heap-free, lock free ring-buffer that holds tasks to execute that could be added from everywhere and executed from a central position
     * 
     * @tparam MaxSize the maximum amount of tasks the queue may hold
     */
    template <size_t MaxSize>
    class TaskQueue {
    public:

        //for performance max size must be a power of two
        static_assert((MaxSize & (MaxSize - 1)) == 0, "For performance reasons the maximum size of a task queue must be a power of two (1, 2, 4, 8, 16, 32, ...)");

        /**
         * @brief Construct a new Task Queue
         */
        TaskQueue() {
            //prepare all the queue slots
            for (size_t i = 0; i < MaxSize; ++i) 
            {m_slots[i].seq.store(i, std::memory_order_relaxed);}
        }

        /**
         * @brief Construct a new Task Queue
         * 
         * Explicitly deleted
         */
        TaskQueue(const TaskQueue&) = delete;
        /**
         * @brief Copy assignment
         * 
         * Explicitly deleted
         * 
         * @return `TaskQueue&` a reference to the task queue
         */
        TaskQueue& operator=(const TaskQueue&) = delete;
        /**
         * @brief Construct a new Task Queue
         * 
         * Explicitly deleted
         */
        TaskQueue(TaskQueue&&) = delete;
        /**
         * @brief Move assignment
         * 
         * Explicitly deleted
         * 
         * @return `TaskQueue&` a reference to the task queue
         */
        TaskQueue& operator=(TaskQueue&&) = delete;

        /**
         * @brief check if the task queue is full
         * 
         * @return `true` if the queue is marked as full, `false` otherwise
         */
        inline bool isFull() const noexcept
        {return ((m_head.load(std::memory_order_acquire) + 1) & IndexMask) == (m_tail.load(std::memory_order_acquire) & IndexMask);}

        /**
         * @brief check if the task queue is empty
         * 
         * @return `true` if the task queue is empty, `false` otherwise
         */
        inline bool isEmpty() const noexcept
        {return (m_head.load(std::memory_order_acquire) & IndexMask) == (m_tail.load(std::memory_order_acquire) & IndexMask);}

        /**
         * @brief Get the amount of queued tasks
         * 
         * @return u64 the amount of currently queued tasks
         */
        inline u64 getTaskCount() const noexcept {
            //pre-fetch the head and tail to prevent later memory fetches
            u64 head = m_head.load(std::memory_order_acquire) & IndexMask;
            u64 tail = m_tail.load(std::memory_order_acquire) & IndexMask;
            //return the size with correct respect to wrap-arounds
            return head - tail;
        }

        /**
         * @brief push a new task to the queue
         * 
         * @param task a constant reference to the task to add for execution
         * 
         * @return `true` when the task was successfully pushed, `false` otherwise
         */
        bool push(const Task& task) noexcept {
            //get the next position
            uint64_t pos = m_head.fetch_add(1, std::memory_order_acq_rel);
            //get the queue slot
            QueueSlot& slot = m_slots[pos & IndexMask];

            //if the task is full, stop
            if (slot.seq.load(std::memory_order_acquire) != pos)
            {return false;}

            //push the task to the queue
            slot.task = task;
            //set the sequence to mark in use
            slot.seq.store(pos + 1, std::memory_order_release);

            //successful push
            return true;
        }

        /**
         * @brief execute a single task from the tail of the queue
         * 
         * @return `true` if a task was popped, `false` otherwise
         */
        bool pop() noexcept {
            //consume the tail
            uint64_t pos = m_tail.load(std::memory_order_relaxed);
            //get the back most slot
            QueueSlot& slot = m_slots[pos & IndexMask];

            //check if the slot is ready
            if (slot.seq.load(std::memory_order_acquire) != (pos + 1))
            {return false;}

            //execute the task
            slot.task.call();

            //mark the slot as free
            slot.seq.store(pos + MaxSize, std::memory_order_release);

            //advance the tail
            m_tail.store(pos + 1, std::memory_order_relaxed);

            //successful pop
            return true;
        }

        /**
         * @brief execute all tasks starting from the queues end until the head is reached
         */
        void execute() noexcept {
            //pop until the queue is empty
            while (pop()) {}
        }

    protected:

        /**
         * @brief store the structure of a single queue slot
         */
        struct QueueSlot {
            /**
             * @brief store the task for the queue slot
             */
            Task task;
            /**
             * @brief store the sequence number (UINT64_MAX means that the slot is unused (it is very unlikely to be a valid index) )
             */
            alignas(alignof(u64)) std::atomic_uint64_t seq{UINT64_MAX};
            /**
             * @brief add padding for memory read speed optimizations (padds the size to always be 128 bytes)
             */
            u8 padding[(192) - (sizeof(task) + sizeof(seq))] = { 0 };
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
    #endif

}

#endif