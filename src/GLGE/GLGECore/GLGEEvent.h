/**
 * @file GLGEEvent.h
 * @author DM8AT
 * @brief define an abstract event class to send system events
 * @version 0.1
 * @date 2025-06-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_EVENT_
#define _GLGE_EVENT_

//include the settings
#include "GLGESettings.h"

//define the high level event identifyer for GLGE default events

//identify that the event is from some low level data in GLGE
#define GLGE_EVENT_IDENTIFYER_LOW_LEVEL 0
//identify that the event is for an GLGE instance
#define GLGE_EVENT_IDENTIFYER_INSTANCE 1
//identify that the event is for a GLGE window (as example closing, resizing, moving)
#define GLGE_EVENT_IDENTIFYER_WINDOW 2
//identify that the event is for an input devies (as example keyboard, mouse, controller, ...)
#define GLGE_EVENT_IDENTIFYER_INPUT 3

/**
 * @brief store informaton about a single event type
 */
typedef struct s_EventIdentifyer
{
    /**
     * @brief store the identifyer of the library
     * 
     * @warning some library identifyers are used by GLGE. The following channels are used:
     *           - 0 : Low level system events, {0, 0} is reserved for NO_EVENT
     *           - 1 : Instance event bus
     *           - 2 : Window event bus
     *           - 3 : Input (Keyboard / Mouse / Controller / Gamepad) event bus
     *          Because of this all channels from 0 to 0xf are reserved for GLGE.
     *          Extensions should use the channels 0x11 and onwards. 
     */
    uint16_t library;
    /**
     * @brief store the identifyer for the event
     */
    uint32_t event;
} EventIdentifyer;

//check for C++
#if GLGE_CPP

/**
 * @brief an abstract class to overload for events
 */
class Event
{
public:

    /**
     * @brief Construct a new empty Event
     */
    Event() = default;

    /**
     * @brief Construct a new Event
     * 
     * @param name the name of the event
     * @param identifyer the identifyer of the event
     */
    Event(const std::string_view name, const EventIdentifyer identifyer)
     : m_name(name), m_identifyer(identifyer)
    {}

    /**
     * @brief Destroy the Event
     */
    virtual ~Event() {}

    /**
     * @brief Get the name of the event
     * 
     * @return const std::string_view& a constant reference to the string view storing the event's name
     */
    inline const std::string_view& getName() const noexcept {return m_name;}

    /**
     * @brief Get the Identifyer of the event
     * 
     * @return const EventIdentifyer& a constant reference to the event's identifyer
     */
    inline const EventIdentifyer& getIdentifyer() const noexcept {return m_identifyer;}

protected:

    /**
     * @brief store the name of the event
     */
    std::string_view m_name = "Event";
    /**
     * @brief store the identifyer of the event
     */
    EventIdentifyer m_identifyer = {0,0};

};

#endif //end of C++ section

#endif