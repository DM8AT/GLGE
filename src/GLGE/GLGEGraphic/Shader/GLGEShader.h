/**
 * @file GLGEShader.h
 * @author DM8AT
 * @brief Store the deffination for a shader class
 * @version 0.1
 * @date 2025-05-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_SHADER_
#define _GLGE_SHADER_

//include the settings
#include "../../GLGECore/GLGESettings.h"
//include instances
#include "../../GLGECore/GLGEInstance.h"
//include the API Shader
#include "../GraphicAPI/GLGEGraphicAPI_Shader.h"
//include pathes
#include "../../GLGECore/Files/GLGEPath.h"
//include buffer
#include "../GLGEBuffer.h"

/**
 * @brief store a mapping from a buffer to a shader binding point
 */
typedef struct s_BufferShaderBinding
{
    /**
     * @brief store the index of the unit to bind to
     * 
     * @warning the first few units (like 0 and 1) are often used by GLGE itself
     */
    uint32_t unit;
    /**
     * @brief store a pointer to the buffer to bind
     */
    void* buffer;

    //check for C++ to add a constructor
    #if GLGE_CPP

    /**
     * @brief Construct new information of a mapping from a buffer to a shader binding point
     * 
     * @param _unit the index of the shader unit to bind to
     * @param _buffer a pointer to the buffer to bind
     */
    s_BufferShaderBinding(uint32_t _unit, Buffer* _buffer) : unit(_unit), buffer(_buffer->getMemoryArena()) {}

    /**
     * @brief Construct a new information of a mapping from a buffer to a shader binding port
     * 
     * @param _unit the unit to bind to
     * @param _buffer the memory arena to bind to that port
     */
    s_BufferShaderBinding(uint32_t _unit, GraphicMemoryArena* _buffer) : unit(_unit), buffer(_buffer) {}

    #endif //end of C++ section

} BufferShaderBinding;

//check for C++
#if GLGE_CPP

//say that a shader processor will be defined
class ShaderProcessor;
//say that textures will be defined
class Texture;

/**
 * @brief a shader is a code program that runs on the GPU
 */
class Shader : public InstAttachableClass
{
public:

    /**
     * @brief Construct a new Shader
     */
    Shader() = default;

    /**
     * @brief Construct a new Shader
     * 
     * @param path the path to the file to load as a shader
     * @param processor the shader processor to use for compillation
     * @param textures store an unorderd map that maps names to texture pointers the shader can access
     * @param buffers store an unorderd map that maps names to buffer binding solutions
     * @param instance the instance the shader will belong to
     */
    Shader(const Path& path, ShaderProcessor* processor, 
            std::unordered_map<std::string_view, Texture*> textures, 
            std::unordered_map<std::string_view, BufferShaderBinding> buffers, const Instance& instance);

    /**
     * @brief Construct a new shader
     * 
     * @param source the source code for the shader
     * @param file a path to the shader. Used for the #include preprocessor statement. 
     * @param processor the shader processor to use for compillation
     * @param textures store an unorderd map that maps names to texture pointers the shader can access
     * @param buffers store an unorderd map that maps names to buffer binding solutions
     * @param instance the instance the shader will belong to
     */
    Shader(const std::string source, std::string file, ShaderProcessor* processor, 
            std::unordered_map<std::string_view, Texture*> textures, 
            std::unordered_map<std::string_view, BufferShaderBinding> buffers, const Instance& instance);

    /**
     * @brief Destroy the Shader
     */
    virtual ~Shader();

    //add the shader processor as a friend class
    friend class ShaderProcessor;

    /**
     * @brief Set or add an entry point
     * @warning DO NOT USE THIS MANUALY!
     * 
     * @param type the type of shader process to add the entry point for
     * @param string the function name of the entry point for this shader stage
     */
    void setEntryPoint(ShaderType type, const std::string& string) {m_shaders[type] = string;}

    /**
     * @brief Get the file path to the shader file
     * 
     * @return const std::string& the path to the shader file
     */
    const std::string& getFile() const noexcept {return m_file;}

