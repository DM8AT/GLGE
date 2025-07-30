/**
 * @file GLGEObject.h
 * @author DM8AT
 * @brief define objects for GLGE
 * @version 0.1
 * @date 2025-04-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_OBJECT_
#define _GLGE_OBJECT_

//include transforms
#include "GLGETransform.h"
//include files
#include "../Files/GLGEFile.h"
//include instances
#include "../GLGEInstance.h"

//check for C++
#if GLGE_CPP

/**
 * @brief say that object attatchables will be defined later
 */
class ObjectAttatchable;

/**
 * @brief define a simple object
 */
class Object : public InstAttachableClass
{
public:

    /**
     * @brief Construct a new Object
     */
    Object() = default;

    /**
     * @brief Construct a new Object
     * 
     * @param name the name of the object
     * @param transform the transform of the object
     * @param children the children for the object or 0
     * @param childCount the amount of childs. Must be 0 if children is 0. 
     * @param instance the instance the object will belong to
     */
    Object(const std::string_view& name, Transform transform, Object** children, uint64_t childCount, Instance& instance);

    /**
     * @brief Destroy the Object
     */
    ~Object();

    /**
     * @brief get if the object is parent of a specific child
     * 
     * @param name the name of the child to check
     * @return true : the object is parent of a child named name
     * @return false : the object is not a parent of a child named name
     */
    inline bool contains(const std::string_view& name) const noexcept {return (m_children.find(name) != m_children.end());}

    /**
     * @brief get if the object is parent of another object
     * 
     * @param object the object to check
     * @return true : the object is parent of this object
     * @return false : the object is not parent of this object
     */
    inline bool isParentOf(const Object& object) const noexcept
    {
        //get the position of the element
        const auto& pos = m_children.find(object.m_name);
        //if the position is not valid, return false
        if (pos == m_children.end()) {return false;}
        //else, return if the requested element is stored there
        return (pos->second == &object);
    }

    /**
     * @brief check if the object can be a child of this object
     * The object can not be a child if any of the following cases are met:
     * - the new object is the parent of this object
     * - the new object contains a parent of this object
     * 
     * @param object the object to check 
     * @return true : the object can be a child of this object
     * @return false : the object can not be a child of this object
     */
    bool canBeChild(const Object& object);

    /**
     * @brief add a child to this object
     * 
     * @param object the child to add
     * @return true : the object was added successfully
     * @return false : failed to add the child object
     */
    bool addChild(Object& object);

    /**
     * @brief remove a child object
     * 
     * @param child the child to remove
     */
    void removeChild(Object& child);

    /**
     * @brief remove a child by its name
     * 
     * @param name the name of the child to remove
     */
    void removeChildNamed(const std::string_view& name);

    /**
     * @brief Get all children the object owns
     * 
     * @return const std::unordered_map<std::string_view, Object*> all bound children
     */
    const std::unordered_map<std::string_view, Object*>& getChildren() const noexcept {return m_children;}

    /**
     * @brief Get the parent of the object
     * 
     * @return Object* the parent of the object
     */
    Object* getParent() const noexcept {return m_parent;}

    /**
     * @brief print the object to an output stream
     * 
     * @param os the output stream to print to
     * @param o the object to print
     * @return std::ostream& the filled output stream
     */
    friend std::ostream& operator<<(std::ostream& os, const Object& o);

    /**
     * @brief Set the position of the object
     * 
     * @param pos the new position of the object
     */
    inline void setPos(const vec3& pos) noexcept {m_transform.pos = pos; m_updateFlags |= UPDATE_MOVED;}

    /**
     * @brief Get the position of the object
     * 
     * @return const vec3& the position of the object
     */
    inline const vec3& getPos() const noexcept {return m_transform.pos;}

    /**
     * @brief Set the rotation of the object
     * 
     * @param rot the new rotation of the object
     */
    inline void setRot(const Rotation& rot) noexcept {m_transform.rot = rot; m_updateFlags |= UPDATE_ROTATED;}

    /**
     * @brief Get the rotation of the object
     * 
     * @return const Rotation& the rotation of the object
     */
    inline const Rotation& getRot() const noexcept {return m_transform.rot;}

    /**
     * @brief Set the scale of the object
     * 
     * @param scale the new scale of the object
     */
    inline void setScale(const vec3& scale) noexcept {m_transform.scale = scale; m_updateFlags |= UPDATE_SCALED;}

