/**
 * @file GLGEAssetStorage.h
 * @author DM8AT
 * @brief define a structure to hold the minimum data and still be able to store assets
 * @version 0.1
 * @date 2025-08-04
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_ASSET_STORAGE_
#define _GLGE_ASSET_STORAGE_

//include the settings and the instance
#include "../GLGEInstance.h"

/**
 * @brief store the character that is used as a sub-asset seperator
 */
#define GLGE_ASSET_SUB_ASSET_SEPERATOR '/'

/**
 * @brief store the character to replace the sub-asset seperator with if it is found
 */
#define GLGE_REPLACE_SUB_ASSET_SEPERATOR_WITH "%"

//check for C++ to create the structure - it is a C++ structure
#if GLGE_CPP

//assets will be defined later
class Asset;

/**
 * @brief store some data that is needed to manage assets
 */
struct AssetStorage
{
    /**
     * @brief store a mapping from the asset's name to the asset
     */
    std::unordered_map<std::string, Asset*> assets;
    /**
     * @brief store the instance the assets can register to (if they want)
     */
    Instance* instance;
};

#endif

#endif