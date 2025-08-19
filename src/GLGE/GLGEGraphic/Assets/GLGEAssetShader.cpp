/**
 * @file GLGEAssetShader.cpp
 * @author DM8AT
 * @brief implement the loading of the shader asset
 * @version 0.1
 * @date 2025-08-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the shader asset
#include "GLGEAssetShader.h"

//include pugixml for XML stuff
#include "../../GLGE3rdParty/pugixml/src/pugixml.hpp"

AssetShader::AssetShader(std::filesystem::path path, ShaderProcessor* processor, AssetStorage* storage)
   //initalize the actual asset
 : Asset(path, storage),
   //sotre a pointer to the processor
   m_processor(processor)
{
    //load the shader
    loadShader();
}

AssetShader::AssetShader(std::filesystem::path path, ShaderProcessor* processor, std::unordered_map<std::string_view, Texture*> textures, 
            std::unordered_map<std::string_view, BufferShaderBinding> buffers, AssetStorage* storage)
   //initalize the actual asset
 : Asset(path, storage),
   //store a pointer to the processor
   m_processor(processor),
   //store the additional shader inputs
   m_textures(textures),
   m_buffers(buffers)
{
    //load the shader
    loadShader();
}

AssetShader::~AssetShader() noexcept
{
    //check if the shader exists
    if (m_shader)
    {
        //delete the shader
        delete m_shader;
        m_shader = 0;
    }
}

void AssetShader::reload() noexcept
{
    //if the file is empty, stop
    if (m_file == "") {return;}

    //check if the file exists
    if (std::filesystem::is_regular_file(m_file))
    {
        //check if the file was changed
        if (m_lastWrite != std::filesystem::last_write_time(m_file))
        {
            //debug info
            GLGE_DEBUG_WRAPPER(
                //print some info
                std::stringstream stream;
                stream << "Re-Loading an existsing texture asset from file " << m_file << " because the file was updated.";
                m_storage->instance->logDebug(stream, MESSAGE_TYPE_DEBUG);
            )

            //re-load the contents
            loadShader();

            //store the new last write time
            m_lastWrite = std::filesystem::last_write_time(m_file);
        }
    }
}


void AssetShader::loadShader() noexcept
{
    //check if the file exists
    if (!std::filesystem::is_regular_file(m_file))
    {
        //error - file not found
        std::stringstream stream;
        stream << "Failed to load the asset shader file " << m_file << " because the file was not found";
        //log the error
        m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
        //stop the functoin 
        return;
    }
    //mark that the file is now loaded
    m_lastWrite = std::filesystem::last_write_time(m_file);

    //load the file with the xml parser
    pugi::xml_document doc;
    pugi::xml_parse_result res = doc.load_file(m_file.c_str());
    
    //check if the parsing is successfully
    if (!res)
    {
        //parsing was not successfull - print an error
        std::stringstream stream;
        stream << "Failed to load the asset shader file " << m_file << " because a parsing error occoured. \nError: \n    "
               << res.description() << "\nAt:\n" << getCharacterInfo(m_file, res.offset, "    ");
        //log the error
        m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
        //stop the functoin 
        return;
    }
    //check if the version is OK
    if (!hasAcceptableGLGEVersion(&doc, m_file, m_storage->instance))
    {
        //print a warning
        m_storage->instance->log("Loading a shader with newer GLGE version than the library. Some elements may not load correctly.", MESSAGE_TYPE_WARNING);
    }

    //get the shader element
    pugi::xml_node shader = doc.child("shader");
    if (!shader)
    {
        //the shader does not exist - print an error
        std::stringstream stream;
        stream << "Failed to load the asset shader file " << m_file << " because no element named \"shader\" is child of the root node";
        //log the error
        m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
        //stop the functoin 
        return;
    }

    //get the name attribute
    pugi::xml_attribute name = shader.attribute("name");
    //check if a name attribute exists
    if (!name)
    {
        //the name attribute was not found -> error
        std::stringstream stream;
        stream << "Failed to load the shader asset file " << m_file << " because no attrbute named \"name\" belonging to top-level attribute named \"shader\"";
        m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
        return;
    }

    //get the asset name
    std::string nameStr = name.value();
    //replace all occourences of a slash with a % (percent) to allow access to compound assets
    for (auto pos = nameStr.find_first_of(GLGE_ASSET_SUB_ASSET_SEPERATOR); pos != std::string::npos; pos = nameStr.find_first_of(GLGE_ASSET_SUB_ASSET_SEPERATOR)) {nameStr.replace(pos, 1, GLGE_REPLACE_SUB_ASSET_SEPERATOR_WITH);}
    //store the name
    m_name = nameStr;

    //get the source code
    pugi::xml_node source = shader.child("source");
    if (!source)
    {
        //the source element was not found -> error
        std::stringstream stream;
        stream << "Failed to load the shader asset file " << m_file << " because no element named \"source\" belonging to top-level attribute named \"shader\"";
        m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
        return;
    }

    //store the full source code
    std::string sourceCode = "";
    //store the path to the shader
    std::filesystem::path path;
    //get the file attribute
    pugi::xml_attribute file = source.attribute("file");
    if (file)
    {
        //load the source from another file
        //first, extract the path
        path = file.value();
        //check if the file exists
        if (!std::filesystem::is_regular_file(path))
        {
            //the source file was not found -> error
            std::stringstream stream;
            stream << "Failed to load the shader asset file " << m_file << " because the file " << path << " that is listed as source file for the shader asset";
            m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
            return;
        }

        //open the file at the end to get the length
        std::ifstream f(path, std::ifstream::ate);
        //resize the source code and go back to the start of the file
        sourceCode.resize(f.tellg());
        f.seekg(std::ifstream::beg);
        //read the whole source code and close the file
        f.read(sourceCode.data(), sourceCode.size());
        f.close();
    }
    else
    {
        //the source code is in-lined
        //just load it directly
        sourceCode = source.text().as_string();
        //the path is the path to the file
        path = m_file;
    }
    
    //check if a shader exists
    if (m_shader)
    {
        //update the shader
        m_shader->recreate(sourceCode, m_processor);
    }
    else
    {
        //create the shader
        m_shader = new Shader(sourceCode, path, m_processor, m_textures, m_buffers, *m_storage->instance);
    }
}