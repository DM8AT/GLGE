/**
 * @file __Mapper.cpp
 * @author DM8AT
 * @brief implement the mapping functions
 * @version 0.1
 * @date 2026-03-12
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add mappings
#include "__Mapper.h"
//add exceptions
#include "Core/Exception.h"

////////////////////////////
// SAMPLING
////////////////////////////

GLenum __translate_filter(GLGE::Graphic::SamplerCPU::Filter filter) {
    //just use a switch case map
    switch (filter)
    {
    case GLGE::Graphic::SamplerCPU::FILTER_LINEAR:
        return GL_LINEAR;
        break;
    case GLGE::Graphic::SamplerCPU::FILTER_NEAREST:
        return GL_NEAREST;
        break;
    
    default:
        return GL_LINEAR;
        break;
    }
}

/**
 * @brief translate a filter enum that also has a mipmap filter
 * 
 * @param filter the main filter to translate
 * @param mipFilter the mipmap filter to translate
 * @return `GLenum` the OpenGL enum combination for the filter
 */
GLenum __translate_filter_with_mip(GLGE::Graphic::SamplerCPU::Filter filter, GLGE::Graphic::SamplerCPU::Filter mipFilter) {
    //switch over the main filter
    switch (filter)
    {
    case GLGE::Graphic::SamplerCPU::FILTER_LINEAR: {
            //switch over the mip filter
            switch (mipFilter)
            {
            case GLGE::Graphic::SamplerCPU::FILTER_LINEAR:
                return GL_LINEAR_MIPMAP_LINEAR;
                break;
            case GLGE::Graphic::SamplerCPU::FILTER_NEAREST:
                return GL_LINEAR_MIPMAP_NEAREST;
                break;
            
            default:
                break;
            }
        }
        break;
    case GLGE::Graphic::SamplerCPU::FILTER_NEAREST: {
            //switch over the mip filter
            switch (mipFilter)
            {
            case GLGE::Graphic::SamplerCPU::FILTER_LINEAR:
                return GL_NEAREST_MIPMAP_LINEAR;
                break;
            case GLGE::Graphic::SamplerCPU::FILTER_NEAREST:
                return GL_NEAREST_MIPMAP_NEAREST;
                break;
            
            default:
                break;
            }
        }
        break;
    
    default:
        break;
    }
    //return a default
    return GL_LINEAR_MIPMAP_LINEAR;
}

/**
 * @brief a helper function to translate the wrapping enum for OpenGL
 * 
 * @param wrap the wrap enum value
 * @return `GLenum` the OpenGL enum value
 */
GLenum __translate_wrap(GLGE::Graphic::SamplerCPU::Wrap wrap) {
    //just use a switch case map
    switch (wrap)
    {
    case GLGE::Graphic::SamplerCPU::WRAP_REPEAT:
        return GL_REPEAT;
        break;
    case GLGE::Graphic::SamplerCPU::WRAP_MIRROR_REPEAT:
        return GL_MIRRORED_REPEAT;
        break;
    case GLGE::Graphic::SamplerCPU::WRAP_CLAMP_TO_EDGE:
        return GL_CLAMP_TO_EDGE;
        break;
    case GLGE::Graphic::SamplerCPU::WRAP_CLAMP_TO_BLACK:
        return GL_CLAMP_TO_BORDER;
        break;
    
    default:
        return GL_REPEAT;
        break;
    }
}

/**
 * @brief a helper function to translate the compare op enum
 * 
 * @param compareOp the compare op enum value
 * @return `GLenum` the corresponding OpenGL enum value
 */
GLenum __translate_compare_op(GLGE::Graphic::SamplerCPU::CompareOp compareOp) {
    //just use a switch case map
    switch (compareOp)
    {
    case GLGE::Graphic::SamplerCPU::COMPARE_OP_ALWAYS:
        return GL_ALWAYS;
        break;
    case GLGE::Graphic::SamplerCPU::COMPARE_OP_NEVER:
        return GL_NEVER;
        break;
    case GLGE::Graphic::SamplerCPU::COMPARE_OP_LESS:
        return GL_LESS;
        break;
    case GLGE::Graphic::SamplerCPU::COMPARE_OP_LESS_EQUALS:
        return GL_LEQUAL;
        break;
    case GLGE::Graphic::SamplerCPU::COMPARE_OP_GREATER:
        return GL_GREATER;
        break;
    case GLGE::Graphic::SamplerCPU::COMPARE_OP_GREATER_EQUALS:
        return GL_GEQUAL;
        break;
    case GLGE::Graphic::SamplerCPU::COMPARE_OP_EQUAL:
        return GL_EQUAL;
        break;
    case GLGE::Graphic::SamplerCPU::COMPARE_OP_NOT_EQUAL:
        return GL_NOTEQUAL;
        break;
    
    default:
        return GL_ALWAYS;
        break;
    }
}

////////////////////////////
// TEXTURE FORMATS
////////////////////////////

/**
 * @brief a helper to map a channel count to the closest amount of valid OpenGL channels
 * 
 * @param b the requested bit count
 * @return `GLGE::u8` the amount of bits for the channels
 */
