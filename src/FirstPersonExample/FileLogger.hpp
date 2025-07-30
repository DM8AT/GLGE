/**
 * @file FileLogger.hpp
 * @author DM8AT
 * @brief a simple file logger for GLGE
 * @version 0.1
 * @date 2025-07-25
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//only compile once
#pragma once

//GLGE is needed for loggers
#include "../GLGE/GLGE.h"

/**
 * @brief overload the logger class so the output is written into a log file and not into the console
 */
class FileLogger : public Logger
{
public:

    /**
     * @brief Construct a new File Logger
     */
    FileLogger() = default;

    /**
     * @brief Construct a new File Logger
     * 
     * @param fileName the name of the file the data will be stored in
     * @param debug true : debug info will be printed | false : no debug info will be printed
     */
    FileLogger(std::string fileName, bool debug = false)
     : Logger(debug), m_file(Path(fileName), true) {m_file.content() = "";}

    /**
     * @brief Destroy the File Logger
     */
    ~FileLogger()
    {
        //print all the data into the file
        printAll();
        //save the file. The file will close once it runs out of scope
        m_file.save();
    }

    /**
     * @brief overload the print function to store to a file instead of printing to the console
     */
    virtual void print() override
    {
        //print the message into the file
        m_file.content() += format(m_messages[0]) + "\n";
    }

    /**
     * @brief overload the hook that says that all messages are printed
     */
    virtual void finishPrint() override
    {
        //when the printing finishes, save all changes
        m_file.save();
    }

protected:

    /**
     * @brief store the file to store the data in
     */
    File m_file;

};
