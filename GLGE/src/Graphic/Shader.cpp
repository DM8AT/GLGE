/**
 * @file Shader.cpp
 * @author DM8AT
 * @brief implement the frontend shader class
 * @version 0.1
 * @date 2026-03-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//include the shader frontend
#include "Graphic/Shader.h"
//add exceptions
#include "Core/Exception.h"

//add SPIRV-Reflect
#include <spirv_reflect.h>

//for file io
#include <fstream>

//for default resources
//add images
#include "Graphic/Image.h"
//add raw buffers
#include "Graphic/Buffer.h"

/**
 * @brief a utility function to convert a reflected descriptor type to an resource type
 * 
 * @param type the type to translate
 * @return `GLGE::Graphic::ResourceType` the correct enum value
 */
static GLGE::Graphic::ResourceType __mapSpirvDescriptorToInput(SpvReflectDescriptorType type) {
    //just use switch case for mapping
    switch(type) {
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            return GLGE::Graphic::ResourceType::UNIFORM_BUFFER;

        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            return GLGE::Graphic::ResourceType::STORAGE_BUFFER;

        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
        case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            return GLGE::Graphic::ResourceType::SAMPLED_TEXTURE;

        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            return GLGE::Graphic::ResourceType::IMAGE;

        default:
            return GLGE::Graphic::ResourceType::CUSTOM;
    }
}

