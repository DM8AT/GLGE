/**
 * @file GLGEGraphicAPI.h
 * @author DM8AT
 * @brief define the API instance for the abstracted graphic functions
 * @version 0.1
 * @date 2025-04-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_GRAPHIC_ABSTRACT_API_
#define _GLGE_GRAPHIC_ABSTRACT_API_

//include instances
//Also includes:
// - APIs
#include "../../GLGECore/GLGEInstance.h"

//check if this is C++
#if GLGE_CPP

//graphic instance attachables will exist
class GraphicInstAttatchable;

/**
 * @brief manage the lifetime of all graphic dependend objects
 */
class GraphicInstance : public InstAttachableClass
{
public:

    /**
     * @brief Construct a new Graphic Instance
     */
    GraphicInstance() = default;

    /**
     * @brief Construct a new Graphic Instance
     * 
     * @param instance a pointer to the instance the graphic instance belongs to
     */
    GraphicInstance(Instance* instance) : InstAttachableClass(instance, "GraphicInstance") {}

    /**
     * @brief Destroy the Graphic Instance
     */
    virtual ~GraphicInstance();

    /**
     * @brief add a new object to the class
     * 
     * @param element a pointer to the instance to add
     */
    void addElement(GraphicInstAttatchable* element) noexcept;

    /**
     * @brief remove a class instance from the instance
     * 
     * @param element a pointer to the instance to remove
     */
    void removeElement(GraphicInstAttatchable* element) noexcept;

    /**
     * @brief Get all elements that are attatched to the instance
     * @warning DO ONLY USE IF YOU KNOW WHAT YOU ARE DOING
     * 
     * @return std::vector<GraphicInstAttatchable*>& a reference to the vector containing all the elements
     */
    inline std::vector<GraphicInstAttatchable*>& getAttatchedElements() {return m_elements;}

    /**
     * @brief override the update function
     * 
     * @return true : the object still exists
     * @return false : the object no longer exists
     */
    virtual bool onUpdate() noexcept override;

    /**
     * @brief Get the Instance the graphic instance belongs to
     * 
     * @return Instance* a pointer to the owning instance
     */
    inline Instance* getInstance() noexcept {return m_instance;}

protected:

    /**
     * @brief store the used graphic api
     */
    APIs m_api = API_FALLBACK_ERROR;
    /**
     * @brief store all attatched elements
     */
    std::vector<GraphicInstAttatchable*> m_elements;

};

/**
 * @brief base class for all classes that can be attatched to a graphic instance
 */
class GraphicInstAttatchable
{
public:

    /**
     * @brief Construct a new Graphic Inst Attatchable
     */
    GraphicInstAttatchable() = default;

    /**
     * @brief Construct a new Graphic Inst Attatchable
     * 
     * @param instance a pointer to the instance the graphic instance attatchable belongs to
     */
    GraphicInstAttatchable(GraphicInstance* instance) : m_graphicInstance(instance) {m_graphicInstance->addElement(this);}

    /**
     * @brief Destroy the Graphic Inst Attatchable
     */
    virtual ~GraphicInstAttatchable() {if (!m_graphicInstance) {return;} m_graphicInstance->removeElement(this); m_graphicInstance = 0;}

    /**
     * @brief print a graphic instance into an output stream
     * 
     * @param os the output stream to print into
     * @param gia the graphic instance attatchable to print
     * @return std::ostream& a reference to the filled output stream
     */
    inline friend std::ostream& operator<<(std::ostream& os, const GraphicInstAttatchable& gia) {return os << "graphicInstAttatchable{pointer: " << &gia << ", graphic instance: " << gia.m_graphicInstance << "}";}

    /**
     * @brief update the attatched object
     * 
     * @return true : the element still exists
     * @return false : the element no longer exists
     */
    virtual bool onUpdate() {return true;}

protected:

    /**
     * @brief store a pointer to the parent graphic instance
     */
    GraphicInstance* m_graphicInstance = 0;
    
};

#endif

#endif