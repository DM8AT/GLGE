/**
 * @file GLGEMessage.h
 * @author DM8AT
 * @brief this file defines the structure of a logger message
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_MESSAGE_
#define _GLGE_MESSAGE_

//include the settings
#include "../GLGESettings.h"
//include utility functions
#include "../GLGEUtil.h"
//include the time
#include "../Time/GLGETime.h"

/**
 * @brief define the type of a message
 */
typedef enum e_MessageType {
    /**
     * @brief say that the type of message is undefined
     */
    MESSAGE_TYPE_UNDEFINED = 0,
    /**
     * @brief say that a message is a info message
     */
    MESSAGE_TYPE_INFO,
    /**
     * @brief say that a message is a debug message
     */
    MESSAGE_TYPE_DEBUG,
    /**
     * @brief say that a message is a warning message
     */
    MESSAGE_TYPE_WARNING,
    /**
     * @brief say that a message is a critical warning message
     */
    MESSAGE_TYPE_CRITICAL_WARNING,
    /**
     * @brief say that a message is an error message
     */
    MESSAGE_TYPE_ERROR,
    /**
     * @brief say that a message is a fatal error message
     */
    MESSAGE_TYPE_FATAL_ERROR
} MessageType;

/**
 * @brief convert a message type enum to a string
 * 
 * @param messageType the message type identifyer to convert
 * @return const char* the C-String that maps to the inputed message type
 */
const char* GLGE_C_FUNC(messageTypeToString)(MessageType messageType) GLGE_FUNC_NOEXCEPT;

//check if this is C++
#if GLGE_CPP

/**
 * @brief print a message type
 * 
 * @param os the output stream to print into
 * @param mt the messae type to print
 * @return std::ostream& a reference to the filled output stream
 */
inline std::ostream& operator<<(std::ostream& os, const MessageType& mt) {return os << GLGE_C_FUNC(messageTypeToString)(mt);}

/**
 * @brief a class to structure messages
 */
class Message
{
public:

    Message() = default;

    /**
     * @brief Construct a new Message
     * 
     * @param message the content of the message
     * @param type the type of the message
     */
    Message(std::string_view message, MessageType type) : m_message(message), m_type(type), m_now(GLGE_C_FUNC(getLocalTime)()) {}

    /**
     * @brief Construct a new Message
     * 
     * @param stream the stream for the message
     * @param type the type of the message
     */
    Message(std::stringstream& stream, MessageType type) : m_message(stream.str()), m_type(type), m_now(GLGE_C_FUNC(getLocalTime)()) {}

    /**
     * @brief Get the contents of the message
     * 
     * @return const std::string_view& a constant reference to the message
     */
    const std::string& getMessage() const noexcept {return m_message;}

    /**
     * @brief Get the type of message
     * 
     * @return MessageType the type of message
     */
    MessageType getMessageType() const noexcept {return m_type;}

    /**
     * @brief Get the time the message was created
     * 
     * @return const std::chrono::system_clock::time_point& a constant reference to the time point
     */
    const Time& getTime() const noexcept {return m_now;}

    /**
     * @brief print the message to an output stream
     * 
     * @param os the output stream to print to 
     * @param m the message to print
     * @return std::ostream& a refernece to the filled output stream
     */
    inline friend std::ostream& operator<<(std::ostream& os, const Message& m) {return os << "message{message: " << m.m_message << ", type: " << m.m_type << ", " << m.m_now << "}";}

protected:

    /**
     * @brief store the contents of the message
     */
    std::string m_message = "";
    /**
     * @brief store the type of the message
     */
    MessageType m_type = MESSAGE_TYPE_UNDEFINED;
    /**
     * @brief store the current time
     */
    Time m_now = {0,0,0};
};

#endif //C++ section

#endif