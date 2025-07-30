/**
 * @file GLGEObject.cpp
 * @author DM8AT
 * @brief implement the basic GLGE object
 * @version 0.1
 * @date 2025-04-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include objects
#include "GLGEObject.h"

Object::Object(const std::string_view& name, Transform transform, Object** children, uint64_t childCount, Instance& instance)
 : InstAttachableClass(&instance, name), m_transform(transform)
{
    //check if children are available
    if (children)
    {
        //resize the child array
        m_children.reserve(childCount);

        //iterate over all children
        for (uint64_t i = 0; i < childCount; ++i)
        {
            //add the child
            m_children[children[i]->m_name] = children[i];
            //remove the child from its old parent
            if (children[i]->m_parent) {children[i]->m_parent->removeChild(*(children[i]));}
            //set the parent of the child
            children[i]->m_parent = this;
        }
    }
}

Object::~Object()
{
    //iterate over all attatchments
    while (m_attatchments.size() > 0)
    {
        //remove the element
        removeAttatchment(m_attatchments[0]);
    }
}

bool Object::canBeChild(const Object& object)
{
    //iterate over all children
    for (auto it = m_children.begin(); it != m_children.end(); ++it)
    {
        //cehck if this is the object
        if (it->second == &object)
        {
            //return that it can't be added
            return false;
        }
        //check if the child could be added to them
        if (!it->second->canBeChild(object))
        {
            //if it can't be there, return false
            return false;
        }
    }
    //return that the object can be added
    return true;
}

bool Object::addChild(Object& object)
{
    //check if the child can be added
    if (!canBeChild(object))
    {
        //return that it can't be added
        return false;
    }
    //check if the child's name is contained
    if (contains(object.m_name))
    {
        //return false, it can't be stored
        return false;
    }

    //store the new child
    m_children[object.m_name] = &object;
    //set the parent
    object.m_parent = this;

    //return the successful add
    return true;
}

void Object::removeChild(Object& child)
{
    //find the child object
    const auto& pos = m_children.find(child.m_name);
    //stop if the element is not the correct one
    if (pos->second != &child) {return;}
    //return if the child was not found
    if (pos == m_children.end()) {return;}
    //else, set the parent of the chiled to 0
    pos->second->m_parent = 0;
    //remove the element
    m_children.erase(pos);
}

void Object::removeChildNamed(const std::string_view& name)
{
    //find the child object
    const auto& pos = m_children.find(name);
    //return if the child was not found
    if (pos == m_children.end()) {return;}
    //else, set the parent of the chiled to 0
    pos->second->m_parent = 0;
    //remove the element
    m_children.erase(pos);
}

std::ostream& operator<<(std::ostream& os, const Object& o)
{
    //add the start
    os << "object{name: \"" << o.m_name << "\", transform: " << o.m_transform << ", children: {";
    //check if children exist, add a new line
    if (o.m_children.size() > 0) {os << "\n";}
    //iterate over all children
    for (auto it = o.m_children.begin(); it != o.m_children.end(); ++it)
    {
        //print the child
        os << *it->second << "\n";
    }
    //print the ending and return
    return os << "}";
}


void Object::addAttatchment(ObjectAttatchable* attatchment)
{
    //stop if the attatchment allready exists
    if (std::find(m_attatchments.begin(), m_attatchments.end(), attatchment) != m_attatchments.end()) {return;}
    //store the new attatchment
    m_attatchments.push_back(attatchment);
    //set the object
    attatchment->m_object = this;
    //call the on attatch function
    attatchment->onAttatch();
}

void Object::removeAttatchment(ObjectAttatchable* attatchment)
{
    //store the position of the element
    const auto& pos = std::find(m_attatchments.begin(), m_attatchments.end(), attatchment);
    //stop if the element was not found
    if (pos == m_attatchments.end()) {return;}
    //else, call the on remove function
    attatchment->onRemove();
    //set the parent object to 0
    attatchment->m_object = 0;
    //lastly, erase the element
    m_attatchments.erase(pos);
}

bool Object::onUpdate()
{
    //iterate over all attatchments for the default update function
    for (size_t i = 0; i < m_attatchments.size(); ++i)
    {
        //update the element
        m_attatchments[i]->onUpdate();
    }
    //iterate over all attatchments for the late update functions
    for (size_t i = 0; i < m_attatchments.size(); ++i)
    {
        //update the element
        m_attatchments[i]->onLateUpdate();
    }

    //reset the update flag
    m_updateFlags = 0x00;

    //always return true
    return true;
}


ObjectAttatchable* Object::getFirstOfTypeName(const char* typeName) const noexcept
{
    //iterate over all elements
    for (size_t i = 0; i < m_attatchments.size(); ++i)
    {
        //check for the type name
        if (strcmp(typeName, m_attatchments[i]->getTypeName()) == 0)
        {
            //return the element
            return m_attatchments[i];
        }
    }
    //if the code runs untill here, no element was found. Return 0.
    return 0;
}

ObjectAttatchable* Object::getNthOfTypeName(const char* typeName, uint64_t elementIndex) const noexcept
{
    //store how many elements where found
    uint64_t count = 0;
    //iterate over all elements
    for (size_t i = 0; i < m_attatchments.size(); ++i)
    {
        //check if this is of the searched type
        if (strcmp(typeName, m_attatchments[i]->getTypeName()) == 0)
        {
            //increment the found count
            ++count;
            //check if the count is correct
            if (count == elementIndex)
            {
                //return the element
                return m_attatchments[i];
            }
        }
    }
    //if the code runs untill here, no element was found. Return 0.
    return 0;
}

ObjectAttatchable* Object::getLastOfTypeName(const char* typeName) const noexcept
{
    //iterate backwards over all elements
    for (size_t i = m_attatchments.size(); i > 0; --i)
    {
        //check the next element
        if (strcmp(typeName, m_attatchments[i-1]->getTypeName()) == 0)
        {
            //return the text element
            return m_attatchments[i-1];
        }
    }
    //if the code runs untill here, no element was found. Return 0.
    return 0;
}

void Object::getAllChlidren(std::vector<Object*>& children) noexcept
{
    //iterate over all children
    for (auto it = m_children.begin(); it != m_children.end(); ++it)
    {
        //add the child
        children.push_back(it->second);
        //call the function on the child to add its children
        it->second->getAllChlidren(children);
    }
}

//statics
uint8_t Object::UPDATE_MOVED = 0b1;
uint8_t Object::UPDATE_ROTATED = 0b10;
uint8_t Object::UPDATE_SCALED = 0b100;