/**
 * @file GLGEMessage.cpp
 * @author DM8AT
 * @brief implement the messages for GLGE
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the messages
#include "GLGEMessage.h"

//store the message type lookup table
std::string_view MESSAGE_TYPE_LOOKUP_TABLE[] = {
    "MESSAGE_TYPE_UNDEFINED",
    "MESSAGE_TYPE_INFO",
    "MESSAGE_TYPE_DEBUG",
    "MESSAGE_TYPE_WARNING",
    "MESSAGE_TYPE_CRITICAL_WARNING",
    "MESSAGE_TYPE_ERROR",
    "MESSAGE_TYPE_FATAL_ERROR"
};

const char* GLGE_C_FUNC(messageTypeToString)(MessageType messageType) GLGE_FUNC_NOEXCEPT
{
    //sanity check the input
    if (((uint32_t)messageType) >= (uint32_t)(sizeof(MESSAGE_TYPE_LOOKUP_TABLE) / sizeof(*MESSAGE_TYPE_LOOKUP_TABLE)))
    {
        //return 0 for out of range
        return 0;
    }
    //else, return the correct element
    return MESSAGE_TYPE_LOOKUP_TABLE[(uint32_t)messageType].data();
}