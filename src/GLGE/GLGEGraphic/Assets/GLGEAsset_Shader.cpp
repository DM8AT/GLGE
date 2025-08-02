/**
 * @file GLGEAsset_Shader.cpp
 * @author DM8AT
 * @brief implement the shader asset
 * @version 0.1
 * @date 2025-08-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the shader asset
#include "GLGEAsset_Shader.h"
//include general assets
#include "../../GLGECore/Assets/GLGEAssets.h"
//include files
#include "../../GLGECore/Files/GLGEFile.h"

AssetShader::AssetShader(ShaderProcessor* processor, 
    std::unordered_map<std::string_view, Texture*> textures, 
    std::unordered_map<std::string_view, BufferShaderBinding> buffers, Asset* asset) noexcept
    //store the inputs
    : m_asset(asset),
      m_processor(processor)
{
    //load the file
    File file(Path(m_asset->getPath()), false);

    //create the shader from the file's contents
    m_shader = new Shader(file.content().data, m_asset->getPath(), m_processor, textures, buffers, *m_asset->getInstance());
}

void AssetShader::reloadFromFile() noexcept
{
    //load the file
    File file(Path(m_asset->getPath()), false);

    //pass the reload to the shader
    m_shader->recreate(file.content().data, m_processor);
}