/**
 * @file Shader.h
 * @author DM8AT
 * @brief Define the frontend API for the centralized shader creation and reflection class
 * @version 0.1
 * @date 2026-03-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_SHADER_
#define _GLGE_GRAPHIC_SHADER_

//add common stuff
#include "Core/Common.h"
//add optionals
#include "Core/utils/Optional.h"
//add the filesystem for the path class
#include <filesystem>
//add type hashing
#include "Core/TypeInfo.h"

//add the resource system
#include "ResourceSet.h"

//add base classes
#include "Core/BaseClass.h"
//add instances
#include "Core/Instance.h"
//add graphic instances
#include "Instance.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief define a class that stores a shader and reflects on its contents
     */
    class Shader : public BaseClass {
    public:

        /**
         * @brief store the types of the shader
         */
        enum Type : i32 {
            /**
             * @brief a shader program that operates on every vertex
             */
            VERTEX = 0,
            /**
             * @brief a shader program that controls the tesselation
             */
            TESSELATION_CONTROL = 1,
            /**
             * @brief a shader program that evaluates the tesselation
             */
            TESSELATION_EVALUATION = 2,
            /**
             * @brief a shader program that operates on primitives and may create or discard primitives
             */
            GEOMETRY = 3,
            /**
             * @brief a shader program that operates on every fragment
             */
            FRAGMENT = 4,
            /**
             * @brief a shader that just does stuff, fully separate from the render pipeline
             */
            COMPUTE = 5,
            /**
             * @brief a shader that is a compute kernal
             */
            KERNAL = 6
        };

        /**
         * @brief store information about a single shader
         */
        struct ElementInfo {
            /**
             * @brief store the type of the shader
             */
            Type type;
            /**
             * @brief store the entry point
             */
            std::string entryPoint;
            /**
             * @brief for a compute shader this stores the invocation count per axis (else, it is 0,0,0)
             */
            uvec3 workgroupSize {0,0,0};
            /**
             * @brief store the sets referenced by the shader
             */
            std::vector<u32> sets;
        };

        /**
         * @brief Construct a new Shader
         * 
         * @param files the SPIR-V files to add to the shader
         */
        Shader(std::initializer_list<std::pair<std::string, std::filesystem::path>> files);

        /**
         * @brief Destroy the Shader
         */
        ~Shader() = default;

        /**
         * @brief check if a set exists
         * 
         * @param set the ID of the set to check
         * @return `true` if the set exists, `false` if it does not
         */
        bool hasSet(u32 set) const noexcept
        {if (set >= m_structures.size()) {return false;} else {return m_structures[set].hasValue();}}

        /**
         * @brief Get the structure of a specific set
         * 
         * @param set the set to get the structure for
         * @return `const ResourceTemplate&` a constant reference to a template for the set
         */
        inline const ResourceTemplate& getSet(u32 set) const noexcept
        {return m_structures[set].get();}

        /**
         * @brief Get the amount of sets
         * 
         * @return `u32` the amount of sets
         */
        inline u32 getSetCount() const noexcept
        {return m_sets.size();}

        /**
         * @brief Set the resources
         * 
         * @param set the index of the set
         * @param resources a pointer to the resources
         */
        void setResources(u32 set, ResourceSet* resources);

        /**
         * @brief Get the resources of a specific set
         * 
         * @param set the set to get the resources for
         * @return `ResourceSet*` a pointer to the resources
         */
        inline ResourceSet* getResources(u32 set) const noexcept
        {return m_sets[set];}

        /**
         * @brief check if the shader has a shader element of a specific type
         * 
         * @param type the type to check
         * @return `true` if the type exists, `false` otherwise
         */
        inline bool hasType(Type type) const noexcept
        {return m_typeMap.contains(type);}

        /**
         * @brief Get the information about a specific shader element
         * 
         * @param type the type of the element to quarry
         * @return `const ElementInfo&` a constant reference to the element information
         */
        inline const ElementInfo& getOfType(Type type) const noexcept
        {return m_elements[m_typeMap.at(type)];}
        
        /**
         * @brief check if the shader is valid as a compute shader
         * 
         * @return `true` if the shader is a valid compute shader, `false` if not
         */
        inline bool isValidComputeShader() const noexcept
        {return hasType(Type::COMPUTE);}

        /**
         * @brief check if the shader is a valid rasterization shader
         * 
         * @return `true` if the shader can be use for rasterization, `false` if not
         */
        inline bool isValidRasterizationShader() const noexcept
        {return hasType(Type::VERTEX);}

        /**
         * @brief Get the Backend
         * 
         * @return `Reference<GLGE::Graphic::Backend::Graphic::Shader>` a reference to the backend
         */
        inline Reference<GLGE::Graphic::Backend::Graphic::Shader> getBackend() const noexcept
        {return m_shader;}

    protected:

        /**
         * @brief store the amount of custom types
         */
        u8 m_customTypes = 0;
        /**
         * @brief store information about the shader
         */
        std::vector<ElementInfo> m_elements;
        /**
         * @brief map the shader types to shader element infos
         */
        std::unordered_map<Type, size_t> m_typeMap;
        /**
         * @brief store mappings from shader names to the shader elements
         */
        std::unordered_map<std::string, size_t> m_nameIdMap;
        /**
         * @brief store templates for all the sets
         */
        std::vector<Optional<ResourceTemplate>> m_structures;
        /**
         * @brief store all the sets
         */
        std::vector<ResourceSet*> m_sets;

        /**
         * @brief store a reference to the shader backend
         */
        Reference<GLGE::Graphic::Backend::Graphic::Shader> m_shader;

    };

}

#endif