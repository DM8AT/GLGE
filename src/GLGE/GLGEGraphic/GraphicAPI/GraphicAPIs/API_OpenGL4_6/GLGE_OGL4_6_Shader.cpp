/**
 * @file GLGE_OGL4_6_Shader.cpp
 * @author DM8AT
 * @brief define the OpenGL 4.6 shader implementatoin
 * @version 0.1
 * @date 2025-05-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the OpenGL 4.6 shader
#include "GLGE_OGL4_6_Shader.h"
//include the actual shader
#include "../../../Shader/GLGEShader.h"
//include command buffers
#include "GLGE_OGL4_6_CommandBuffer.h"
//include OpenGL
#include <GL/glew.h>

void OGL4_6_Shader::onCreate() noexcept
{
    //add the creation data
    m_shader->getInstance()->getGraphicInstance()->getBuffers()[0]->add(0, (void*)compileShader, this, sizeof(this));
}

void OGL4_6_Shader::onDestroy() noexcept
{
    //queue the deletion of the shader
    m_shader->getInstance()->getGraphicInstance()->getBuffers()[0]->add(0, (void*)destroyShader, this, sizeof(this));
}

void OGL4_6_Shader::onAttatch(GraphicCommandBuffer* cmdBuff) noexcept
{
    //add the attatching to the command buffer
    cmdBuff->add(0, (void*)attatchShader, this, sizeof(this));
}

bool OGL4_6_Shader::compileSubProgram(ShaderType type, uint32_t& shader) noexcept
{
    //store the finalized string
    std::string finalized = m_shader->getShaderStages()[type];
    //finalize the string for the stage
    Shader::prepareForStage(finalized, type);

    //store the correct OpenGL type
    GLenum stage = 0;
    //switch over the stage to select the correct OpenGL value
    switch (type)
    {
    case SHADER_TYPE_VERTEX:
        stage = GL_VERTEX_SHADER;
        break;
    case SHADER_TYPE_FRAGMENT:
        stage = GL_FRAGMENT_SHADER;
        break;
    case SHADER_TYPE_GEOMETRY:
        stage = GL_GEOMETRY_SHADER;
        break;
    case SHADER_TYPE_TESSELATION_CONTROLL:
        stage = GL_TESS_CONTROL_SHADER;
        break;
    case SHADER_TYPE_TESSELATION_EVALUATION:
        stage = GL_TESS_EVALUATION_SHADER;
        break;
    case SHADER_TYPE_COMPUTE:
        stage = GL_COMPUTE_SHADER;
        break;
    
    default:
        break;
    }

    //create the shader
    shader = glCreateShader(stage);

    //store the parameters
    const char* str = finalized.c_str();
    int32_t size = finalized.size();
    //add the shader's source code
    glShaderSource(shader, 1, &str, &size);

    //compile the shader
    glCompileShader(shader);
    //get the shader's compillation state
    GLint state = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &state);

    //attatch the shader
    glAttachShader(m_program, shader);

    //check if the compillation was successfull
    if (!state)
    {
        //log an error
        m_shader->getInstance()->log("Failed to compile a shader element", MESSAGE_TYPE_ERROR);
        //return that the compillation failed
        return false;
    }

    //return that the compilation was successfull
    return true;
}

void OGL4_6_Shader::compileShader(void* cmdData, uint64_t) noexcept
{
    //compile a shader from a command buffer

    //read the shader
    OGL4_6_Shader* self = (OGL4_6_Shader*)cmdData;

    //store all shader
    std::vector<uint32_t> shaders;
    shaders.reserve(self->getShader()->getShaderStages().size());

    //create the new program
    self->m_program = glCreateProgram();

    //store if the shader compiled
    bool success = true;
    //iterate over all stages
    for (auto it = self->getShader()->getShaderStages().begin(); it != self->getShader()->getShaderStages().end(); ++it)
    {
        //store the new shader
        uint32_t shader = 0;
        //compile the sub shader
        if (self->compileSubProgram(it->first, shader))
        {
            //if the compillation failed, note it
            success = false;
            //stop
            break;
        }
        //add the shader
        shaders.push_back(shader);
    }

    //check if the compillation failed
    if (!success)
    {
        //iterate over all shader
        for (uint64_t i = 0; i < shaders.size(); ++i)
        {
            //delete the shader
            glDeleteShader(shaders[i]);
        }
        //log the error
        self->getShader()->getInstance()->log("Failed to compile shader - Sub shader failed to compile", MESSAGE_TYPE_ERROR);
        return;
    }

    //link the program
    glLinkProgram(self->m_program);

    //delete all shader
    for (uint64_t i = 0; i < shaders.size(); ++i) {glDeleteShader(shaders[i]);}

    //check the link status
    int32_t status = 0;
    glGetProgramiv(self->m_program, GL_LINK_STATUS, &status);
    //check the status
    if (!status)
    {
        //log the error
        self->getShader()->getInstance()->log("Failed to link the shader", MESSAGE_TYPE_ERROR);
        //stop the function
        return;
    }
}

void OGL4_6_Shader::destroyShader(void* data, uint64_t) noexcept
{
    //extract the shader pointer
    OGL4_6_Shader* shader = (OGL4_6_Shader*)data;
    
    //delete the shader
    glDeleteProgram(shader->m_program);
    //set the program to 0
    shader->m_program = 0;
    //make sure to use program 0
    glUseProgram(0);
}

void OGL4_6_Shader::attatchShader(void* data, uint64_t) noexcept
{
    //attach the shader
    glUseProgram(((OGL4_6_Shader*)data)->m_program);
}