    /**
     * @brief make this the current shader. This updates all needed uniforms. 
     * 
     * @param cmdBuffer the command buffer to attach the shader on
     */
    void attatch(GraphicCommandBuffer* cmdBuffer) noexcept;

    /**
     * @brief remove this shader from being the compute and clean up
     * 
     * @param cmdBuffer the command buffer to detatch the shader from
     */
    void detatch(GraphicCommandBuffer* cmdBuffer) noexcept;

    /**
     * @brief Get the Shader Stages and the corresponding entry points the shader contains
     * 
     * @return const std::unordered_map<ShaderType, std::string>& the shader's stored shader stages and corresponding entry point names
     */
    inline std::unordered_map<ShaderType, std::string>& getShaderStages() noexcept {return m_shaders;}

    /**
     * @brief prepare a sourcecode snipped for a shader stage
     * 
     * @param src the source code of the stage to prepare
     * @param stage the shader stage to prepare for
     */
    static void prepareForStage(std::string& src, ShaderType stage) noexcept;

    //add the graphic shader class as a friend class
    friend class GraphicShader;

    /**
     * @brief Get the source processed by GLGE from the shader file
     * 
     * @return const std::string& a constant reference to the shader source code
     */
    inline const std::string& getSource() const noexcept {return m_glgeSource;}

    /**
     * @brief Get a specific texture that is attatched to the shader
     * 
     * @param name the name of the texture to querry
     * @return Texture* a pointer to the texture or 0 if it was not found
     */
    inline Texture* getTexture(const std::string_view& name) noexcept {return (m_textures.find(name) != m_textures.end()) ? m_textures[name] : 0;}

    /**
     * @brief Get the amount of bound textures
     * 
     * @return uint64_t the amount of bound textures
     */
    inline uint64_t getTextureCount() const noexcept {return m_textures.size();}

    /**
     * @brief Get all the textures bound to the shader
     * 
     * @return std::unordered_map<std::string_view, Texture*>& a reference to an unorderd map of all textures
     */
    inline std::unordered_map<std::string_view, Texture*>& getTextures() noexcept {return m_textures;}

    /**
     * @brief Get the amount of buffers bound to the shader
     * @warning This is both compute as well as shader storage buffer
     * 
     * @return uint64_t The amount of buffer shader bindings the shader can access
     */
    inline uint64_t getBufferCount() const noexcept {return m_buffers.size();}

    /**
     * @brief Get all the buffers bound to the shader
     * 
     * @return std::unordered_map<std::string_view, BufferShaderBinding>& a reference to the unorderd map of all buffer bindings
     */
    inline std::unordered_map<std::string_view, BufferShaderBinding>& getBuffers() noexcept {return m_buffers;}

    /**
     * @brief Get the Graphic Shader of the shader
     * @warning ONLY USE IF YOU KNOW WHAT YOU'RE DOING
     * 
     * @return GraphicShader* a pointer to the graphic shader the shader encapsulates
     */
    inline GraphicShader* getGraphicShader() const noexcept {return m_shader;}

    /**
     * @brief re-compile the shader from new source code
     * 
     * @param sourcecode the new source code for the shader
     * @param processor the shader processor to compile the code with
     */
    void recreate(std::string sourcecode, ShaderProcessor* processor) noexcept;

protected:

    /**
     * @brief store the unprocessed GLGE shader source code
     */
    std::string m_glgeSource;
    /**
     * @brief store the absolute path to the file
     */
    std::string m_file = "";
    /**
     * @brief store all the shader stages
     */
    std::unordered_map<ShaderType, std::string> m_shaders;
    /**
     * @brief store the actual graphic shader
     */
    GraphicShader* m_shader = 0;

    /**
     * @brief store all textures the shader can access
     */
    std::unordered_map<std::string_view, Texture*> m_textures;

    /**
     * @brief store all buffers the shader can read and write to / from
     */
    std::unordered_map<std::string_view, BufferShaderBinding> m_buffers;

};

#endif //end of C++ section

#endif