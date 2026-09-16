/**
 * @file __Mapper.h
 * @author DM8AT
 * @brief this file defines all mappings from GLGE enum values to OpenGL values
 * @version 0.1
 * @date 2026-03-12
 * 
 * All functions are for the backend, so the `__` prefix is used. 
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_IMP_GRAPHIC_BACKEND_BUILTIN_OPENGL_MAPPER_
#define _GLGE_IMP_GRAPHIC_BACKEND_BUILTIN_OPENGL_MAPPER_

//add samplers
#include "Graphic/SamplerCPU.h"
//add OpenGL
#include "glad/glad.h"
//add color information
#include "Graphic/ColorInfo.h"
//add the shader frontend
#include "Graphic/Shader.h"
//add the buffer backend
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Buffer.h"


////////////////////////////
// SAMPLING
////////////////////////////

/**
 * @brief a helper function to translate a filter into an OpenGL filter
 * 
 * @param filter the filter enum value
 * @return `GLenum` the OpenGL enum value
 */
GLenum __translate_filter(GLGE::Graphic::SamplerCPU::Filter filter);

/**
 * @brief translate a filter enum that also has a mipmap filter
 * 
 * @param filter the main filter to translate
 * @param mipFilter the mipmap filter to translate
 * @return `GLenum` the OpenGL enum combination for the filter
 */
GLenum __translate_filter_with_mip(GLGE::Graphic::SamplerCPU::Filter filter, GLGE::Graphic::SamplerCPU::Filter mipFilter);

/**
 * @brief a helper function to translate the wrapping enum for OpenGL
 * 
 * @param wrap the wrap enum value
 * @return `GLenum` the OpenGL enum value
 */
GLenum __translate_wrap(GLGE::Graphic::SamplerCPU::Wrap wrap);

/**
 * @brief a helper function to translate the compare op enum
 * 
 * @param compareOp the compare op enum value
 * @return `GLenum` the corresponding OpenGL enum value
 */
GLenum __translate_compare_op(GLGE::Graphic::SamplerCPU::CompareOp compareOp);

////////////////////////////
// TEXTURE FORMATS
////////////////////////////

/**
 * @brief store the combined texture format
 */
struct GLTextureFormat {
    /**
     * @brief store the internal OpenGL format
     */
    GLenum internalFormat = GL_NONE;
    /**
     * @brief store the actual OpenGL format
     */
    GLenum format = GL_NONE;
    /**
     * @brief store the type for the internal format
     */
    GLenum type = GL_NONE;
};

/**
 * @brief a function to translate the pixel format to an OpenGL texture format
 * 
 * @param pixelFormat the pixel format to translate
 * @return `GLTextureFormat` the closest matching OpenGL texture format
 */
GLTextureFormat __translate_pixelFormat(const GLGE::Graphic::PixelFormat& pixelFormat);

////////////////////////////
// SHADER FORMATS
////////////////////////////

/**
 * @brief a function to translate the shader type to an OpenGL shader type
 * 
 * @throws `GLGE::Exception` if the type is not supported by OpenGL
 * 
 * @param type the type to translate
 * @return `GLenum` the OpenGL shader type
 */
GLenum __translate_shaderType(GLGE::Graphic::Shader::Type type);


////////////////////////////
// BUFFER FORMATS
////////////////////////////

/**
 * @brief a function to translate the buffer type to an OpenGL buffer type
 * 
 * @param type the buffer type to translate
 * @return `GLenum` the OpenGL buffer type
 */
GLenum __translate_bufferType(GLGE::Graphic::Backend::Graphic::Buffer::Type type);

#endif