/**
 * @file GLGEAsset_Compound.h
 * @author DM8AT
 * @brief define the layout of a compound asset. A compound asset is an asset that contains other assets. 
 * @version 0.1
 * @date 2025-08-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_ASSET_COMPOUND_
#define _GLGE_ASSET_COMPOUND_

//include the settings
#include "../GLGESettings.h"

//check for C++ to create a class
#if GLGE_CPP

/**
 * @brief say that assets will be defined later
 */
class Asset;

/**
 * @brief store a lot of other assets
 */
class AssetCompound
{
public:

    /**
     * @brief Construct a new Asset Compound
     * Explicetly deleted to prevent empty assets from existing
     */
    AssetCompound() = delete;

    AssetCompound(Asset* m_asset);

protected:

    /**
     * @brief store a list of pointers to all assets that are part of the compound asset
     * Sadly that must be pointer because assets depend on compound assets and are not defined at this point
     */
    std::vector<Asset*> m_subassets;

    /**
     * @brief store a pointer to the asset the compound asset belongs to
     */
    Asset* m_asset = 0;

};

#endif //end of C++ section

#endif