/**
 * @file Shader.cpp
 * @author DM8AT
 * @brief implement the OpenGL shader backend
 * @version 0.1
 * @date 2026-03-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add OpenGL shader
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Shader.h"
//add mappings
#include "__Mapper.h"
//add exceptions
#include "Core/Exception.h"

GLGE::Graphic::Backend::Graphic::OpenGL::Shader::Shader(GLGE::Graphic::Shader* frontend) 
 : GLGE::Graphic::Backend::Graphic::Shader(frontend)
{
    //create the program
    m_program = glCreateProgram();

    //label the object in debug
    #if GLGE_DEBUG
    std::string name = std::string("ShaderProgram_") + std::to_string(m_name);
    glObjectLabel(GL_PROGRAM, m_program, name.length(), name.c_str());
    #endif
}

GLGE::Graphic::Backend::Graphic::OpenGL::Shader::~Shader() {
    //clean up all the shader
    for (u32 shader : m_shader)
    {glDeleteShader(shader);}
    m_shader.clear();
    //clear the program
    glDeleteProgram(m_program);
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Shader::addShaderElement(void* data, u32 size, const void* _element) {
    //extract the element
    const GLGE::Graphic::Shader::ElementInfo& element = *reinterpret_cast<const GLGE::Graphic::Shader::ElementInfo*>(_element);
    //create a new shader element
    u32 shader = glCreateShader(__translate_shaderType(element.type));
    //label the object in debug
    #if GLGE_DEBUG
    std::string name = std::string("Shader_") + std::to_string(m_name) + "_" + std::to_string(m_shader.size());
    glObjectLabel(GL_SHADER, shader, name.length(), name.c_str());
    #endif

    //add the source code
    glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, data, size);
    //setup the shader
    glSpecializeShader(shader, element.entryPoint.c_str(), 0, nullptr, nullptr);
    //sanity check the output
    GLint res = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
    if (!res) {
        //get the error log
        GLint logLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        std::string log(logLen, '\0');
        glGetShaderInfoLog(shader, logLen, nullptr, log.data());

        //cleanup and throw the error
        glDeleteShader(shader);
        throw GLGE::Exception(std::string("OpenGL shader compilation error:\n") + log, "GLGE::Graphic::Backend::Graphic::OpenGL::Shader::addShaderElement");
    }

    //add the compiled shader
    m_shader.push_back(shader);
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Shader::finalize() {
    //link all shader
    for (u32 shader : m_shader)
    {glAttachShader(m_program, shader);}

    //link the program
    glLinkProgram(m_program);

    //check for success
    GLint res = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &res);
    if (!res) {
        //get the error log
        GLint logLen = 0;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLen);
        std::string log(logLen, '\0');
        glGetProgramInfoLog(m_program, logLen, nullptr, log.data());

        //clean up and throw the error
        glDeleteProgram(m_program);
        m_program = 0;
        throw GLGE::Exception(std::string("OpenGL shader linking error:\n") + log, "GLGE::Graphic::Backend::Graphic::OpenGL::Shader::finalize");
    }

    //the shader is finalized
    m_finalized = true;
}