    /**
     * @brief Get the scale of the object
     * 
     * @return const vec3& the scale of the object
     */
    inline const vec3& getScale() const noexcept {return m_transform.scale;}

    /**
     * @brief Set the transform of the object
     * 
     * @param transform the new transform of the object
     */
    inline void setTransform(const Transform& transform) noexcept {m_transform = transform; m_updateFlags |= (UPDATE_MOVED | UPDATE_ROTATED | UPDATE_SCALED);}

    /**
     * @brief Get the transform of the object
     * 
     * @return const Transform& the transform of the object
     */
    inline const Transform& getTransform() const noexcept {return m_transform;}

    /**
     * @brief get a reference to the transform of the object
     * 
     * @return Transform& a reference to the transform
     */
    inline Transform& transform() noexcept {return m_transform;}

    /**
     * @brief Get the name of the object
     * 
     * @return const std::string_view& the name of the object
     */
    inline const std::string_view& getName() const noexcept {return m_name;}

    /**
     * @brief add a new attatchment to the object
     * 
     * @param attatchment the attatchment to add
     */
    void addAttatchment(ObjectAttatchable* attatchment);

    /**
     * @brief remove an attatchment from the object
     * 
     * @param attatchment the attatchment to remove
     */
    void removeAttatchment(ObjectAttatchable* attatchment);

    /**
     * @brief Get the amount of bound attatchments
     * 
     * @return uint64_t the amount of bound attatchments
     */
    inline uint64_t getAttatchmentCount() const noexcept {return m_attatchments.size();}

    /**
     * @brief Get one specific attatchment
     * @warning if the index is greater or equal to the currently attatchet attachment count this function will result in undefined behaviour
     * 
     * @param index the index of the attatchment to querr
     * @return ObjectAttatchable* a pointer to the attatchment
     */
    inline ObjectAttatchable* getAttatchment(uint64_t index) const noexcept {return m_attatchments[index];}

    /**
     * @brief Get all attatchments attatched to the object
     * 
     * @return const std::vector<ObjectAttatchable*>& all attatchments the object owns
     */
    const std::vector<ObjectAttatchable*>& getAttatchments() const noexcept {return m_attatchments;}

    /**
     * @brief update the object
     * 
     * @return true : always returned
     * @return false : never returned
     */
    virtual bool onUpdate() override;

    /**
     * @brief Get the First object attatchable that has a specific type name
     * 
     * @param typeName the type name of the element to find
     * @return ObjectAttatchable* a pointer to the object attatchable or 0 if it was not found
     */
    ObjectAttatchable* getFirstOfTypeName(const char* typeName) const noexcept;

    /**
     * @brief Get the Nth object attatchable by its type name
     * 
     * @param typeName the type name of the element to find
     * @param elementIndex specify the how manyth element to find
     * @return ObjectAttatchable* a pointer to the object attatchable or 0 if it was not found
     */
    ObjectAttatchable* getNthOfTypeName(const char* typeName, uint64_t elementIndex) const noexcept;

    /**
     * @brief Get the last element that has a specific type name
     * 
     * @param typeName the type name to find
     * @return ObjectAttatchable* a pointer to the object attatchable or 0 if it was not found
     */
    ObjectAttatchable* getLastOfTypeName(const char* typeName) const noexcept;

    /**
     * @brief Get the first object attatchable that has a specific type
     * 
     * @tparam T the type of the element to get. MUST INHERITE FROM ObjectAttatchable!
     * @param example an example element to pull the string from
     * @return T* a pointer to the object
     */
    template <class T> inline T* getFirstOf(const T& example = T()) const noexcept
    {
        //return the first element found casted to the type
        return (T*)getFirstOfTypeName(example.getTypeName());
    }

    /**
     * @brief Get the nth element of a specific object attatchable class
     * 
     * @tparam T the type of the class to find. MUST INHERITE FROM ObjectAttatchable!
     * @param index the how manyth element to find
     * @param example an example element to pull the string from
     * @return T* a pointer to the object
     */
    template <class T> inline T* getNthOf(uint64_t index, const T& example = T()) const noexcept
    {
        //return the nth element found casted to the type
        return (T*)getNthOfTypeName(example.getTypeName(), index);
    }

    /**
     * @brief Get the last object attatchable that has a specific type
     * 
     * @tparam T the type of the element to get. MUST INHERITE FROM ObjectAttatchable!
     * @param example an example element to pull the string from
     * @return T* a pointer to the object
     */
    template <class T> inline T* getLastOf(const T& example = T()) const noexcept
    {
        //return the last element found casted to the type
        return (T*)getLastOfTypeName(example.getTypeName());
    }

