/**
 * @file GLGEAssetManager.cpp
 * @author DM8AT
 * @brief implement the functionality for the non-template functions of the asset manager
 * @version 0.1
 * @date 2025-08-04
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//inlcude the asset manager
#include "GLGEAssetManager.h"

AssetManager::~AssetManager()
{
    //stop the reload thread
    m_runReloadThread = false;
    m_reloadThread.join();
    //iterate over all assets and delete them
    for (auto it = m_storage.assets.begin(); it != m_storage.assets.end(); ++it)
    {
        //clean up the assets
        delete it->second;
        it->second = 0;
    }
}

void AssetManager::reloadThread(AssetManager* self) noexcept
{
    //start the reload limiter
    self->m_reloadLimiter.start();
    //loop while the thread should run
    while (self->m_runReloadThread)
    {
        //iterate over the assets of the asset manager
        for (auto it = self->m_storage.assets.begin(); it != self->m_storage.assets.end(); ++it)
        {
            //reload the assets. This will stop if reloading is not needed
            if (it->second) {it->second->reload();}
        }

        //limit the iteration rate using the limiter
        self->m_reloadLimiter.endTick();
    }
}