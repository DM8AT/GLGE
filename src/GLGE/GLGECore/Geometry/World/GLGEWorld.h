/**
 * @file GLGEWorld.h
 * @author DM8AT
 * @brief define the layout for worlds in GLGE
 * @version 0.1
 * @date 2025-04-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_WORLD_
#define _GLGE_WORLD_

//include the settings
#include "../../GLGESettings.h"
//include instances
#include "../../GLGEInstance.h"
//include objects
#include "../GLGEObject.h"

//check if this is C++
#if GLGE_CPP

/**
 * @brief world extensions will be defined later
 */
class WorldExtension;

/**
 * @brief store the world
 * 
 * A world is a collection of objects. One of the objects may be connected to multiple worlds. 
 */
class World : public InstAttachableClass
{
public:

    /**
     * @brief Construct a new World
     */
    World() = default;

    /**
     * @brief Construct a new World
     * 
     * @param objects an array of object pointern for the world
     * @param objectCount the amount of object pointer stored in the array
     * @param name the name of the world
     * @param instance the instance the world will belong to
     */
    World(Object** objects, uint64_t objectCount, const std::string_view& name, Instance& instance);

    /**
     * @brief Construct a new World
     * 
     * @param fileName the name of the file to load the world from
     * @param instance the instance the world will belong to
     * @param fileSuffix the suffix of the file to load from. Default is .gw (GLGE World)
     */
    World(const std::string_view fileName, Instance& instance, const std::string_view& fileSuffix = ".gw");

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
    bool canBeAdded(const Object& object) const noexcept;

    /**
     * @brief check if the world contains a specific object
     * 
     * @param object the object to check 
     * @return true : the world contains this object
     * @return false : the world does not contain this object
     */
    bool contains(const Object& object) const noexcept;

    /**
     * @brief check if the world contains a specific object
     * 
     * @param name the name of the object
     * @return true : the world contains the object
     * @return false : the world does not contain the object
     */
    inline bool contains(const std::string_view& name) const noexcept
    {
        //check if the object is contained
        return (m_objects.find(name) != m_objects.end());
    }

    /**
     * @brief add a object to the world
     * 
     * @param object the object to add
     * @return true : the object was added to the world
     * @return false ; the object could not be added
     */
    bool addObject(const Object& object) noexcept;

    /**
     * @brief remove an object from the world
     * 
     * @param object the object to remove
     */
    void removeObject(const Object& object) noexcept;

    /**
     * @brief Get an object from the world by its name
     * 
     * @param name the name of the object to get
     * @return Object* a pointer to the object or 0 if the object was not found
     */
    Object* getObject(const std::string_view& name) const noexcept;

    /**
     * @brief add a new world extension to this world
     * 
     * @param extension a pointer to the extension to add
     */
    void addExtension(WorldExtension* extension) noexcept;

    /**
     * @brief remove an extension from this world
     * 
     * @param extension a pointer to the extension to remove
     */
    void removeExtension(WorldExtension* extension) noexcept;

    /**
     * @brief Get the Index Of an attatched Extension
     * 
     * @param extension the pointer to the extension
     * @return uint64_t the index of that extension or UINT64_MAX if the extension was not found
     */
    uint64_t getIndexOfExtension(WorldExtension* extension) const noexcept;

    /**
     * @brief override the update function
     * 
     * @return true : always returned
     * @return false : never returned
     */
    virtual bool onUpdate() noexcept override;

    /**
     * @brief print the world to an output stream
     * 
     * @param os the output stream to print to
     * @param world the world to print
     * @return std::ostream& the filled output stream
     */
    friend std::ostream& operator<<(std::ostream& os, const World& world) noexcept;

protected:

    /**
     * @brief store all attatched objects
     */
    std::unordered_map<std::string_view, Object*> m_objects;
    /**
     * @brief store all world extensions
     */
    std::vector<WorldExtension*> m_extensions;

};

/**
 * @brief define a class that extents the behaviour of a simple world
 */
class WorldExtension
{
public:

    /**
     * @brief Construct a new World Extension
     */
    WorldExtension() = default;

    /**
     * @brief Construct a new World Extension
     * 
     * @param world the world the extension will belong to
     */
    WorldExtension(World* world) {world->addExtension(this);}

    /**
     * @brief Destroy the World Extension
     */
    virtual ~WorldExtension() {m_world->removeExtension(this);}

    /**
     * @brief a function that is called when the extension is attatched
     */
    virtual void onAttatch() noexcept {}

    /**
     * @brief a function that is called every tick
     */
    virtual void onTick() noexcept {}

    /**
     * @brief a function that is called when the extension is detatched from the world
     */
    virtual void onDetatch() noexcept {}

    /**
     * @brief Get the Type Name object
     * 
     * @return const char* 
     */
    virtual const char* getTypeName() const noexcept {return "WorldExtension";}

    /**
     * @brief say that the world class is a friend
     */
    friend class World;

protected:

    /**
     * @brief store the world the extension belongs to
     */
    World* m_world = 0;

};

#endif //C++ section

#endif