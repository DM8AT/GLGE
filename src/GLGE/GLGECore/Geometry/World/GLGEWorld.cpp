/**
 * @file GLGEWorld.cpp
 * @author DM8AT
 * @brief implement the world class for GLGE
 * @version 0.1
 * @date 2025-04-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//incldue the world
#include "GLGEWorld.h"

/**
 * @brief Construct a new World
 * 
 * @param objects an array of object pointern for the world
 * @param objectCount the amount of object pointer stored in the array
 * @param name the name of the world
 * @param instance the instance the world will belong to
 */
World::World(Object** objects, uint64_t objectCount, const std::string_view& name, Instance& instance)
 : InstAttachableClass(&instance, name)
{
    //allocate the needed RAM
    m_objects.reserve(objectCount);
    //iterate over all objects
    for (uint64_t i = 0; i < objectCount; ++i)
    {
        //check if the object can be added
        if (!canBeAdded(*(objects[i])))
        {
            //throw an error
            m_instance->log("Can not add an object with the same name twice to the world", MESSAGE_TYPE_ERROR);
            return;
        }
        //store the object
        m_objects[objects[i]->getName()] = objects[i];
    }
}

/**
 * @brief Construct a new World
 * 
 * @param fileName the name of the file to load the world from
 * @param instance the instance the world will belong to
 * @param fileSuffix the suffix of the file to load from. Default is .gw (GLGE World)
 */
World::World(const std::string_view fileName, Instance& instance, const std::string_view& fileSuffix)
{

}

/**
 * @brief check if the object can be added
 * 
 * Prerequesits for being able to add:
 *  - The object is not in the world
 *  - The object is not in the parent / child chane of one of the object that is contained in the world
 * 
 * @param object the object to check
 * @return true : the object can be added
 * @return false : the object cannot be added
 */
bool World::canBeAdded(const Object& object) const noexcept
{
    //check if the object is directly contained, if it is, it cannot be added
    if (contains(object.getName())) {return false;}

    //iterate over all objects
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
    {
        //check if the object could be added to the current selected object, if it cannot, return false
        if (!it->second->canBeChild(object)) {return false;}

        //store the current parent
        Object* parent = 0;
        //do this while the parent is not 0
        do {
            //get the parent of the object
            Object* parent = it->second->getParent();
            //check if the parent is the object. If it is, return false
            if (parent == &object) {return false;}
        } while (parent);
    }
    //if all tests are passed, return true
    return true;
}

/**
 * @brief check if the world contains a specific object
 * 
 * @param object the object to check 
 * @return true : the world contains this object
 * @return false : the world does not contain this object
 */
bool World::contains(const Object& object) const noexcept
{
    //iterate over all objects
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
    {
        //check if this is the object
        if (it->second == &object)
        {
            //return that the object is contained
            return true;
        }
    }
    //if this point is reached, the object is not contained
    return false;
}

/**
 * @brief add a object to the world
 * 
 * @param object the object to add
 * @return true : the object was added to the world
 * @return false ; the object could not be added
 */
bool World::addObject(const Object& object) noexcept
{
    //check if the object can be added
    if (canBeAdded(object)) 
    {
        //print an error
        m_instance->log("Cannot attatch an object that has the same name as an allready attatched object", MESSAGE_TYPE_ERROR);
        //return that the adding failed
        return false;
    }
    //else, store the object
    m_objects[object.getName()] = (Object*)&object;
    //return that the adding was successfull
    return true;
}

/**
 * @brief remove an object from the world
 * 
 * @param object the object to remove
 */
void World::removeObject(const Object& object) noexcept
{
    //check if the object is contained
    if (contains(object.getName()))
    {
        //if it is contained, remove it
        m_objects.erase(object.getName());
    }
}

/**
 * @brief Get an object from the world by its name
 * 
 * @param name the name of the object to get
 * @return Object* a pointer to the object or 0 if the object was not found
 */
Object* World::getObject(const std::string_view& name) const noexcept
{
    //search the object
    auto it = m_objects.find(name);
    //return the pointer of found and 0 if not
    return ((it == m_objects.end()) ? 0 : it->second);
}

/**
 * @brief add a new world extension to this world
 * 
 * @param extension a pointer to the extension to add
 */
void World::addExtension(WorldExtension* extension) noexcept
{
    //check if the extension exists
    if (getIndexOfExtension(extension) == UINT64_MAX)
    {
        //add the extension
        m_extensions.push_back(extension);
        //store the world
        extension->m_world = this;
        //call the attatch function
        extension->onAttatch();
        return;
    }
    //else, log an error
    m_instance->log("Failed to add an extension because the extension allready exists", MESSAGE_TYPE_ERROR);
    return;
}

/**
 * @brief remove an extension from this world
 * 
 * @param extension a pointer to the extension to remove
 */
void World::removeExtension(WorldExtension* extension) noexcept
{
    //get the extension index
    uint64_t idx = getIndexOfExtension(extension);
    //if the index is UINT64_MAX, return
    if (idx == UINT64_MAX) {return;}
    //call the detatch function
    m_extensions[idx]->onDetatch();
    //set the world to 0
    m_extensions[idx]->m_world = 0;
    //else, erase the specified point
    m_extensions.erase(m_extensions.begin() + idx);
}

/**
 * @brief Get the Index Of an attatched Extension
 * 
 * @param extension the pointer to the extension
 * @return uint64_t the index of that extension or UINT64_MAX if the extension was not found
 */
uint64_t World::getIndexOfExtension(WorldExtension* extension) const noexcept
{
    //iterate over all extensions
    for (uint64_t i = 0; i < m_extensions.size(); ++i)
    {
        //check if this is the extension
        if (m_extensions[i] == extension)
        {
            //return the index
            return i;
        }
    }
    //if this point is reached, the extension was not found
    return UINT64_MAX;
}

bool World::onUpdate() noexcept
{
    //iterate over all extensions
    for (size_t i = 0; i < m_extensions.size(); ++i)
    {
        //tick the extension
        m_extensions[i]->onTick();
    }

    //always return true
    return true;
}

std::ostream& operator<<(std::ostream& os, const World& w) noexcept
{
    //print the header
    os << "world{name: " << w.m_name << ", objects: {\n";
    //iterate over all objects
    for (auto it = w.m_objects.begin(); it != w.m_objects.end(); ++it)
    {
        //print the object
        os << *it->second;
        //print a comma if this is not the last element
        if (it != w.m_objects.end()) {os << ",\n";}
    }
    //print the closing
    os << "},\n";
    //print the amount of extensions, close up and return
    return (os << "extensionCount: " << w.m_extensions.size() << "}");
}