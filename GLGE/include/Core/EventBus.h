/**
 * @file EventBus.h
 * @author DM8AT
 * @brief define a system where events can be parsed into and subscribed to
 * @version 0.1
 * @date 2025-12-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_CORE_EVENT_BUS_
#define _GLGE_CORE_EVENT_BUS_

//include events + common + type info
#include "Event.h"

//use the library namespace
namespace GLGE {

    /**
     * @brief a event system 
     */
    class EventBus {
    public:

        /**
         * @brief define the expected layout of a callback function
         */
        using Callback = void (*)(const Event&);

        /**
         * @brief Construct a new Event Bus
         */
        EventBus() = default;

        /**
         * @brief subscribe a function for a specific event
         * 
         * @param id the ID of the event to listen to
         * @param callback the callback to call
         */
        inline void subscribe(u64 id, Callback callback) {
            //store the subscriber call function
            m_subscribers[id].emplace_back(callback);
        }

        /**
         * @brief subscribe a listener function to the event bus
         * 
         * @param name the name to determine the ID from. The ID is the FNV 1A 64 bit hash of the name. 
         * @param cb the callable to call
         */
        inline void subscribe(const std::string_view& name, const Callback& cb)
        {subscribe(__FNV_1A_64_HASH(name), cb);}

        /**
         * @brief parse an event to the subscribers
         * 
         * @param event a constant reference to the event to parse
         */
        void emit(const Event& event) const {
            GLGE_PROFILER_SCOPE();

            //lookup the subscriber for the event
            auto it = m_subscribers.find(event.getID());
            //if it does not exist, stop
            if (it == m_subscribers.end()) {return;}

            //else, parse the event to all listener functions
            for (auto& cb : it->second) {cb(event);}
        }

        /**
         * @brief create and emit an event
         * 
         * @tparam Args the arguments for the event to emit
         * @param id the ID of the event to emit
         * @param args the arguments to parse to the event
         */
        template <typename... Args>
        void emit(u64 id, const Args&... args) const {
            //construct the event and emit it
            emit(Event::create(id, args...));
        }

        /**
         * @brief create and emit an event
         * 
         * @tparam Args the arguments for the event to emit
         * @param name the name of the event to emit. The ID is computed as the 64 bit FNV 1A hash of the string. 
         * @param args the arguments to parse to the event
         */
        template <typename... Args>
        void emit(const std::string_view& name, const Args&... args) const {
            //construct the event and emit it
            emit<Args...>(Event::create(name, args...));
        }

    protected:

        /**
         * @brief store the subscribes to the different events
         */
        std::unordered_map<u64, std::vector<Callback>> m_subscribers = {};

    };

}

#endif