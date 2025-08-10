/**
 * @file GLGEAssetTexture.cpp
 * @author DM8AT
 * @brief implement the loading and saving for the asset texture
 * @version 0.1
 * @date 2025-08-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the asset texture
#include "GLGEAssetTexture.h"

//include pugixml for XML stuff
#include "../../GLGE3rdParty/pugixml/pugixml/src/pugixml.hpp"
//include SDL image for loading image files
#include <SDL2/SDL_image.h>

//store the mapping elements from a base 64 string to a value
static const std::string BASE_64_MAPPING = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

//store the size of a base 64 element
#define BASE64_ELEMENT_SIZE 6
//store the maximum value in base 64
#define BASE64_MAX 0b111111
//store how many bits are in a byte
#define BITS_PER_BYTE 8

/**
 * @brief convert some data stored in a base 64 string to the data
 * 
 * @param base64 a constant reference to a string containing the base 64 encoded data
 * @param data a reference to an STD vector to fill the data from the base 64 string to
 */
void base64Decode(const std::string& base64, std::vector<uint8_t>& data)
{
    //clear the data vector and reserve enough space
    data.clear();
    data.reserve((base64.size() * BASE64_ELEMENT_SIZE + BITS_PER_BYTE - 1) / BITS_PER_BYTE);

    //store the element to write to the buffer
    uint32_t buffer = 0;
    //store how many bits of the current byte are used
    int bits_collected = 0;

    //iterate over all base 64 elements in the string
    for (char c : base64)
    {
        //padding character, stop decoding
        if (c == '=') {break;}

        //get the position in the character map
        std::size_t pos = BASE_64_MAPPING.find(c);
        //ignore non-base64 characters (characters that are not in the character map)
        if (pos == std::string::npos)
            continue; 

        //add the position element to the buffer
        buffer = (buffer << BASE64_ELEMENT_SIZE) | ((uint32_t)pos);
        //increase the amount of collected bits
        bits_collected += BASE64_ELEMENT_SIZE;

        //decrease the amount of written bits if it is bigger than the size of a byte
        //should never be bigger than two bytes because a byte is 8 bit and a Base64 element is 6 bit
        if (bits_collected >= BITS_PER_BYTE)
        {
            //decrease the amount of collected bits and add the byte to the data
            bits_collected -= BITS_PER_BYTE;
            data.push_back((uint8_t)((buffer >> bits_collected) & 0xFF));
        }
    }
}

AssetTexture::AssetTexture(std::filesystem::path path, TexturePurpose purpose, AssetStorage* storage)
   //initalize the asset data
 : Asset(path, storage),
   //store the purpose of the texture
   m_purpose(purpose)
{
    //load the file's contents
    loadContent();
}

void AssetTexture::reload() noexcept
{
    //if the file is empty, stop
    if (m_file == "") {return;}

    //check if the file exists
    if (std::filesystem::is_regular_file(m_file))
    {
        //check if the file was changed
        if (m_lastWrite != std::filesystem::last_write_time(m_file))
        {
            //debug info
            GLGE_DEBUG_WRAPPER(
                //print some info
                std::stringstream stream;
                stream << "Re-Loading an existsing texture asset from file " << m_file << " because the file was updated.";
                m_storage->instance->logDebug(stream, MESSAGE_TYPE_DEBUG);
            )

            //re-load the contents
            loadContent();

            //store the new last write time
            m_lastWrite = std::filesystem::last_write_time(m_file);
        }
    }
}

