/**
 * @file Object.h
 * @author DM8AT
 * @brief define the a Object / World system
 * @version 0.1
 * @date 2026-02-06
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_CORE_OBJECT_
#define _GLGE_CORE_OBJECT_

//add common stuff
#include "Common.h"
//add the base
#include "BaseClass.h"
//add tiny ECS (custom ECS system) and wrap it into GLGE
#define TINY_JOBS_NO_FIBERS
#define TINY_JOBS_NO_LOCKFREE
#include "dependencies/TinyECS.h"

//use the library namespace
namespace GLGE {

    //worlds are defined later
    class World;

    /**
     * @brief store all the default components
     */
    namespace Component {

        /**
         * @brief a base class for all components that can be saved / loaded
         */
        class SerializableComponent {
        public:

            /**
             * @brief Destroy the Serializable Component
             */
            virtual ~SerializableComponent() = default;

            /**
             * @brief load the serializable component
             * 
             * @param entity the entity to load to
             * @param buffer the buffer to load from
             * @param offset the starting offset to start reading from
             * @param world the context of the world to load to
             * @return `u64` the amount of read bytes
             */
            virtual u64 load(Tiny::ECS::Entity entity, const std::vector<u8>& buffer, u64 offset, World* world) = 0;

            /**
             * @brief store the serializable asset
             * 
             * @param buffer the buffer to write the serializable data to
             * @param world a pointer to the world to store from
             */
            virtual void store(std::vector<u8>& buffer, World* world) = 0;

        };

        /**
         * @brief store the name of an object
         */
        class Name : public SerializableComponent {
        public:

            /**
             * @brief Construct a new Name
             * 
             * @warning initializes to an invalid name
             */
            Name() : name(UINT32_MAX) {}

            /**
             * @brief Construct a new Name
             * 
             * @param _name the ID of the name in the context of the world
             */
            Name(u64 _name)
             : name(_name)
            {}

            /**
             * @brief Destroy the Name
             */
            virtual ~Name() = default;

            /**
             * @brief store the name id
             */
            u32 name;

            /**
             * @brief store a name in a binary buffer
             * 
             * @param buffer the buffer to write to
             * @param world a pointer to the world to store from
             */
            virtual void store(std::vector<u8>& buffer, World* world) override;

            /**
             * @brief load a name from a binary buffer
             * 
             * @param entity the entity to load to
             * @param buffer the buffer to read from
             * @param offset the offset to start reading at
             * @param world the context of the world to load to
             * @return `u64` the amount of read bytes
             */
            virtual u64 load(Tiny::ECS::Entity entity, const std::vector<u8>& buffer, u64 offset, World* world) override;
        };

        /**
         * @brief a class for storing a single node of the world hierarchy tree
         */
        class HierarchyNode : public SerializableComponent {
        public:

            /**
             * @brief Construct a new Hierarchy Node
             */
            HierarchyNode() = default;

            /**
             * @brief Destroy the Hierarchy Node
             */
            virtual ~HierarchyNode() = default;

            /**
             * @brief store the entity identifier of the parent
             */
            Tiny::ECS::Entity parent = Tiny::ECS::Entity::INVALID;
            /**
             * @brief store the first sibling of the child
             */
            Tiny::ECS::Entity firstChild = Tiny::ECS::Entity::INVALID;
            /**
             * @brief the identifier of the next sibling
             */
            Tiny::ECS::Entity nextSibling = Tiny::ECS::Entity::INVALID;
            /**
             * @brief the identifier of the previous sibling
             */
            Tiny::ECS::Entity prevSibling = Tiny::ECS::Entity::INVALID;

            /**
             * @brief store a name in a binary buffer
             * 
             * @param buffer the buffer to write to
             * @param world a pointer to the world to store from
             */
            virtual void store(std::vector<u8>& buffer, World* world) override;

            /**
             * @brief load a name from a binary buffer
             * 
             * @param entity the entity to load to
             * @param buffer the buffer to read from
             * @param offset the offset to start reading at
             * @param world the context of the world to load to
             * @return `u64` the amount of read bytes
             */
            virtual u64 load(Tiny::ECS::Entity entity, const std::vector<u8>& buffer, u64 offset, World* world) override;
        };

    }

    /**
     * @brief a reference that refers to an object in a world
     */
    class Object final {
    public:

        /**
         * @brief Construct a new Object
         */
        Object() = default;

        /**
         * @brief check if the object is valid
         * 
         * @return `true` if the object is valid, `false` otherwise
         */
        inline bool valid() const noexcept
        {return !(m_ent == Tiny::ECS::Entity::INVALID);}

        /**
         * @brief check if the object is valid
         * 
         * @return `true` if the object is valid, `false` otherwise
         */
        inline operator bool() const noexcept
        {return valid();}

        /**
         * @brief Construct a new Object
         * 
         * @param ent the entt entity to refer to
         */
        Object(const Tiny::ECS::Entity& ent)
         : m_ent(ent)
        {}

        /**
         * @brief make the object usable like an entity
         * 
         * @return `Tiny::ECS::Entity` the ECS entity wrapped by the object
         */
        inline operator Tiny::ECS::Entity() const noexcept
        {return m_ent;}

    private:

        //worlds are friends
        friend class World;

        /**
         * @brief store the handle of the referenced entity
         */
        Tiny::ECS::Entity m_ent = Tiny::ECS::Entity::INVALID;

    };

    /**
     * @brief a world is a collection of objects
     */
    class World : public BaseClass {
    public:

        /**
         * @brief Construct a new World
         * 
         * @param name the name of the world
         */
        World(const std::string& name)
         : BaseClass(), m_name(name) 
        {}

        /**
         * @brief create a new object
         * 
         * @tparam Cs the components to add to the object
         * @tparam Args the initialization for each component
         * @param name the name of the object
         * @param args the arguments to initialize all components
         * @return `Object` a reference to the new object
         */
        template <typename... Cs, typename... Args>
        requires (((std::is_base_of_v<Component::SerializableComponent, Cs> || std::is_default_constructible_v<Cs>) && ...) && !Tiny::ECS::util::contains_type_v<Object, Args...>)
        Object create(const std::string& name, Args&&... args) {
            //sanity check that all components are assigned
            static_assert(sizeof...(Cs) == sizeof...(Args), "Arguments for all components must be given, not more and not less");

            //use the create function with a dummy parent
            return create<Args...>(name, Object(), std::forward<Args>(args)...);
        }

        /**
         * @brief create a new object
         * 
         * @tparam Cs the components to add to the object
         * @tparam Args the initialization for each component
         * @param name the name of the object
         * @param parent the parent of the object to create
         * @param args the arguments to initialize all components
         * @return `Object` a reference to the new object
         */
        template <typename... Cs, typename... Args>
        requires (((std::is_base_of_v<Component::SerializableComponent, Cs> || std::is_default_constructible_v<Cs>) && ...) && !Tiny::ECS::util::contains_type_v<Object, Args...>)
        Object create(const std::string& name, Object parent, Args&&... args) {
            //sanity check that all components are assigned
            static_assert(sizeof...(Cs) == sizeof...(Args), "Arguments for all components must be given, not more and not less");

            //create the entity
            Tiny::ECS::Entity e = m_reg.create<Component::Name, Component::HierarchyNode, Args...>(Component::Name{getNameIndex(name)}, Component::HierarchyNode{}, std::forward<Args>(args)...);
            //set the parents
            __setParent(Object(e), parent, false);

            //return the created object
            return Object(e);
        }

        /**
         * @brief a function that creates a lot of objects at once
         * 
         * @tparam Cs the components each object has
         * @tparam Args the argument types to create the components
         * @param count the amount of objects to create
         * @param name the name of the objects (all have the same initial name)
         * @param args the arguments to initialize all components
         * @return `std::vector<Object>` a vector containing all the new objects
         */
        template <typename... Cs, typename... Args>
        requires (((std::is_base_of_v<Component::SerializableComponent, Cs> || std::is_default_constructible_v<Cs>) && ...) && !Tiny::ECS::util::contains_type_v<Object, Args...>)
        std::vector<Object> create(u64 count, const std::string& name, Args&&... args) {
            //sanity check that all components are assigned
            static_assert(sizeof...(Cs) == sizeof...(Args), "Arguments for all components must be given, not more and not less");

            //store the entity ids as well as final objects
            std::vector<Tiny::ECS::Entity> ids;
            std::vector<Object> ret;
            ret.reserve(count);

            //compute the name index
            u64 nameId = getNameIndex(name);
            //pre-reserve for all new entities
            m_reg.bulkCreate<Component::Name, Args...>(count, ids, Component::Name(nameId), std::forward<Args>(args)...);

            //wrap the IDs in objects
            for (const Tiny::ECS::Entity& ent : ids)
            {ret.push_back(Object(ent));}

            //insert all the names in a locked scope
            {
                std::lock_guard lock(m_mtx);
                auto& vec = m_nameRegistry[nameId];

                //reserve the space
                vec.reserve(vec.size() + count);
                //copy data over
                vec.insert(vec.end(), ids.begin(), ids.end());
            }

            //return the final vector
            return ret;
        }

        /**
         * @brief destroy an object
         * 
         * @param obj a reference to the object to destroy
         */
        void destroy(Object& obj) {
            //check if the entity had a parent
            Component::HierarchyNode* node = m_reg.get<Component::HierarchyNode>(obj.m_ent);
            //remove the entity from the linked list
            m_reg.get<Component::HierarchyNode>(node->prevSibling)->nextSibling = node->nextSibling;
            m_reg.get<Component::HierarchyNode>(node->nextSibling)->prevSibling = node->prevSibling;
            //destroy the entity
            m_reg.remove(obj.m_ent);
            //invalidate the object
            memset(&obj.m_ent, 0, sizeof(Tiny::ECS::Entity));
        }

        /**
         * @brief Set the Parent of a specific object
         * 
         * @param subject the subject to set the parent for
         * @param parent the parent for the subject
         */
        void setParent(Object subject, Object parent) 
        {__setParent(subject, parent, true);}

        /**
         * @brief Get the direct children of the 
         * 
         * @param obj the object to get the children for
         * @return `std::vector<Object>` a vector containing all the children
         */
        std::vector<Object> getChildren(Object obj) {
            //store all the children
            std::vector<Object> res;
            res.reserve(16);

            //get the first child
            Component::HierarchyNode* node = nullptr;
            if (obj.valid())
            {node = m_reg.get<Component::HierarchyNode>(obj.m_ent);}
            else
            {node = &m_root;}
            Tiny::ECS::Entity ent = node->firstChild;
            while (!(ent == Tiny::ECS::Entity::INVALID)) {
                //store the new node
                res.push_back(Object(ent));
                //step forward
                node = m_reg.get<Component::HierarchyNode>(ent);
                if (!node) {return res;}
                ent = node->nextSibling;
            }

            //return the finalized list
            return res;
        }

        /**
         * @brief Get the Parent of an object
         * 
         * @param obj the object to get the parent from
         * @return `Object` the parent of an object
         */
        inline Object getParent(Object obj) 
        {return Object(m_reg.get<Component::HierarchyNode>(obj.m_ent)->parent);}

        /**
         * @brief a function to invoke a function on all direct children
         * 
         * @tparam Func the function to invoke
         * @param obj the object to pull the children from
         * @param fn the function to invoke
         */
        template <typename Func>
        requires (std::is_invocable_v<Func, Object>) 
        void forAllChildren(Object obj, Func&& fn) {
            //get the start node
            Component::HierarchyNode* hierarchy = nullptr;
            if (obj.valid())
            {hierarchy = m_reg.get<Component::HierarchyNode>(obj.m_ent);}
            else
            {hierarchy = &m_root;}
            Tiny::ECS::Entity node = hierarchy->firstChild;
            //iterate
            while (!(node == Tiny::ECS::Entity::INVALID)) {
                //first, call the function
                fn(Object(node));
                //then, step
                node = m_reg.get<Component::HierarchyNode>(node)->nextSibling;
            }
        }

        /**
         * @brief a function to invoke a function on all direct and indirect children
         * 
         * @tparam Func the function to invoke
         * @param obj the object to pull the children from
         * @param fn the function to invoke
         * @param maxDepth the maximum recursion depth
         */
        template <typename Func>
        requires (std::is_invocable_v<Func, Object>)
        void forAllDescendants(Object obj, Func&& fn, u64 maxDepth = 1000) 
        {__forAllDescendants(obj, fn, maxDepth, 0);}

        /**
         * @brief print all the children of a specific object
         * 
         * @param obj the object the children must belong directly or indirectly to
         * @param maxDepth the maximum recursion depth
         */
        void printTree(Object obj, u64 maxDepth = 1000) {
            //buffer everything to use a single print call
            std::stringstream stream;
            //first, print the object's name
            stream << getObjectName(obj) << "\n";
            //then recurse and print the children
            stream << __printChildren(obj, maxDepth+1, 1).str();
            //finally, print it
            std::cout << stream.str();
        }

        /**
         * @brief get a specific component from an object
         * 
         * @tparam Component the type of component to get
         * @param obj the object to get the component from
         * @return `Component*` a pointer to the component or `nullptr` if the component does not exist
         */
        template <typename Component>
        Component* get(const Object& obj) noexcept
        {return m_reg.get<Component>(obj.m_ent);}

        /**
         * @brief invoke a function on all entities that have at least the specified components
         * 
         * @tparam Cs the components required
         * @tparam Func the type of the function to invoke
         * @param fn the function to invoke
         */
        template <typename... Cs, typename Func>
        void each(Func&& fn)
        {m_reg.each_match<Cs...>(std::forward<Func>(fn));}

        /**
         * @brief invoke a member function on all instances of a specific component
         * 
         * @tparam C the type of component to invoke the member function on
         * @tparam Method the type of the method to invoke
         * @param method the method to invoke on all component instances
         */
        template <typename C, typename Method>
        requires (std::is_member_function_pointer_v<Method>)
        void invoke(Method&& method) 
        {m_reg.invoke(std::forward<Method>(method));}

        /**
         * @brief invoke a function on all entities that have at least the specified components
         * 
         * This function runs in parallel
         * 
         * @tparam Cs the components required
         * @tparam Func the type of the function to invoke
         * @param fn the function to invoke
         */
        template <typename... Cs, typename Func>
        void parallel_each(Func&& fn) 
        {m_reg.parallel_each_match<Cs...>(std::forward<Func>(fn), getInstance()->employer());}

        /**
         * @brief invoke a member function on all instances of a specific component
         * 
         * This function runs in parallel
         * 
         * @tparam C the type of component to invoke the member function on
         * @tparam Func the type of the function to invoke
         * @param fn the function to invoke
         */
        template <typename C, typename Func>
        requires (std::is_member_function_pointer_v<Func>)
        void parallel_invoke(Func&& fn) 
        {m_reg.parallel_invoke(std::forward<Func>(fn), getInstance()->employer());}

        /**
         * @brief check if a specific name is known
         * 
         * @param name the name of the object to check
         * @return `true` if the name is known, `false` otherwise
         */
        bool isNameKnown(const std::string& name) const
        {return m_inverseNameIdMap.find(name) != m_inverseNameIdMap.end();}

        /**
         * @brief Get the name of an object
         * 
         * @param obj the object to get the name from
         * @return `const std::string&` the name of the object
         */
        inline const std::string& getObjectName(Object obj) noexcept
        {return (obj.valid()) ? m_nameIdMap.find(m_reg.get<Component::Name>(obj.m_ent)->name)->second : m_name;}

        /**
         * @brief get the root node
         * 
         * The root node is NOT a valid entity, it is a special case. 
         */
        inline static constexpr Object getRoot() noexcept
        {return Object(Tiny::ECS::Entity::INVALID);}

    protected:

        /**
         * @brief a function used to register a name for an object
         * 
         * @param entity the entity the name was added to
         */
        void onNameAdded(Tiny::ECS::Entity entity) {
            std::lock_guard lock(m_mtx);
            m_nameRegistry[m_reg.get<Component::Name>(entity)->name].emplace_back(entity);
        }

        /**
         * @brief remove a name from an entity
         * 
         * @warning this function is not thread-safe
         * 
         * @param entity the entity who's name to remove
         * @param name the name the entity has
         */
        void __remove_name(Tiny::ECS::Entity entity, u32 name) {
            //get the correct list
            auto& vec = m_nameRegistry.at(name);
            //remove the correct name
            for (auto it = vec.begin(); it != vec.end(); ++it)
            {if (*it == entity) {vec.erase(it); break;}}
            //if no more names are known, remove the element
            if (vec.size() == 0)
            {m_nameRegistry.erase(name);}
        }

        /**
         * @brief remove a name from an entity
         * 
         * @param entity the entity who's name to remove
         */
        void onNameRemoved(Tiny::ECS::Entity entity) {
            std::lock_guard lock(m_mtx);
            //get the name
            u32 name = m_reg.get<Component::Name>(entity)->name;
            //remove the name
            __remove_name(entity, name);
        }

        /**
         * @brief a function to handle the renaming of an object
         * 
         * @param entity the entity to operate on
         * @param oldName the old name of the object
         * @param newName the new name of the object
         */
        void onRenamed(Tiny::ECS::Entity entity, const std::string& oldName, const std::string& newName) {
            std::lock_guard lock(m_mtx);
            //remove the old name
            u32 oldId = m_inverseNameIdMap.at(oldName);
            __remove_name(entity, oldId);
            //actually rename the object by resolving the new name ID
            u32 newNameId = getNameIndex(newName);
            m_reg.get<Component::Name>(entity)->name = newNameId;
            //store the new name
            m_nameRegistry[newNameId].emplace_back(entity);
        }

        /**
         * @brief a function to get or create a name ID
         * 
         * @param name the name to quarry / create the ID for
         * @return `u32` the name ID
         */
        u32 getNameIndex(const std::string& name) {
            //search if the name is known
            auto it = m_inverseNameIdMap.find(name);
            if (it == m_inverseNameIdMap.end()) {
                //need to create a new mapping
                m_nameIdMap.insert_or_assign(m_nextId, name);
                m_inverseNameIdMap.insert_or_assign(name, m_nextId);
                u32 ret = m_nextId++;
                //return the new ID
                return ret;
            }
            //just re-use the mapping
            return it->second;
        }

        /**
         * @brief the implementation function to print the children
         * 
         * @param obj the object to get the children for
         * @param maxDepth the maximum recursion depth
         * @param depth the current depth, used for indenting
         * @return `std::stringstream` the constructed stream
         */
        std::stringstream __printChildren(Object obj, u64 maxDepth, u64 depth = 0) {
            //cache the indent
            std::stringstream indent;
            for (size_t i = 0; i < depth; ++i)
            {indent << "  ";}
            //the stream to return
            std::stringstream stream;

            //if the maximal depth is reached, stop
            if (maxDepth == depth) {
                stream << indent.str();
                stream << "...\n";
                return stream;
            }

            //get all children
            std::vector<Object> children = getChildren(obj);
            //iterate over all children and 1. print the name and 2. recurse
            for (const auto& child : children) {
                stream << indent.str();
                stream << getObjectName(child) << "\n";
                stream << __printChildren(child, maxDepth, depth+1).str();
            }

            //return the final stream
            return stream;
        }

        /**
         * @brief a helper function to invoke a function on all descendants
         * 
         * @tparam Func the function to invoke
         * @param obj the object to pull the children from
         * @param fn the function to invoke
         * @param maxDepth the maximum recursion depth
         * @param depth the current recursion depth
         */
        template <typename Func>
        requires (std::is_invocable_v<Func, Object>)
        void __forAllDescendants(Object obj, Func&& fn, u64 maxDepth, u64 depth) {
            //if the depth is the max depth, stop
            if (maxDepth == depth) {return;}

            //get the start node
            Component::HierarchyNode* hierarchy = nullptr;
            if (obj.valid())
            {hierarchy = m_reg.get<Component::HierarchyNode>(obj.m_ent);}
            else
            {hierarchy = &m_root;}
            Tiny::ECS::Entity node = hierarchy->firstChild;
            //iterate
            while (!(node == Tiny::ECS::Entity::INVALID)) {
                //first, call the function
                fn(Object(node));
                //recurse
                __forAllDescendants(Object(node), fn, maxDepth, depth+1);
                //then, step
                node = m_reg.get<Component::HierarchyNode>(node)->nextSibling;
            }
        }

        /**
         * @brief a hidden internal version of the set parent function with more parameters
         * 
         * @param subject the subject to set the parent for
         * @param parent the new parent for the subject
         * @param exists `true` if the object exists allready, `false` if it is brand new
         */
        void __setParent(Object subject, Object parent, bool exists) {
            //get the nodes
            Component::HierarchyNode* parentNode = m_reg.get<Component::HierarchyNode>(parent.m_ent);
            Component::HierarchyNode* childNode = m_reg.get<Component::HierarchyNode>(subject.m_ent);
            //if the parent is invalid (root), handle that
            if (!parent.valid())
            {parentNode = &m_root;}

            //skip this is the element is new
            if (exists) {
                //lookup the element in the old child list and remove it
                if (childNode->prevSibling != Tiny::ECS::Entity::INVALID) 
                {m_reg.get<Component::HierarchyNode>(childNode->prevSibling)->nextSibling = childNode->nextSibling;}
                else {
                    //if there is no element further in front, don't forget to change the first entry of the parent to the next child
                    Component::HierarchyNode* node = nullptr;
                    if (childNode->parent == Tiny::ECS::Entity::INVALID)
                    {node = &m_root;}
                    else
                    {node = m_reg.get<Component::HierarchyNode>(childNode->parent);}
                    node->firstChild = childNode->nextSibling;
                }
                if (childNode->nextSibling != Tiny::ECS::Entity::INVALID) {m_reg.get<Component::HierarchyNode>(childNode->nextSibling)->prevSibling = childNode->prevSibling;}
                //clean the entry
                childNode->prevSibling = Tiny::ECS::Entity::INVALID;
                childNode->nextSibling = Tiny::ECS::Entity::INVALID;
            }

            //walk the parent liked child list until the end
            Tiny::ECS::Entity next = parentNode->firstChild;
            Component::HierarchyNode* currNode = parentNode;
            bool hasSiblings = false;
            //skip if no childs exist
            if (!(next == Tiny::ECS::Entity::INVALID)) {
                hasSiblings = true;
                while (true) {
                    currNode = m_reg.get<Component::HierarchyNode>(next);
                    if (currNode->nextSibling == Tiny::ECS::Entity::INVALID)
                    {break;}
                    next = currNode->nextSibling;
                }
            }

            //switch depending on if siblings exist
            if (hasSiblings) {
                //if siblings exist, update the sibling list
                currNode->nextSibling = subject.m_ent;
                childNode->prevSibling = next;
            } else {
                //if no siblings where found, write this as the first child
                parentNode->firstChild = subject.m_ent;
            }
            //store the parent
            childNode->parent = parent.m_ent;
        }

        //objects are friends
        friend class Object;
        //grant special access to name components
        friend class Component::Name;

        /**
         * @brief store the root node
         */
        Component::HierarchyNode m_root;

        /**
         * @brief store the name of the world
         */
        std::string m_name;

        /**
         * @brief store all the entities
         */
        Tiny::ECS::World m_reg;

        /**
         * @brief a mutex to make all name operations thread safe
         */
        std::mutex m_mtx;
        /**
         * @brief store a mapping from names to all entities that use that name
         */
        std::unordered_map<u32, std::vector<Tiny::ECS::Entity>> m_nameRegistry;
        /**
         * @brief store a mapping from the name index to the full name
         */
        std::unordered_map<u32, std::string> m_nameIdMap;
        /**
         * @brief store the mapping from names to string IDs
         */
        std::unordered_map<std::string, u32> m_inverseNameIdMap;
        /**
         * @brief store the next name ID
         */
        u32 m_nextId = 0;

    };

}

#endif