inline static GLGE::u8 __helper_normalizeBits(GLGE::u8 b) {
    //0 is valid here as "does not exist" value
    if (b == 0)
        {return 0;}
    if (b <= 8)
        {return 8;}
    if (b <= 16) 
        {return 16;}
    //skip the check for 32, from here on it can only be 32 bits
    //as OpenGL has no 64 bit formats
    return 32;
}

GLTextureFormat __translate_pixelFormat(const GLGE::Graphic::PixelFormat& pixelFormat) {
    //store the format to return
    GLTextureFormat ret;

    //special casing for special formats

    //special casing for the depth component
    if (pixelFormat.order == GLGE::Graphic::PixelFormat::Order::DEPTH) {
        //general format is a depth component
        ret.format = GL_DEPTH_COMPONENT;
        GLGE::u8 quantizedSize = __helper_normalizeBits(pixelFormat.r_Bitcount);
        //set the type (special case to float, else int)
        if (pixelFormat.type == GLGE::Graphic::PixelFormat::Type::FLOAT) {
            ret.type = GL_FLOAT;
        } else {
            //just assume int formats
            ret.format = (quantizedSize == 8) ? GL_BYTE : 
                         (quantizedSize == 16) ? GL_SHORT : 
                         GL_INT;
        }

        //set the component size
        switch (quantizedSize)
        {
        case 16: ret.internalFormat = GL_DEPTH_COMPONENT16; break;
        case 24: ret.internalFormat = GL_DEPTH_COMPONENT24; break;
        case 32: {
            //switch between float and int format
            if (pixelFormat.type == GLGE::Graphic::PixelFormat::Type::FLOAT) {
                ret.internalFormat = GL_DEPTH_COMPONENT32F;
            } else {
                ret.internalFormat = GL_DEPTH_COMPONENT32;
            }
        }
        break;
        
        default:
            break;
        }

        //finally, stop
        return ret;
    } else if (pixelFormat.order == GLGE::Graphic::PixelFormat::Order::STENCIL) {
        //general format is stencil
        ret.format = GL_STENCIL;
        GLGE::u8 quantizedSize = __helper_normalizeBits(pixelFormat.r_Bitcount);
        //format must be unsigned integer
        ret.type = (quantizedSize == 8) ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT;
        //set the internal format correctly
        if (pixelFormat.r_Bitcount < 2) 
        {ret.internalFormat = GL_STENCIL_INDEX1;}
        else if (pixelFormat.r_Bitcount < 5)
        {ret.internalFormat = GL_STENCIL_INDEX4;}
        else if (pixelFormat.r_Bitcount < 9)
        {ret.internalFormat = GL_STENCIL_INDEX8;}
        else
        {ret.internalFormat = GL_STENCIL_INDEX16;}

        //finally stop
        return ret;
    } else if (pixelFormat.order == GLGE::Graphic::PixelFormat::Order::DEPTH_STENCIL) {
        //only two valid formats: 24 Depth + 8 stencil or 32F Depth + 8 stencil
        ret.format = GL_DEPTH_STENCIL;
        if (pixelFormat.r_Bitcount == 32) {
            ret.type = GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
            ret.internalFormat = GL_DEPTH32F_STENCIL8;
        } else {
            ret.type = GL_UNSIGNED_INT_24_8;
            ret.internalFormat = GL_DEPTH24_STENCIL8;
        }
        //then, return the result
        return ret;
    }

    //deduce the channel count
    GLGE::u8 channels = 0;
    //deduce the channel count and base format
    switch (pixelFormat.order)
    {
    case GLGE::Graphic::PixelFormat::Order::R:    channels = 1; ret.format = GL_RED;  break;
    case GLGE::Graphic::PixelFormat::Order::RG:   channels = 2; ret.format = GL_RG;   break;
    case GLGE::Graphic::PixelFormat::Order::RGB:  channels = 3; ret.format = GL_RGB;  break;
    case GLGE::Graphic::PixelFormat::Order::RGBA: channels = 4; ret.format = GL_RGBA; break;
    case GLGE::Graphic::PixelFormat::Order::ARGB: channels = 4; ret.format = GL_RGBA; break;
    case GLGE::Graphic::PixelFormat::Order::BGR:  channels = 3; ret.format = GL_BGR;  break;
    case GLGE::Graphic::PixelFormat::Order::BGRA: channels = 4; ret.format = GL_BGRA; break;
    case GLGE::Graphic::PixelFormat::Order::ABGR: channels = 4; ret.format = GL_RGBA; break;
    case GLGE::Graphic::PixelFormat::Order::INDEXED:
        //OpenGL core 4.6 does not support palette-indexed textures
        //Convert to R8 or R16 depending on bit depth
        channels = 1;
        ret.format = GL_RED;
        break;
    default:
        //undefined
        return ret;
    }

    //quantize the channel bitcounts to supported ones
    GLGE::u8 rb = __helper_normalizeBits(pixelFormat.r_Bitcount);
    GLGE::u8 gb = __helper_normalizeBits(pixelFormat.g_Bitcount);
    GLGE::u8 bb = __helper_normalizeBits(pixelFormat.b_Bitcount);
    GLGE::u8 ab = __helper_normalizeBits(pixelFormat.a_Bitcount);
    //red may not be 0
    if (rb == 0) {return ret;}

    //get the maximum bit count
    //this will also be quantized to OpenGL valid values
    GLGE::u8 maxBits = glm::max<GLGE::u8>(rb, gb, bb, ab);

    //determine the data type
    switch (pixelFormat.type)
    {
    case GLGE::Graphic::PixelFormat::Type::UINT:
    case GLGE::Graphic::PixelFormat::Type::UNORM:
        ret.type = (maxBits == 8)  ? GL_UNSIGNED_BYTE : 
                   (maxBits == 16) ? GL_UNSIGNED_SHORT : 
                                     GL_UNSIGNED_INT;
        break;
    case GLGE::Graphic::PixelFormat::Type::FLOAT:
        ret.type = GL_FLOAT;
        break;
    
    default:
        //unsupported format
        return ret;
        break;
    }

    //compute the internal format
    switch (pixelFormat.type)
    {
    case GLGE::Graphic::PixelFormat::Type::UINT: {
            //switch over non-normalized integer formats
            switch (channels)
            {
            case 1: ret.internalFormat = (maxBits == 8)  ? GL_R8UI  : 
                                         (maxBits == 16) ? GL_R16UI :
                                                           GL_R32UI;
            case 2: ret.internalFormat = (maxBits == 8)  ? GL_RG8UI  : 
                                         (maxBits == 16) ? GL_RG16UI :
                                                           GL_RG32UI;
            case 3: ret.internalFormat = (maxBits == 8)  ? GL_RGB8UI  : 
                                         (maxBits == 16) ? GL_RGB16UI :
                                                           GL_RGB32UI;
            case 4: ret.internalFormat = (maxBits == 8)  ? GL_RGBA8UI  : 
                                         (maxBits == 16) ? GL_RGBA16UI :
                                                           GL_RGBA32UI;
            default:
                break;
            }
        }
        break;
    case GLGE::Graphic::PixelFormat::Type::UNORM: {
            //switch over normalized integer formats
            switch (channels)
            {
            case 1: ret.internalFormat = (maxBits == 8) ? GL_R8    : GL_R16;
            case 2: ret.internalFormat = (maxBits == 8) ? GL_RG8   : GL_RG16;
            case 3: ret.internalFormat = (maxBits == 8) ? GL_RGB8  : GL_RGB16;
            case 4: ret.internalFormat = (maxBits == 8) ? GL_RGBA8 : GL_RGBA16;
            
            default:
                break;
            }
        }
        break;
    case GLGE::Graphic::PixelFormat::Type::FLOAT: {
            //switch over the float formats
            switch (channels)
            {
            case 1: ret.internalFormat = (maxBits == 16) ? GL_R16F    : GL_R32F;
            case 2: ret.internalFormat = (maxBits == 16) ? GL_RG16F   : GL_RG32F;
            case 3: ret.internalFormat = (maxBits == 16) ? GL_RGB16F  : GL_RGB32F;
            case 4: ret.internalFormat = (maxBits == 16) ? GL_RGBA16F : GL_RGBA32F;
            
            default:
                break;
            }
        }
        break;
    
    default:
        break;
    }
    //return the final format
    return ret;
}

