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

Shader::Shader(const Path& file, ShaderProcessor* processor, const Instance& instance)
 : InstAttachableClass((Instance*)&instance, file.getRawPath())
{
    //calculate the path in the native format
    m_file = file.getPath(PATH_LAYOUT_NATIVE);

    //check if the file exists
    if (!File::exists(m_file))
    {
        //create an output stream
        std::stringstream stream;
        //fill the stream
        stream << "Failed to load file \"" << m_file << "\" as a shader";
        //stop the function and throw an error
        m_instance->log(stream, MESSAGE_TYPE_ERROR);
        //stop the function
        return;
    }

    //load the file
    File f(m_file, false);
    //store the file contents
    m_glgeSource = std::string(f.content().data);

    //process the source GLGE source code to convert it to GLSL source code
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

void Shader::prepareForStage(std::string& src, ShaderType stage) noexcept
{
    //store the string to prefix
    std::string prefix = "#define GLGE_SHADER_TYPE " + std::to_string((uint32_t)stage) + "\n";

    //prefix the source code with the define
    src = prefix + src;
}

void Shader::attatch(GraphicCommandBuffer* cmdBuff) noexcept
{
    //attatch the actual shader
    m_shader->onAttatch(cmdBuff);
}