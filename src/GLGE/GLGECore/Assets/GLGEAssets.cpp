/**
 * @file GLGEAsset.cpp
 * @author DM8AT
 * @brief implement the universal asset class
 * @version 0.1
 * @date 2025-07-31
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the universal asset
#include "GLGEAssets.h"


Asset::Asset(std::filesystem::path path, 
    //if a graphic texture is present, add a setting for that
    #if GLGE_INCLUDE_GRAPHICS
    TexturePurpose purpose,
    #else
    //else, just add a dummy setting
    uint64_t purpose,
    #endif //end of graphic dependend switch
    bool autoReload, Instance& instance)
 : InstAttachableClass(&instance, ATTACHMENT_TYPE_ASSET, path.c_str()),
   m_type(ASSET_TYPE_TEXTURE),
   m_path(path),
   m_autoReload(autoReload)
{
    //check if the file to load actually exists
    if (!std::filesystem::is_regular_file(m_path))
    {
        //error - the file was not found
        std::stringstream stream;
        stream << "The file " << m_path << " requested for file loading was not found. Full expected path: \"" << std::filesystem::current_path().c_str() << "/" << m_path.c_str() << "\"";
        m_instance->log(stream, MESSAGE_TYPE_ERROR);
        //stop before something goes wrong
        return;
    }

    //log some debug stuff
    GLGE_DEBUG_WRAPPER(
        //log wich file was loaded
        std::stringstream stream;
        stream << "Loaded the asset file \"" << std::filesystem::current_path().c_str() << "/" << m_path.c_str() << "\"";
        m_instance->logDebug(stream, MESSAGE_TYPE_DEBUG);
    )

    //create the texture
    m_data.texture.create(purpose, this);
    //store the last update time of the file
    m_loadTime = std::filesystem::last_write_time(m_path);
}

//section for graphic specific constructor
#if GLGE_INCLUDE_GRAPHICS

Asset::Asset(std::filesystem::path path, ShaderProcessor* processor, 
        std::unordered_map<std::string_view, Texture*> textures, 
        std::unordered_map<std::string_view, BufferShaderBinding> buffers, 
          bool autoReload,
        Instance& instance)
    : InstAttachableClass(&instance, ATTACHMENT_TYPE_ASSET, path.c_str()),
      //store the asset inputs
      m_type(ASSET_TYPE_SHADER),
      m_path(path),
      m_autoReload(autoReload)
{
    //store the data for the shader
    m_data.shader = AssetShader(processor, textures, buffers, this);
}

#endif //end of section for graphic specific constructor

Asset::~Asset()
{
    //check what type of asset this is
    switch (m_type)
    {
    case ASSET_TYPE_TEXTURE:
        //call the destructor of the texture asset
        m_data.texture.~AssetTexture();
        break;
    
    default:
        break;
    }
}

bool Asset::onUpdate() noexcept
{
    //check if the file currently exists
    if (std::filesystem::is_regular_file(m_path))
    {
        //check if the file changed
        if (m_loadTime < std::filesystem::last_write_time(m_path))
        {
            //the file changed. It needs to be re-created. 
            //switch over the type to select the re-creation
            switch (m_type)
            {
            case ASSET_TYPE_TEXTURE:
                m_data.texture.reloadFromFile();
                break;
            //graphic include dependend functionality
            #if GLGE_INCLUDE_GRAPHICS
            //reload a shader
            case ASSET_TYPE_SHADER:
                m_data.shader.reloadFromFile();
                break;
            #endif //end of graphic dependend stuff
            
            default:
                break;
            }
            //store the new update time
            m_loadTime = std::filesystem::last_write_time(m_path);

            //log some debug info
            GLGE_DEBUG_WRAPPER(
                //log the reload
                std::stringstream stream;
                stream << "Reloaded asset from file: " << m_path;
                m_instance->log(stream, MESSAGE_TYPE_DEBUG);
            );
        }
    }
    //if the file dosn't exist, then just use the loaded data

    //always return true, this object can't delete itself during an update
    return true;
}