GLenum __translate_shaderType(GLGE::Graphic::Shader::Type type) {
    switch (type) {
        case GLGE::Graphic::Shader::Type::VERTEX:                 return GL_VERTEX_SHADER;          break;
        case GLGE::Graphic::Shader::Type::FRAGMENT:               return GL_FRAGMENT_SHADER;        break;
        case GLGE::Graphic::Shader::Type::GEOMETRY:               return GL_GEOMETRY_SHADER;        break;
        case GLGE::Graphic::Shader::Type::TESSELATION_CONTROL:    return GL_TESS_CONTROL_SHADER;    break;
        case GLGE::Graphic::Shader::Type::TESSELATION_EVALUATION: return GL_TESS_EVALUATION_SHADER; break;
        case GLGE::Graphic::Shader::Type::COMPUTE:                return GL_COMPUTE_SHADER;         break;

        //Unsupported by OpenGL
        default:
            throw GLGE::Exception("Unsupported shader type for OpenGL.", "__translate_shaderType");
            return GL_FALSE;
    }
}

GLenum __translate_bufferType(GLGE::Graphic::Backend::Graphic::Buffer::Type type) {
    //just use a simple switch case mapping
    switch (type)
    {
    case GLGE::Graphic::Backend::Graphic::Buffer::Type::STORAGE: return GL_SHADER_STORAGE_BUFFER; break;
    case GLGE::Graphic::Backend::Graphic::Buffer::Type::UNIFORM: return GL_UNIFORM_BUFFER;        break;
    
    default:
        return 0;
        break;
    }
}