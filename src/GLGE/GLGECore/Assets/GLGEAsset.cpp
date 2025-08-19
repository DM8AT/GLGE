/**
 * @file GLGEAsset.cpp
 * @author DM8AT
 * @brief implement a simple asset - most of this will be overloaded by custom functions for the derived classes
 * @version 0.1
 * @date 2025-08-04
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include simple assets
#include "GLGEAsset.h"
//include asset loader
#include "GLGEAssetManager.h"

//include pugixml for XML stuff
#include "../../GLGE3rdParty/pugixml/src/pugixml.hpp"

std::string Asset::getCharacterInfo(std::filesystem::path file, uint64_t characterId, const std::string& indent) noexcept
{
    //load the file (simple file loading code)
    std::ifstream f(file, std::ifstream::ate);
    //get the size of the file and move to the start
    std::streampos size = f.tellg();
    f.seekg(std::ifstream::beg);
    //prepare a string to read the whole file
    std::string wholeFile;
    wholeFile.resize(size);
    //read and close the file
    f.read(wholeFile.data(), size);
    f.close();

    //get info about the position
    uint64_t line = 0;
    uint64_t character = 0;
    //seek the character's position
    for (uint64_t i = 0; i < characterId; ++i)
    {
        //check if the current character is a new line
        if (wholeFile[i] == '\n')
        {
            //increase the line count
            ++line;
            //reset the character
            character = 0;
        }
        else
        {
            //else, increase the character
            ++character;
        }
    }

    //get the next line end
    uint64_t lineEnd = wholeFile.find("\n", characterId);
    //get the whole line the error is in
    std::string substr = wholeFile.substr(characterId - character, lineEnd - (characterId-character));
    //create a line that points at the error element
    std::string fill;
    fill.reserve(substr.size());
    for (uint64_t i = 0; i < character; ++i) {fill += '~';}
    fill += '^';
    uint64_t currLen = fill.size();
    for (uint64_t i = 0; i < (substr.size() - currLen); ++i) {fill += '~';}
    //return the info
    return std::string("Line ") + std::to_string(line+1) + ", character " + std::to_string(character+1) + ":\n" + indent + substr + "\n" + indent + fill;
}

bool Asset::getGLGEVersionFromString(const std::string& string, uint32_t& version) noexcept
{
    //get the position of the first dot
    auto pos1 = string.find_first_of(".");
    //if the dot was not found, stop
    if (pos1 == std::string::npos) {return false;}
    //get the position of the second dot
    auto pos2 = string.find_first_of(".", pos1+1);
    //if the second dot was not found, stop
    if (pos2 == std::string::npos) {return false;}

    //create substrings for all 3 elements
    std::string s_major = string.substr(0, pos1);
    std::string s_minor = string.substr(pos1+1, pos1+1 - pos2);
    std::string s_patch = string.substr(pos2+1, string.size()-pos2);
    //convert all to integers
    uint32_t major = atoi(s_major.c_str());
    uint32_t minor = atoi(s_minor.c_str());
    uint32_t patch = atoi(s_patch.c_str());
    //finaly, set the version to the combined GLGE version
    version = GLGE_TO_VERSION(major, minor, patch);
    //successfully loaded the version
    return true;
}

bool Asset::hasAcceptableGLGEVersion(void* xmlDocument, const std::filesystem::path& file, Instance* instance) noexcept
{
    //map the document
    pugi::xml_document* doc = (pugi::xml_document*)xmlDocument;

    //get the GLGE identifyer
    pugi::xml_node identifyer = doc->child("GLGE");
    if (!identifyer)
    {
        //the identifyer node was not found -> error
        std::stringstream stream;
        stream << "Failed to load the texture asset file " << file << " because no top-level element named \"GLGE\" was found";
        instance->log(stream, MESSAGE_TYPE_ERROR);
        return false;
    }
    //get the version attribute
    pugi::xml_attribute version = identifyer.attribute("version");
    if (!version)
    {
        //the version attribute was not found -> error
        std::stringstream stream;
        stream << "Failed to load the texture asset file " << file << " because the element named \"GLGE\" does not contain an attribute named version";
        instance->log(stream, MESSAGE_TYPE_ERROR);
        return false;
    }
    //debug : print the comment in the GLGE identifyer
    GLGE_DEBUG_WRAPPER(
        //stream to print to console
        std::stringstream stream;
        stream << "Loading asset with comment: \"" << identifyer.text().as_string() << "\". Designed for GLGE version " << version.value();
        //log the comment using debug
        instance->logDebug(stream, MESSAGE_TYPE_DEBUG);
    )
    //check if the format is newer than the version
    uint32_t vers = 0;
    if (!getGLGEVersionFromString(version.value(), vers))
    {
        //error - failed to read the verson
        std::stringstream stream;
        stream << "Failed to load the texture asset file " << file << " because version string of the GLGE version attribute could not be read";
        instance->log(stream, MESSAGE_TYPE_ERROR);
        return false;
    }
    //return if the version is NOT bigger than the current one
    return !(vers > GLGE_VERSION);
}

Asset::Asset(std::filesystem::path path, AssetStorage* storage, bool) noexcept
 : m_storage(storage)
{
    //debug info
    GLGE_DEBUG_WRAPPER(
        //print some info
        std::stringstream stream;
        stream << "Loading a new asset from file " << path;
        m_storage->instance->logDebug(stream, MESSAGE_TYPE_DEBUG);
    )

    //check if the file exists
    if (!std::filesystem::is_regular_file(path))
    {
        //if it dosn't exist, log an error
        std::stringstream stream;
        stream << "Failed to load an asset from " << path << ", file not found";
        m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
        return;
    }

    //store the path
    m_file = path;
    //store the last write time of the file
    m_lastWrite = std::filesystem::last_write_time(path);

    //load the contents
    loadContents();
}

void Asset::reload() noexcept
{
    //if the file is empty, stop
    if (m_file == "") {return;}

    //check if the file exists
    if (!std::filesystem::is_regular_file(m_file))
    {
        //check if the file was changed
        if (m_lastWrite != std::filesystem::last_write_time(m_file))
        {
            //debug info
            GLGE_DEBUG_WRAPPER(
                //print some info
                std::stringstream stream;
                stream << "Re-Loading an existsing asset from file " << m_file << " because the file was updated.";
                m_storage->instance->logDebug(stream, MESSAGE_TYPE_DEBUG);
            )

            //re-load the contents
            loadContents();

            //store the new last write time
            m_lastWrite = std::filesystem::last_write_time(m_file);
        }
    }
}

void Asset::loadContents() noexcept
{
    //load the file using the XML parser
    pugi::xml_document doc;
    pugi::xml_parse_result res = doc.load_file(m_file.c_str());

    //check if the parsing was successfull
    if (!res)
    {
        //parsing was not successfull - print an error
        std::stringstream stream;
        stream << "Failed to load the asset file " << m_file << " because a parsing error occoured. \nError: \n    "
               << res.description() << "\nAt:\n" << getCharacterInfo(m_file, res.offset, "    ");
        //log the error
        m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
        //stop the functoin 
        return;
    }

    //file was parsed successfully

    //check the GLGE version
    if (!hasAcceptableGLGEVersion(&doc, m_file, m_storage->instance))
    {
        //print a warning
        m_storage->instance->log("Loading an asset with newer GLGE version than the library. Some elements may not load correctly.", MESSAGE_TYPE_WARNING);
    }

    //check if the correct element exists - the asset only supports the asset element (what a suprise)
    pugi::xml_node assNote = doc.child("asset");
    //if the asset element does not exist, print an error
    if (!assNote)
    {
        //error - asset element required
        std::stringstream stream;
        stream << "Failed to load the asset file " << m_file << " because no top-level element named asset was found";
        m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
        return;
    }
    //get the name of the asset
    pugi::xml_attribute name = assNote.attribute("name");
    //check if a name exists
    if (!name)
    {
        //error - a name is required for an asset
        std::stringstream stream;
        stream << "Failed to load the asset file " << m_file << " because the asset element does not contain a name";
        m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
        return;
    }
    //get the asset name
    std::string nameStr = name.value();
    //replace all occourences of a slash with a % (percent) to allow access to compound assets
    for (auto pos = nameStr.find_first_of(GLGE_ASSET_SUB_ASSET_SEPERATOR); pos != std::string::npos; pos = nameStr.find_first_of(GLGE_ASSET_SUB_ASSET_SEPERATOR)) {nameStr.replace(pos, 1, GLGE_REPLACE_SUB_ASSET_SEPERATOR_WITH);}
    //store the name
    m_name = nameStr;
}