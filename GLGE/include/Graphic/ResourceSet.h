/**
 * @file ResourceSet.h
 * @author DM8AT
 * @brief define a resource set and a resource structure
 * @version 0.1
 * @date 2026-03-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_RESOURCE_SET_
#define _GLGE_GRAPHIC_RESOURCE_SET_

//add base classes
#include "Core/BaseClass.h"
//add instances
#include "Core/Instance.h"
//add graphic instances
#include "Graphic/Instance.h"

//add common stuff
#include "Core/Common.h"
//add references
#include "Core/Reference.h"
//add type stuff
#include "Core/TypeInfo.h"
//add the resource set backend
#include "Backend/Graphics/ResourceSet.h"

//add the resource base
#include "Resource.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief structure the binding of a resource
     */
    class ResourceBindpoint {
    public:

        /**
         * @brief Construct a new Resource Bindpoint
         */
        ResourceBindpoint() = default;

        /**
         * @brief Construct a new Resource Bindpoint
         * 
         * @param unit the unit to bind to
         * @param typeHash the hash of the resource type to bind
         * @param nameHash the name of the resource
         */
        ResourceBindpoint(u32 unit, u64 typeHash, u64 nameHash)
         : m_unit(unit), m_typeHash(typeHash), m_nameHash(nameHash)
        {}

        /**
         * @brief check if the bind point matches a specific name
         * 
         * @param name the name to check against
         * @return `true` if the name hashes match, `false` if not
         */
        inline bool operator==(const std::string_view& name) const noexcept
        {return m_nameHash == __FNV_1A_64_HASH(name);}

        /**
         * @brief a function to create a new resource binding point
         * 
         * @tparam T the type of the resource to bind
         * @param unit the unit to bind the resource to
         * @param name the name of the resource
         * @return `ResourceBindpoint` the created bind point structure
         */
        template <typename T>
        requires std::is_base_of_v<Resource, T>
        inline static ResourceBindpoint create(u32 unit, std::string_view name) {
            //store the bindpoint to return
            ResourceBindpoint bPoint;
            bPoint.m_unit = unit;
            bPoint.m_typeHash = getTypeHash64<T>();
            bPoint.m_nameHash = __FNV_1A_64_HASH(name);
            //return the filled out binding point
            return bPoint;
        }

        /**
         * @brief Get the unit of the resource bind point
         * 
         * @return `u32` the unit or `UINT32_MAX` if unused
         */
        inline u32 getUnit() const noexcept
        {return m_unit;}

        /**
         * @brief Get the type hash
         * 
         * @return `u64` the type hash
         */
        inline u64 getTypeHash() const noexcept
        {return m_typeHash;}

        /**
         * @brief Get the name hash
         * 
         * @return `u64` the name hash
         */
        inline u64 getNameHash() const noexcept
        {return m_nameHash;}

    protected:

        /**
         * @brief the unit to bind to
         * 
         * `UINT32_MAX` is reserved for undefined
         */
        u32 m_unit = UINT32_MAX;
        /**
         * @brief the type hash of the resource type
         */
        u64 m_typeHash = 0;
        /**
         * @brief store a hash of the bind point name
         */
        u64 m_nameHash = 0;

    };

    /**
     * @brief define a named resource binding that can accept ANY type that is defined as a specific resoure type
     */
    class ResourcePreset {
    public:

        /**
         * @brief Construct a new Resource Preset
         * 
         * @param unit the unit the preset binds to
         * @param name the name for the preset
         * @param type the type of resource the preset holds
         */
        ResourcePreset(u32 unit, const std::string_view& name, ResourceType type)
         : m_unit(unit), m_nameHash(__FNV_1A_64_HASH(name)), m_type(type)
        {}

        /**
         * @brief Get the unit of the resource bind point
         * 
         * @return `u32` the unit or `UINT32_MAX` if unused
         */
        inline u32 getUnit() const noexcept
        {return m_unit;}

        /**
         * @brief Get the type
         * 
         * @return `ResourceType` the type of resource bound to the unit
         */
        inline ResourceType getType() const noexcept
        {return m_type;}

        /**
         * @brief Get the name hash
         * 
         * @return `u64` the name hash
         */
        inline u64 getNameHash() const noexcept
        {return m_nameHash;}

    protected:

        /**
         * @brief store the unit the resource binds to
         */
        u32 m_unit = UINT32_MAX;
        /**
         * @brief store a hash of the name string
         */
        u64 m_nameHash = 0;
        /**
         * @brief store the type of the resource
         */
        ResourceType m_type = ResourceType::UNDEFINED;

    };

    /**
     * @brief define a template for a resource structure
     */
    class ResourceTemplate {
    public:

        /**
         * @brief Construct a new Resource Template
         * 
         * @param bindings a vector containing all the bindings
         */
        ResourceTemplate(const std::vector<ResourcePreset>& bindings)
         : ResourceTemplate(&bindings.front(), bindings.size())
        {}

        /**
         * @brief Construct a new Resource Template
         * 
         * @param bindings the bindings for the resource template
         */
        ResourceTemplate(std::initializer_list<ResourcePreset> bindings)
         : ResourceTemplate(bindings.begin(), bindings.size())
        {}

        /**
         * @brief Construct a new Resource Template
         * 
         * @param ptr a pointer to the front of an continues block of resource presets
         * @param size the amount of elements in the continues block or resource presets
         */
        ResourceTemplate(const ResourcePreset* ptr, size_t size)
         : m_bindings(ptr, ptr + size)
        {}

        /**
         * @brief check if two templates have the same layout
         * 
         * @param other the template to compare with
         * @return `true` if they match, `false` if they don't
         */
        inline bool operator==(const ResourceTemplate& other) const noexcept {
            //sanity check the size
            if (other.m_bindings.size() != m_bindings.size())
            {return false;}

            //check that all presets are equal
            for (size_t i = 0; i < m_bindings.size(); ++i) {
                //check that the resource preset is in the other template
                bool found = false;
                for (size_t j = 0; j < other.m_bindings.size(); ++j) {
                    if ((m_bindings[i].getUnit() == other.m_bindings[j].getUnit()) &&
                        (m_bindings[i].getType() == other.m_bindings[j].getType()) &&
                        (m_bindings[i].getNameHash() == other.m_bindings[j].getNameHash())) {
                        //found a match
                        found = true;
                        break;
                    }
                }
                //if not found, they cannot match
                if (!found) 
                {return false;}
            }

            //all elements where found
            return true;
        }

        /**
         * @brief Get the bindings of the template
         * 
         * @return `const std::vector<ResourcePreset>&` a constant reference to the stored bindings presets
         */
        inline const std::vector<ResourcePreset>& getBindings() const noexcept
        {return m_bindings;}

    protected:

        /**
         * @brief store the bindings
         */
        std::vector<ResourcePreset> m_bindings;

    };

    /**
     * @brief store the structure of resources for a resource set
     */
    class ResourceStructure {
    public:

        /**
         * @brief Construct a new Resource Structure
         */
        ResourceStructure() = default;

        /**
         * @brief Construct a new Resource Structure
         * 
         * @param bindings a vector containing all bindings
         */
        ResourceStructure(const std::vector<ResourceBindpoint>& bindings)
         : ResourceStructure(bindings.data(), bindings.size())
        {}

        /**
         * @brief Construct a new Resource Structure
         * 
         * @param bindings an initializer list containing all bindings
         */
        ResourceStructure(std::initializer_list<ResourceBindpoint> bindings)
         : ResourceStructure(bindings.begin(), bindings.size())
        {}

        /**
         * @brief Construct a new Resource Structure
         * 
         * @param ptr a pointer to the start of an continues buffer of resource bindpoints
         * @param size the amount of elements in the buffer
         */
        ResourceStructure(const ResourceBindpoint* ptr, size_t size) 
         : m_bindings(ptr, ptr + size)
        {}

        /**
         * @brief check if a binding for a specific unit exists
         * 
         * @param unit the unit to check
         * @return `true` if a binding exists, `false` if not
         */
        inline bool hasBindingFor(u32 unit) const noexcept
        {if (unit < m_bindings.size()) {return m_bindings[unit].getUnit() != UINT32_MAX;} else {return false;}}

        /**
         * @brief Get the resource bindpoint of a specific unit
         * 
         * @param unit the unit to get the bind point for
         * @return `const ResourceBindpoint&` a constant reference to all bind points
         */
        inline const ResourceBindpoint& getBinding(u32 unit) const noexcept
        {return m_bindings[unit];}

        /**
         * @brief get an iterator to the beginning
         * 
         * @return `std::vector<ResourceBindpoint>::const_iterator` an iterator to the beginning
         */
        inline std::vector<ResourceBindpoint>::const_iterator begin() const noexcept
        {return m_bindings.begin();}

        /**
         * @brief get an iterator to the end
         * 
         * @return `std::vector<ResourceBindpoint>::const_iterator` an iterator to the ending
         */
        inline std::vector<ResourceBindpoint>::const_iterator end() const noexcept
        {return m_bindings.end();}

        /**
         * @brief get the amount of bindings
         * 
         * @return `size_t` the amount of existing bindings
         */
        inline size_t size() const noexcept
        {return m_bindings.size();}

        /**
         * @brief check if two resource structures match
         * 
         * @warning This ignores names. Names are purely for human understanding and two structures are still compatable if the 
         * 
         * @param other 
         * @return true 
         * @return false 
         */
        inline bool operator==(const ResourceStructure& other) const noexcept {
            //count must be the same
            if (m_bindings.size() != other.m_bindings.size()) {return false;}

            //check if all bindpoints match
            for (size_t i = 0; i < m_bindings.size(); ++i) {
                //check for a matching type on the unit in the other structure
                bool found = false;
                for (size_t j = 0; j < m_bindings.size(); ++j) {
                    if (m_bindings[i].getUnit() == other.m_bindings[j].getUnit()) {
                        //check types
                        if (m_bindings[i].getTypeHash() == other.m_bindings[j].getTypeHash())
                        {found = true; break;}
                    }
                }
                //not found, failure
                if (!found)
                {return false;}
            }

            //everything matches
            return true;
        }

    protected:

        /**
         * @brief store the structure of the resource
         */
        std::vector<ResourceBindpoint> m_bindings;

    };

    /**
     * @brief store a set of resources that can be bound and unbound
     */
    class ResourceSet : public BaseClass {
    public:

        /**
         * @brief Construct a new resource set
         * 
         * @tparam Resources the types of the resources to add. All resources must inherit from the `Resource` class
         * @param structure the structure of the resource set
         * @param resources references to all resources to store
         */
        template <typename... Resources>
        requires (std::is_base_of_v<Resource, Resources> && ...)
        ResourceSet(const ResourceStructure& structure, std::pair<const char*, Resources*> ...resources) 
         : BaseClass(), m_structure(structure), m_rSet(getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicDescription()->createResourceSet(this))
        {
            //store the resources and build the resource LUT
            m_resources.reserve(sizeof...(Resources));
            m_nameLut.reserve(sizeof...(Resources));
            ((m_nameLut.insert(std::pair{__FNV_1A_64_HASH(std::string_view(resources.first)), m_resources.size()}), m_resources.push_back(resources.second)), ...);

            //build all the bindings
            for (ResourceBindpoint binding : m_structure) {
                //lookup the resource
                if (!m_nameLut.contains(binding.getNameHash()))
                {throw GLGE::Exception("Failed to provide resources binding to all requested name binding points", "GLGE::Graphic::ResourceSet::ResourceSet");}
                Resource* res = m_resources[m_nameLut.at(binding.getNameHash())];

                //build the binding
                res->onBuildBinding(this, binding.getUnit());
            }
        }

        /**
         * @brief build a new resource set from a resource template
         * 
         * @tparam Resources the resources to store in the set
         * @param structure a template for the resource set to build
         * @param resources pairs of names and pointers to the resources
         */
        template <typename... Resources>
        requires (std::is_base_of_v<Resource, Resources> && ...)
        ResourceSet(const ResourceTemplate& structure, std::pair<const char*, Resources*> ...resources) 
         : BaseClass()
        {
            //store the resources and build the resource LUT
            m_resources.reserve(sizeof...(Resources));
            m_nameLut.reserve(sizeof...(Resources));
            ((m_nameLut.insert(std::pair{__FNV_1A_64_HASH(std::string_view(resources.first)), m_resources.size()}), m_resources.push_back(resources.second)), ...);

            //store resource bindings to build the structure
            std::vector<ResourceBindpoint> bindings;
            bindings.reserve(sizeof...(Resources));

            //build all the bindings
            for (const auto& binding : structure.getBindings()) {
                //lookup the resource
                if (!m_nameLut.contains(binding.getNameHash()))
                {throw GLGE::Exception("Failed to provide resources binding to all requested name binding points", "GLGE::Graphic::ResourceSet::ResourceSet");}
                Resource* res = m_resources[m_nameLut.at(binding.getNameHash())];

                //check if the resource is valid
                const char* msg = res->onValidation();
                if (msg) {
                    std::stringstream err;
                    err << "Provided an invalid resource. Reason: " << msg;
                    throw Exception(err.str(), "GLGE::Graphic::ResourceSet::ResourceSet");
                }

                //check if the type matches in debug
                #if GLGE_DEBUG
                if (res->getType() != binding.getType())
                {throw GLGE::Exception("Provided a binding to an unmatching type for an element", "GLGE::Graphic::ResourceSet::ResourceSet");}
                #endif

                //build the binding
                bindings.emplace_back(binding.getUnit(), res->getTypeHash(), binding.getNameHash());
            }
            //finalize the structure
            m_structure = ResourceStructure(bindings);

            //build the backend
            m_rSet = getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicDescription()->createResourceSet(this);

            //build all the backend bindings
            for (const auto& binding : structure.getBindings()) {
                //lookup the resource (safety check skipped since the first loop confirmed validity)
                Resource* res = m_resources[m_nameLut.at(binding.getNameHash())];

                //build the binding
                res->onBuildBinding(this, binding.getUnit());
            }
        }

        /**
         * @brief Destroy the Resource Set
         */
        ~ResourceSet() {
            //remove all bindings
            for (const auto& resource : m_resources) {
                //drop the binding
                resource->onRemoveBinding(this);
            }
        }

        /**
         * @brief a function to check if a resource set matches a specific template
         * 
         * @param structure a constant reference to a template to check for matching binding names, structure and types
         * @return `true` if the resource set satisfies the template, `false` if not
         */
        const bool matches(const ResourceTemplate& structure) const noexcept {
            //check if the amount of elements match
            if (structure.getBindings().size() != m_structure.size())
            {return false;}

            //check all bindings
            for (const auto& binding : m_structure) {
                //try to find the element in the template
                bool found = false;
                for (size_t i = 0; i < structure.getBindings().size(); ++i) {
                    if ((binding.getUnit() == structure.getBindings()[i].getUnit()) && 
                        (binding.getNameHash() == structure.getBindings()[i].getNameHash())) {
                        if (m_resources[m_nameLut.at(structure.getBindings()[i].getNameHash())]->getTypeHash() == binding.getTypeHash()) {
                            //match found
                            found = true;
                            break;
                        }
                    }
                }
                //if the element was not found, the structures cannot match
                if (!found) {return false;}
            }
            //all elements where found, the elements match
            return true;
        }

        /**
         * @brief Get the backend
         * 
         * @return `Reference<GLGE::Graphic::Backend::Graphic::ResourceSet>` a reference to the backend
         */
        inline Reference<GLGE::Graphic::Backend::Graphic::ResourceSet> getBackend() const noexcept
        {return m_rSet;}

        /**
         * @brief access all the resources of the set
         * 
         * @return `const std::vector<Resource*>&` a constant reference to the resource vector
         */
        inline const std::vector<Resource*>& resources() const noexcept
        {return m_resources;}

        /**
         * @brief Get the Structure of the set
         * 
         * @return `const ResourceStructure&` a constant reference to the set's structure
         */
        inline const ResourceStructure& getStructure() const noexcept
        {return m_structure;}

    protected:

        /**
         * @brief store the structure of the resource set
         */
        ResourceStructure m_structure;
        /**
         * @brief store pointers to all the resources
         */
        std::vector<Resource*> m_resources;
        /**
         * @brief store a mapping from the name hashes to the indices in the resource vector
         */
        std::unordered_map<u64, size_t> m_nameLut;
        /**
         * @brief store a reference to the backend
         */
        Reference<GLGE::Graphic::Backend::Graphic::ResourceSet> m_rSet = nullptr;

    };

}

#endif