/**
 * @file GLGEAsset.h
 * @author DM8AT
 * @brief define an abstract asset class for all assets to inherit from
 * @version 0.1
 * @date 2025-08-04
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_ASSET_
#define _GLGE_ASSET_

//include the settings
#include "../GLGESettings.h"
//include asset storages
#include "GLGEAssetStorage.h"

//check for C++ to create a class
#if GLGE_CPP

//asset manager will be defined later - prevent circular dependencies
class AssetManager;
//instances will be defined together with asset managers
class Instance;

/**
 * @brief an abstract class to store the base for every asset
 * This class provides functions for loading and saving assets
 */
class Asset
{
public:

    /**
     * @brief Construct a new Asset
     * Explicetly deleted - empty assets can not exist
     */
    Asset() = delete;

    /**
     * @brief Construct a new Asset
     * 
     * @param path the filesystem path to load the asset from
     * @param storage a pointer to the asset storage the asset will be stored in
     */
    Asset(std::filesystem::path path, AssetStorage* storage) noexcept;

    /**
     * @brief Destroy the Asset
     */
    virtual ~Asset() noexcept {}

    /**
     * @brief re-load the asset from the disk
     */
    virtual void reload() noexcept;

    /**
     * @brief Get the Name of the asset
     * 
     * @return const std::string& a constant reference to the asset's name
     */
    inline const std::string& getName() const noexcept {return m_name;}

    /**
     * @brief Get the Type Name of the asset
     * @warning Even if this function is not virtual, sub-classes MUST have a function with this name
     * 
     * @return const char* the constant name of the asset
     */
    static const char* getTypeNameStatic() noexcept {return "GLGE_EMPTY_ASSET";}

    /**
     * @brief Get the Type Name of the asset (but overloadable)
     * 
     * @return const char* the type name of the asset
     */
    virtual const char* getTypeName() const noexcept {return getTypeNameStatic();}

protected:

    /**
     * @brief store a pointer to the asset storage the asset belongs to
     */
    AssetStorage* m_storage = 0;

    /**
     * @brief load the contents from the own file
     */
    void loadContents() noexcept;

    /**
     * @brief Get error information about a single character
     * 
     * @param file the file to load the character from
     * @param character the offset from the start of the file to the character
     * @param indent the indentation, used for cleaner printing
     * @return std::string the string containing the data for a clean print
     */
    static std::string getCharacterInfo(std::filesystem::path file, uint64_t character, const std::string& indent) noexcept;

    /**
     * @brief store the file to re-load from
     */
    std::filesystem::path m_file = "";
    /**
     * @brief store the last write time of the file
     */
    std::filesystem::file_time_type m_lastWrite;
    /**
     * @brief store the name of the asset
     */
    std::string m_name = "";

};

#endif //end of C++ section

#endif