/**
 * @file GLGEGraphicAPI_Shader.cpp
 * @author DM8AT
 * @brief implement the default functionality for Graphic shaders
 * @version 0.1
 * @date 2025-06-04
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include graphic shaders
#include "GLGEGraphicAPI_Shader.h"
//include shaders
#include "../Shader/GLGEShader.h"

GraphicShader::~GraphicShader()
{
    //call the virtual destructor
    onDestroy(); 
    //de-register the shader
    m_shader->m_shader = 0;
}