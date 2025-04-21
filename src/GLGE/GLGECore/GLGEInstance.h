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
//include a logger
#include "Logging/GLGELogger.h"
//include the time
#include "Time/GLGETime.h"
//include limiter
#include "Time/GLGELimiter.h"

//check if this is C++
#if GLGE_CPP

//check if graphics should be included
#if GLGE_INCLUDE_GRAPHICS
//say that graphic instances will be defined later
class GraphicInstance;
//say that windows will be defined later
class Window;
#endif //Graphic API include

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
    Instance(std::string_view name, APIs api);

    /**
     * @brief Destroy the Instance
     */
    ~Instance() noexcept;

    /**
     * @brief Set the logger for the instance
     * 
     * @param logger a pointer to a logger or a class that publicly inherits from the logger base class
     */
    inline void setLogger(Logger* logger) noexcept {m_logger = logger;}

    /**
     * @brief Get the logger used by the instance
     * 
     * @return Logger* a pointer to the logger
     */
    inline Logger* getLogger() noexcept {return m_logger;}

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
     * @brief log a message if a logger exists
     * 
     * @param args the arguments for the message creation
     */
    template<typename... Args> inline void log(Args&&... args) noexcept {if (m_logger) {m_logger->log((args)...);}}

    /**
     * @brief log a message if a logger exists and debug mode is enabled
     * 
     * @param args the arguments for the message creation
     */
    template<typename... Args> inline void logDebug(Args&&... args) noexcept {if (m_logger) {if (m_logger->isDebug()) {m_logger->log((args)...);}}}

    /**
     * @brief print the instance into the default output
     * 
     * @param os a refernece to the output stream to print into
     * @param i a constant reference to the instance to print
     * @return std::ostream& a reference to the filled output stream
     */
    inline friend std::ostream& operator<<(std::ostream& os, const Instance& i) {return os << "instance{name: \"" << i.m_name << "\", api: " << i.m_api << ", current elements: " << i.m_elements << "}";}

    /**
     * @brief add a new object to the class
     * 
     * @param element a pointer to the instance to add
     */
    void addElement(InstAttachableClass* element) noexcept;

    /**
     * @brief remove a class instance from the instance
     * 
     * @param element a pointer to the instance to remove
     */
    void removeElement(InstAttachableClass* element) noexcept;

    /**
     * @brief check if the instance is active
     * 
     * @return true : the instance is active
     * @return false : the instance is not active
     */
    inline bool isActive() const noexcept {return m_active;}

    /**
     * @brief Get all elements that are attatched to the instance
     * @warning DO ONLY USE IF YOU KNOW WHAT YOU ARE DOING
     * 
     * @return std::vector<InstAttachableClass*>& a reference to the vector containing all the elements
     */
    inline std::vector<InstAttachableClass*>& getAttatchedElements() {return m_elements;}

    /**
     * @brief get a reference to the update limiter
     * 
     * @return Limiter& a reference to the limiter for the update thread
     */
    inline Limiter& updateLimiter() noexcept {return m_updateLimiter;}

    //check if graphics should be included
    #if GLGE_INCLUDE_GRAPHICS

    /**
     * @brief Set the Graphic Instance
     * @warning ONLY USE IF YOU KNOW WHAT YOU ARE DOING
     * 
     * @param gInstance a pointer to the new graphic instance
     */
    void setGraphicInstance(GraphicInstance* gInstance) noexcept {m_gInstance = gInstance;}

    /**
     * @brief Get the Graphic Instance
     * 
     * @return GraphicInstance* a pointer to the graphic instance
     */
    GraphicInstance* getGraphicInstance() noexcept {return m_gInstance;}

    /**
     * @brief initalize the graphic API
     * 
     * @param window a pointer to the window to create from
     */
    void initGraphicAPI(Window* window);
    /**
     * @brief close the graphic API
     */
    void closeGraphiAPI();

    /**
     * @brief get the SLD2 limiter
     * 
     * @return Limiter& the limiter for the SDL2 thread
     */
    static inline Limiter& getSDLLimiter() noexcept {return m_sdlLimiter;}

    #endif //Graphic only section

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
     * @brief store if the instance is active
     */
    bool m_active = true;

    /**
     * @brief store the own update thread
     */
    std::thread m_updateThread;

    /**
     * @brief store the own logger, 0 = no logging
     */
    Logger* m_logger = 0;

    /**
     * @brief store the limiter for the update thread
     */
    Limiter m_updateLimiter;

    /**
     * @brief store all elements that are attatched to the class
     */
    std::vector<InstAttachableClass*> m_elements;

    //check if graphics should be included
    #if GLGE_INCLUDE_GRAPHICS
    /**
     * @brief store a pointer to the graphic instance
     */
    GraphicInstance* m_gInstance = 0;

    /**
     * @brief store the SDL limiter
     */
    static Limiter m_sdlLimiter;

    #endif //Graphic only section
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
     * @param name the name of the instance attatchable
     */
    inline InstAttachableClass(Instance* instance, const std::string_view& name) : m_instance(instance), m_name(name) {m_instance->addElement(this);}

    /**
     * @brief Destroy the instance
     */
    inline virtual ~InstAttachableClass() {if (!m_instance) {return;} m_instance->removeElement(this);m_instance = 0;}

    /**
     * @brief print an instance of this class into an output stream
     * 
     * @param os the output stream to print into
     * @param iac the instance to print
     * @return std::ostream& a reference to the output stream with the new content
     */
    inline friend std::ostream& operator<<(std::ostream& os, const InstAttachableClass& iac) noexcept {return os << "InstAttachableClass{this: " << &iac << ", name: \"" << iac.m_name << "\", instance: " << iac.m_instance << ", instance name: \"" << iac.m_instance->getName() << "\"}";}

    /**
     * @brief update the attatched object
     * 
     * @return true : the element still exists
     * @return false : the element no longer exists
     */
    virtual bool onUpdate() {return true;}

protected:

    /**
     * @brief store the instance the class belongs to
     */
    Instance* m_instance = 0;
    /**
     * @brief store the name of the instance attatchable
     */
    std::string_view m_name = "";

};

#endif

#endif