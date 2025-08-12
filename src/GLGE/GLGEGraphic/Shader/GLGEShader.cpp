/**
 * @file GLGEShader.cpp
 * @author DM8AT
 * @brief implement the Shader class
 * @version 0.1
 * @date 2025-05-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the shader class
#include "GLGEShader.h"
//include files
#include "../../GLGECore/Files/GLGEFile.h"
//include the shader processor
#include "GLGEShaderProcessor.h"

//include all graphic APIs
#include "../GraphicAPI/GraphicAPIs/GLGE_AllAPIs.h"

Shader::Shader(const Path& file, ShaderProcessor* processor,
            std::unordered_map<std::string_view, Texture*> textures, 
            std::unordered_map<std::string_view, BufferShaderBinding> buffers, const Instance& instance)
 : InstAttachableClass((Instance*)&instance, ATTACHMENT_TYPE_SHADER, file.getRawPath())
{
    //log a warning - deprecated function
    m_instance->log("Warning - directly loading a shader from a file. This is a deprecated function. It will be removed in a future update. ", MESSAGE_TYPE_WARNING);

    //check if the file exists
    if (!File::exists(file.getPath(PATH_LAYOUT_NATIVE)))
    {
        //create an output stream
        std::stringstream stream;
        //fill the stream
        stream << "Failed to load file \"" << file << "\" as a shader";
        //stop the function and throw an error
        m_instance->log(stream, MESSAGE_TYPE_ERROR);
        //stop the function
        return;
    }

    //load the file
    File f(file, false);
    //store the file contents
    m_glgeSource = std::string(f.content().data);

    //process the source GLGE source code to convert it to GLSL source code
    processor->processShader(*this);

    //store the inputed textures and buffers
    m_textures = textures;
    m_buffers = buffers;

    //switch over the selected graphic API
    switch (m_instance->getAPI())
    {
    case API_OPENGL_4_6:
        m_shader = new OGL4_6_Shader(this, m_instance->getGraphicInstance());
        break;
    
    default:
        break;
    }

    //create the shader
    m_shader->onCreate();
}


Shader::Shader(const std::string source, std::string file, ShaderProcessor* processor, 
        std::unordered_map<std::string_view, Texture*> textures, 
        std::unordered_map<std::string_view, BufferShaderBinding> buffers, const Instance& instance)
     : InstAttachableClass((Instance*)&instance, ATTACHMENT_TYPE_SHADER, "ShaderFromSource"),
       //store the source code
       m_glgeSource(source),
       //store the file
       m_file(file),
       //store the inputs
       m_textures(textures),
       m_buffers(buffers)
{
    //process the GLGE source code to convert it to GLSL source code
    processor->processShader(*this);

    //switch over the selected graphic API
    switch (m_instance->getAPI())
    {
    case API_OPENGL_4_6:
        m_shader = new OGL4_6_Shader(this, m_instance->getGraphicInstance());
        break;
    
    default:
        break;
    }

    //create the shader
    m_shader->onCreate();
}

Shader::~Shader()
{
    //return if the shader actual is not set
    if (!m_shader) {return;}

    //destroy the actual shader
    m_shader->onDestroy();
    //delete the shader
    delete m_shader;
    m_shader = 0;
}

void Shader::recreate(std::string sourcecode, ShaderProcessor* processor) noexcept
{
    //store the new sourcecode
    m_glgeSource = sourcecode;
    //process the GLGE source code to convert it to GLSL source code
    processor->processShader(*this);

    //recompile the shader
    m_shader->onRecompile();
}

void Shader::prepareForStage(std::string& src, ShaderType stage) noexcept
{
    //store the string to prefix
    std::string prefix = "#define GLGE_SHADER_TYPE " + std::to_string((uint32_t)stage) + "\n";

    //search the version directive (it must be the first thing)
    uint64_t pos = src.find_first_of("#version");
    //after that, search the next line
    pos = src.find_first_of('\n', pos);
    //insert the define after the new line
    src.insert(pos + 1, prefix);
}

void Shader::attatch(GraphicCommandBuffer* cmdBuff) noexcept
{
    //attatch the actual shader
    m_shader->onAttatch(cmdBuff);
}

void Shader::detatch(GraphicCommandBuffer* cmdBuffer) noexcept
{
    //detatch the actual shader
    m_shader->onDetatch(cmdBuffer);
}