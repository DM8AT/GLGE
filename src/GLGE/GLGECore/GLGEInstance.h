/**
 * @file GLGEInstance.h
 * @author DM8AT
 * @brief define the layout of an GLGE instance
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_INSTANCE_
#define _GLGE_INSTANCE_

//include the settings
#include "GLGESettings.h"
//include the APIs
#include "GraphicAPIs/GLGEApi.h"

//check if this is C++
#if GLGE_CPP

/**
 * @brief attatchable classes will be defined later
 */
class InstAttachableClass;

/**
 * @brief a instance is a class that owns all other GLGE classes that need an instance in construction
 */
class Instance
{
public:

    /**
     * @brief Construct a new Instance
     */
    Instance() = default;

    /**
     * @brief Construct a new Instance
     * 
     * @param name the name for the instance
     * @param api the API to use
     */
    Instance(std::string_view name, APIs api) : m_name(name), m_api(api) {}

    /**
     * @brief Get the name of the instance
     * 
     * @return const std::string_view& a constant reference to the name
     */
    inline const std::string_view& getName() const noexcept {return m_name;}

    /**
     * @brief get the API of the instance
     * 
     * @return APIs the API of the instance
     */
    inline APIs getAPI() const noexcept {return m_api;}

    /**
     * @brief print the instance into the default output
     * 
     * @param os a refernece to the output stream to print into
     * @param i a constant reference to the instance to print
     * @return std::ostream& a reference to the filled output stream
     */
    inline friend std::ostream& operator<<(std::ostream& os, const Instance& i) {return os << "instance{name: \"" << i.m_name << "\", api: " << i.m_api << ", current elements: " << i.m_elements << "}";}

private:

    /**
     * @brief store the name of the instance
     */
    std::string_view m_name = "";
    /**
     * @brief store the API the instance uses
     */
    APIs m_api = API_FALLBACK_ERROR;

    /**
     * @brief store all elements that are attatched to the class
     */
    std::vector<InstAttachableClass*> m_elements;
};

/**
 * @brief a simple virtual class to add objects to a GLGE instance
 */
class InstAttachableClass
{
public:

    /**
     * @brief Construct a new instance
     */
    InstAttachableClass() = default;

    /**
     * @brief Construct a new instance
     * 
     * @param instance a pointer to the GLGE instance the new instance will belong to
     */
    InstAttachableClass(Instance* instance) : m_instance(instance) {}

    /**
     * @brief Destroy the instance
     */
    virtual ~InstAttachableClass() {}

    /**
     * @brief print an instance of this class into an output stream
     * 
     * @param os the output stream to print into
     * @param iac the instance to print
     * @return std::ostream& a reference to the output stream with the new content
     */
    inline friend std::ostream& operator<<(std::ostream& os, const InstAttachableClass& iac) noexcept {return os << "InstAttachableClass{this: " << &iac << ", instance: " << iac.m_instance << ", instance name: \"" << iac.m_instance->getName() << "\"}";}

protected:

    /**
     * @brief destroy the object
     */
    virtual void onDestroy() {}

    /**
     * @brief attach the object to an instance
     */
    virtual void onAttach() {}

    /**
     * @brief store the instance the class belongs to
     */
    Instance* m_instance = 0;

};

#endif

#endif