    /**
     * @brief Get the All chilldren of the object
     * 
     * @param children a vector the children will be APPENDED to
     */
    void getAllChlidren(std::vector<Object*>& children) noexcept;

    /**
     * @brief check if the object was moved this tick
     * 
     * @return true : the object was moved this tick
     * @return false : the object was not moved this tick
     */
    inline bool didMove() const noexcept {return (m_updateFlags & UPDATE_MOVED) > 0;}

    /**
     * @brief check if the object was rotated this tick
     * 
     * @return true : the object was rotated this tick
     * @return false : the object was not rotated this tick
     */
    inline bool didRotate() const noexcept {return (m_updateFlags & UPDATE_ROTATED) > 0;}

    /**
     * @brief check if the object was scaled this tick
     * 
     * @return true : the object was scaled this tick
     * @return false : the object was not scaled this tick
     */
    inline bool wasScaled() const noexcept {return (m_updateFlags & UPDATE_SCALED) > 0;}

protected:
    
    /**
     * @brief store all attatched children. The name serves to identify them. 
     */
    std::unordered_map<std::string_view, Object*> m_children;
    /**
     * @brief store a pointer to the parent of 0 if this is a root object
     */
    Object* m_parent = 0;
    /**
     * @brief store the own transformation
     */
    Transform m_transform;

    /**
     * @brief store the byte index for the flag to store if the object was moved this tick
     */
    static uint8_t UPDATE_MOVED;
    /**
     * @brief store the byte index for the flag to store if the object was rotated this tick
     */
    static uint8_t UPDATE_ROTATED;
    /**
     * @brief store the byte index for the flag to store if the object was scaled this tick
     */
    static uint8_t UPDATE_SCALED;
    /**
     * @brief store the flags for update
     * At start, all flags are set to true because nothing is initalized to the inital values
     */
    uint8_t m_updateFlags = UPDATE_MOVED | UPDATE_ROTATED | UPDATE_SCALED;
    
    /**
     * @brief store all object attatchments
     */
    std::vector<ObjectAttatchable*> m_attatchments;

};

/**
 * @brief define a class that can be added to an object
 */
class ObjectAttatchable
{
public:

    /**
     * @brief Construct a new Object Attatchable
     */
    ObjectAttatchable() = default;

    /**
     * @brief Construct a new Object Attatchable
     * 
     * @param object the object the attatchable will belong to
     */
    ObjectAttatchable(Object* object) {if (!m_object) {object->addAttatchment(this);}}

    /**
     * @brief Destroy the Object Attatchable
     */
    virtual ~ObjectAttatchable() {if (m_object) {m_object->removeAttatchment(this);}}

    /**
     * @brief update the attatchment
     * Do transformations of the object here
     */
    virtual void onUpdate() noexcept {}

    /**
     * @brief a update function that runs after all update functions
     * Not used for transformations, the update flags should be set correctly
     */
    virtual void onLateUpdate() noexcept {}

    //state that the object class is a friend
    friend class Object;

    /**
     * @brief Get the Type Name
     * @warning MUST BE OVERLOADED!
     * 
     * @return const char* the constant type name
     */
    virtual const char* getTypeName() const noexcept {return "UNDEFINED";}

    /**
     * @brief print the object attatchable pointer to the console
     * 
     * @param os the output stream to print to
     * @param a the attatchable to print
     * @return std::ostream& the filled output stream
     */
    inline friend std::ostream& operator<<(std::ostream& os, const ObjectAttatchable* a) {return os << "objectAttatchable{type: " << a->getTypeName() << "}";}

    /**
     * @brief Get the Object the attatchable is attatched to or 0 if it is attatched to nothing
     * 
     * @return Object* a pointer to the object the attatchment is attatched to or 0 if it isn't attatched to anything
     */
    inline Object* getObject() const noexcept {return m_object;}

protected:

    /**
     * @brief a function that is called when the object is attatched to an object
     */
    virtual void onAttatch() noexcept {}

    /**
     * @brief a function that is called when the object is removed from an object
     */
    virtual void onRemove() noexcept {}

    /**
     * @brief store the parent object (0 means uninitalized)
     */
    Object* m_object = 0;

};

#endif //C++ section

#endif