GLGE::Graphic::Shader::Shader(std::initializer_list<std::pair<std::string, std::filesystem::path>> files) 
 : BaseClass(), m_customTypes(0), m_shader(getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicDescription()->createShader(this))
{
    struct SetData {
        /**
         * @brief store a map from binding indices to name hashes
         */
        std::unordered_map<u32, u64> bindingToName;
        /**
         * @brief store a map from name hashes to binding indices
         */
        std::unordered_map<u64, u32> nameToBinding;
        /**
         * @brief map the units to types
         */
        std::unordered_map<u32, SpvReflectDescriptorType> unitTypeMap;
        /**
         * @brief store all the presets
         */
        std::vector<ResourcePreset> presets;
    };

    //per set store all the bindings
    std::unordered_map<u32, SetData> bindings;
    //iterate over all files
    for (const auto& [name, file] : files) {
        //check if the file is sane
        if (!std::filesystem::is_regular_file(file))
        {throw GLGE::Exception(std::string("Failed to load file ") + file.string() + " because it was not found", "GLGE::Graphic::Shader::Shader");}

        //load the whole file
        std::ifstream f(file, std::ifstream::binary | std::ifstream::ate);
        size_t size = f.tellg();
        f.seekg(std::ifstream::beg);
        u32* data = new u32[size];
        f.read(reinterpret_cast<char*>(data), size);

        //load the shader module
        SpvReflectShaderModule ref_mod;
        SpvReflectResult res = spvReflectCreateShaderModule(size, data, &ref_mod);
        if (res != SPV_REFLECT_RESULT_SUCCESS) {
            //make SURE to clean up
            delete[] data;
            //then, throw
            throw GLGE::Exception("Errors while reflecting SPIR-V data", "GLGE::Graphic::Shader::Shader");
        }

        //store the element to add
        ElementInfo element;

        //get the execution model
        //the Shader::Type enum is written to match minimals of the state of SpvExecutionModel_ as of 12/05/2026. 
        //it is written so that it may be extended automatically and remain memory-valid as the SpvExecutionModel_ is
        //extended in the future. Both SpvExecutionModel_ and Shader::Type shall be viewed as being equal to i32 in a 
        //strict memory sense. 
        element.type = static_cast<Shader::Type>(ref_mod.spirv_execution_model);

        //prepare for all entry points
        element.entryPoint = ref_mod.entry_points[0].name;
        
        //get all the bindings
        u32 bindCount = 0;
        spvReflectEnumerateDescriptorBindings(&ref_mod, &bindCount, nullptr);
        std::vector<SpvReflectDescriptorBinding*> binds(bindCount);
        spvReflectEnumerateDescriptorBindings(&ref_mod, &bindCount, binds.data());

        //store all the sets of the element
        std::unordered_set<u32> sets;

        //convert the reflected bindings to understandable bindings
        for (size_t i = 0; i < binds.size(); ++i) {
            SpvReflectDescriptorBinding* bind = binds[i];
            
            //hash the name
            std::string_view name = std::string_view(bind->name);
            u64 nameHash = __FNV_1A_64_HASH(name);
            //check if the binding OR name is allready in use
            auto bindNameIter = bindings[bind->set].bindingToName.find(bind->binding);
            auto nameBindIter = bindings[bind->set].nameToBinding.find(nameHash);

            //check which and how the names and / or units are known
            if (bindNameIter != bindings[bind->set].bindingToName.end()) {
                if (nameBindIter != bindings[bind->set].nameToBinding.end()) {
                    //special case if the name / unit PAIR is known
                    if (bindNameIter->second == nameHash) {
                        //the type must match (else, report type mismatch)
                        if (bindings[bind->set].unitTypeMap.find(nameBindIter->second)->second != bind->descriptor_type) {
                            throw Exception("Found a type mismatch for a unit/name pair - double definition for same unit + name with different types found", "GLGE::Graphic::Shader::Shader");
                        }
                        //types match, this is OK - but don't record the same binding twice
                        continue;
                    }
                } else {
                    //double assignment to same unit
                    throw Exception("Double assignment to same unit with different names found", "GLGE::Graphic::Shader::Shader");
                }
            } else if (nameBindIter != bindings[bind->set].nameToBinding.end()) {
                //double name usage found
                throw Exception("Double usage for same name with different units found", "GLGE::Graphic::Shader::Shader");
            }

            //map to engine type
            ResourceType t = __mapSpirvDescriptorToInput(bind->descriptor_type);
            //store the new binding point
            bindings[bind->set].presets.emplace_back(bind->binding, std::string_view(bind->name), t);
            //store the mapping
            bindings[bind->set].bindingToName.insert_or_assign(bind->binding, nameHash);
            bindings[bind->set].nameToBinding.insert_or_assign(nameHash, bind->binding);
            bindings[bind->set].unitTypeMap.insert_or_assign(bind->binding, bind->descriptor_type);
            //make known that the set is used
            sets.emplace(bind->set);
        }

        //store all the sets
        element.sets.reserve(sets.size());
        for (u32 set : sets)
        {element.sets.push_back(set);}

        //sanity check sanity check to prevent multiple inclusions
        if (m_typeMap.contains(element.type)) 
        {throw GLGE::Exception("Tried to add a single type of shader multiple times", "GLGE::Graphic::Shader::Shader");}

        //store the data
        m_nameIdMap[name] = m_elements.size();
        m_typeMap[element.type] = m_elements.size();
        m_elements.push_back(element);

        //register the shader element
        m_shader->addShaderElement(data, size, &m_elements.back());

        //make sure to clean up
        spvReflectDestroyShaderModule(&ref_mod);
        delete[] data;
    }

    //finalize the shader
    m_shader->finalize();

    //now, create the layouts for all sets
    for (const auto& [set, binding] : bindings) {
        //check if the set fits
        if (m_structures.size() < set+1)
        {m_structures.resize(set+1);}

        //create the structure
        m_structures[set].set(binding.presets);
    }
    //prepare for all sets
    m_sets.clear();
    m_sets.resize(m_structures.size(), nullptr);
}

void GLGE::Graphic::Shader::setResources(u32 set, ResourceSet* resources) {
    //check if the set exists
    if (!hasSet(set))
    {throw GLGE::Exception("Tried to bind resources to a set that does not exist", "GLGE::Graphic::Shader::setResources");}

    //nullptr always matches
    if (!resources)
    {m_sets[set] = resources; return;}

    //check if the structure matches
    const ResourceTemplate& temp = m_structures[set].get();
    if (!resources->matches(temp))
    {throw GLGE::Exception("Tried to bind a resource set with incompatible structure to a set of a shader", "GLGE::Graphic::Shader::setResources");}

    //structures match, so store the resource
    m_sets[set] = resources;
}