/**
 * @file Exception.h
 * @author DM8AT
 * @brief define a class that is used as a base exception class for GLGE
 * @version 0.1
 * @date 2025-12-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef _GLGE_CORE_EXCEPTION_
#define _GLGE_CORE_EXCEPTION_

//add common stuff
#include "Common.h"
//add exceptions
#include <exception>

//use the library namespace
namespace GLGE {

    /**
     * @brief an override for the default exception class
     */
    class Exception : public std::exception {
    public:

        /**
         * @brief Construct a new Exception
         * 
         * @param msg the raw message describing the error that occurred
         * @param where information on where the error occurred
         */
        Exception(const std::string& msg, const std::string& where)
           //store the message as a combination of the raw message and the where information
         : m_msg(std::string("\"") + msg + "\" at: " + where)
        {}

        /**
         * @brief get information about the error
         * 
         * @return const char* a constant string defining the error that occurred
         */
        const char* what() const noexcept override
        {return m_msg.c_str();}

    protected:

        /**
         * @brief store the message
         */
        std::string m_msg = "";

    };

}

#endif