void AssetTexture::loadContent()
{
    //load the file with the xml parser
    pugi::xml_document doc;
    pugi::xml_parse_result res = doc.load_file(m_file.c_str());
    
    //check if the parsing is successfully
    if (!res)
    {
        //parsing was not successfull - print an error
        std::stringstream stream;
        stream << "Failed to load the asset texture file " << m_file << " because a parsing error occoured. \nError: \n    "
               << res.description() << "\nAt:\n" << getCharacterInfo(m_file, res.offset, "    ");
        //log the error
        m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
        //stop the functoin 
        return;
    }
    //check if the version is OK
    if (!hasAcceptableGLGEVersion(&doc, m_file, m_storage->instance))
    {
        //print a warning
        m_storage->instance->log("Loading a texture with newer GLGE version than the library. Some elements may not load correctly.", MESSAGE_TYPE_WARNING);
    }

    //get the texture node
    pugi::xml_node tex = doc.child("texture");
    //check if the node exists
    if (!tex)
    {
        //the texture node was not found -> error
        std::stringstream stream;
        stream << "Failed to load the texture asset file " << m_file << " because no top-level element named \"texture\" was found";
        m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
        return;
    }

    //get the name attribute
    pugi::xml_attribute name = tex.attribute("name");
    //check if a name attribute exists
    if (!name)
    {
        //the name attribute was not found -> error
        std::stringstream stream;
        stream << "Failed to load the texture asset file " << m_file << " because no attrbute named \"name\" belonging to top-level attribute named \"texture\"";
        m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
        return;
    }

    //get the asset name
    std::string nameStr = name.value();
    //replace all occourences of a slash with a % (percent) to allow access to compound assets
    for (auto pos = nameStr.find_first_of(GLGE_ASSET_SUB_ASSET_SEPERATOR); pos != std::string::npos; pos = nameStr.find_first_of(GLGE_ASSET_SUB_ASSET_SEPERATOR)) {nameStr.replace(pos, 1, GLGE_REPLACE_SUB_ASSET_SEPERATOR_WITH);}
    //store the name
    m_name = nameStr;

    //store the formating info
    uint8_t channels = 0;
    //get the format node
    pugi::xml_node format = tex.child("format");
    if (!format)
    {
        //the format node was not found -> error
        std::stringstream stream;
        stream << "Failed to load the texture asset file " << m_file << " because no element named \"format\" belonging to top-level attribute named \"texture\"";
        m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
        return;
    }
    //get the channels attribute
    pugi::xml_attribute n_channels = format.attribute("channels");
    if (!n_channels)
    {
        //the channels attribute was not found -> error
        std::stringstream stream;
        stream << "Failed to load the texture asset file " << m_file << " because no attribute named \"channels\" belonging to element named \"format\"";
        m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
        return;
    }
    //extract the channel count
    channels = atoi(n_channels.value());

    //get the data element of the texture node
    pugi::xml_node data = tex.child("data");
    //check if a data attribute exists
    if (!data)
    {
        //the data attribute was not found -> error
        std::stringstream stream;
        stream << "Failed to load the texture asset file " << m_file << " because no element named \"data\" belonging to top-level attribute named \"texture\"";
        m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
        return;
    }

    //store the raw data
    std::vector<uint8_t> rawData;
    //get a potential file attribute
    pugi::xml_attribute file = data.attribute("file");
    //check if the attribute was found
    if (file)
    {
        //the attribute exists
        //use a texture loaded from another file
        std::ifstream f(file.value(), std::ifstream::ate | std::ifstream::binary);
        //safty check to check if the file was found
        if (!f.is_open())
        {
            //error: file not found
            std::stringstream stream;
            stream << "Failed to load the texture asset file " << m_file << " because the file \"" << file.value() << "\" is requested by the texture but was not found. Info: Root is the working directory, NOT the location of the file";
            m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
            return;
        }
        //resize the raw data to the file's size
        rawData.resize(f.tellg());
        //go back to the end
        f.seekg(f.beg);
        //get the whole file's binary data
        f.read((char*)rawData.data(), rawData.size());
        //close the file
        f.close();
    }
    else
    {
        //the attribute does not exist
        //the data SHOULD be inlined using base 64 encoding
        //check if the text exists
        if (data.text().empty())
        {
            //error - the node has no text
            std::stringstream stream;
            stream << "Failed to load the texture asset file " << m_file << " because the element named \"data\" does not contain text nor a file to load the data from";
            m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
            return;
        }

        //get the raw data
        std::string text = data.text().as_string();
        //prepare for the hex string
        auto start = text.find_first_of(BASE_64_MAPPING);
        auto end = text.find_first_not_of(BASE_64_MAPPING, start);
        end = (end == std::string::npos) ? text.size() : end;
        text = text.substr(start, end-start);
        //load the raw data
        base64Decode(text, rawData);
    }

    //load the data from RAM using SDL2_image
    //first, initalize all supported formats
    IMG_Init(IMG_INIT_PNG | IMG_INIT_AVIF | IMG_INIT_JPG | IMG_INIT_JXL | IMG_INIT_TIF | IMG_INIT_WEBP);
    //then, create a surface from the data stored in RAM
    SDL_Surface* surf = IMG_Load_RW(SDL_RWFromMem(rawData.data(), rawData.size()), 1);
    //check if the load was successfull
    if (!surf)
    {
        //error - failed to load the data
        std::stringstream stream;
        stream << "Failed to load the texture asset file " << m_file << " because the raw data could not be decoded - it does not match any knwon texture format";
        m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
        return;
    }
    //create a new surface with the format that is requested
    SDL_Surface* formated = SDL_CreateRGBSurfaceWithFormat(0, surf->w, surf->h, channels*8, (channels == 3) ? SDL_PIXELFORMAT_RGB24 : SDL_PIXELFORMAT_RGBA32);
    //blit the surface to format it
    SDL_BlitSurface(surf,0,formated,0);

    //check if a texture allready exists
    if (m_texture)
    {
        //just update the texture
        m_texture->reload(formated->pixels, uvec2(formated->w,formated->h), false, channels==4);
    }
    else
    {
        //create a new texture
        m_texture = new Texture(formated->pixels, m_purpose, uvec2(formated->w, formated->h), false, channels==4, *m_storage->instance);
    }

    //clean up
    SDL_FreeSurface(formated);
    SDL_FreeSurface(surf);
    //mark that the file is now loaded
    m_lastWrite = std::filesystem::last_write_time(m_file);
}