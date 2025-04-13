/**
 * @file GLGELogger.h
 * @author DM8AT
 * @brief implement a logger that can be used together with an instance to customize GLGEs logging activity
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_LOGGER_
#define _GLGE_LOGGER_

//include the settings
#include "../GLGESettings.h"

//include messages
#include "GLGEMessage.h"
//check if this is C++
#if GLGE_CPP

/**
 * @brief the default logger class for GLGE, CAN BE USED FOR INHERITED
 */
class Logger
{
public:

    /**
     * @brief Construct a new Logger
     * 
     * @param debugging true : debug logging will be enabled | false : no debug logging will occour
     */
    Logger(bool debugging = false) : m_debug(debugging) {}

    /**
     * @brief Destroy the Logger
     */
    virtual ~Logger() {}

    /**
     * @brief log a message
     * 
     * @param args the arguments to create the message
     */
    template<typename... Args> void log(Args&&... args) noexcept {m_messages.emplace_back((args)...);}

    /**
     * @brief Get the string corresponding to a specific message type
     * 
     * @param out the output for the type string
     * @param type the type to identify
     */
    inline static void getMessageTypeString(std::string_view& out, MessageType type)
    {
        //store the type string
        out = "[UNKNOWN]";
        //switch over the type and set the type string correctly
        switch (type)
        {
        case MESSAGE_TYPE_INFO:
            out = "[INFO]";
            break;
        case MESSAGE_TYPE_DEBUG:
            out = "[DEBUG]";
            break;
        case MESSAGE_TYPE_WARNING:
            out = "[WARNING]";
            break;
        case MESSAGE_TYPE_CRITICAL_WARNING:
            out = "[CRITICAL WARNING]";
            break;
        case MESSAGE_TYPE_ERROR:
            out = "[ERROR]";
            break;
        case MESSAGE_TYPE_FATAL_ERROR:
            out = "[FATAL ERROR]";
            break;
        
        default:
            break;
        }
    }

    /**
     * @brief print the top most message into the specified output
     */
    virtual void print()
    {
        //get the string for the message type
        std::string_view typestr;
        getMessageTypeString(typestr, m_messages[0].getMessageType());
        //output the time, then the type string and finaly the message
        std::cout << "At " << m_messages[0].getTime() << " : " << typestr << " " << m_messages[0].getMessage() << "\n" << std::flush;
    }

    /**
     * @brief print the whole log
     */
    void printAll()
    {
        //iterate over all elements
        while (m_messages.size() > 0)
        {
            //print the messages
            print();
            //erase the message
            m_messages.erase(m_messages.begin());
        }
    }

    /**
     * @brief get if debug messages should be printed
     * 
     * @return true : debug messages will be printed
     * @return false : no debug messages will be printed
     */
    inline bool isDebug() const noexcept {return m_debug;}

protected:

    /**
     * @brief specify if build-in debug messages should occour
     */
    bool m_debug = false;
    /**
     * @brief store the own messages
     */
    std::vector<Message> m_messages;

};

#endif //C++ section

#endif