/**
 * @file AssetManager.h
 * @author DM8AT
 * @brief define a class to manage multiple assets at once. The class can import assets, load assets, save assets and re-load assets. The purpose is to have asset loading not block the main executions. 
 * @version 0.1
 * @date 2025-08-04
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_ASSET_MANAGER_
#define _GLGE_ASSET_MANAGER_

//include the asset storages
#include "GLGEAssetStorage.h"
//include assets - they should be managed here
#include "GLGEAsset.h"
//include instances - the asset manager is an instance attachable
#include "../GLGEInstance.h"

//check for C++ to create a class
#if GLGE_CPP

//type traits is needed to constrain the arguments passed to template functions
#include <type_traits>

/**
 * @brief store a system to manage a collection of assets
 */
class AssetManager final : public InstAttachableClass
{
protected:

    /**
     * @brief the funciton to run for asynchrone asset loading
     * 
     * @tparam T the type of asset to load. Must inherit from the asset class
     * @tparam Args a list of arguments to parse to the constructor of the asset
     * @param self a pointer to the asset manager that called the function
     * @param store a pointer to a pointer to store the new asset into. Set to 0 to store the asset only in the manager
     * @param file the file to load the asset from
     * @param args a list of arguments to parse to the constructor of the asset
     */
    template <typename T, typename ... Args> static void loadAsyncFunc(AssetManager* self, T** store, std::filesystem::path file, Args ... args) noexcept
    {
        //run the load function
        T* asset = self->load<T>(file, args...);
        //check if store is set
        if (store)
        {
            //store the asset
            *store = asset;
        }
        //safely remove the element from the vector
        self->m_loadStoreVecMutex.lock();
        //delete the thread from the queue
        for (size_t i = 0; i < self->m_loadStoreThreads.size(); ++i)
        {
            if (self->m_loadStoreThreads[i].get_id() == std::this_thread::get_id())
            {
                self->m_loadStoreThreads.erase(self->m_loadStoreThreads.begin() + i);
            }
        }
        //unlock the mutex - vector operation finished
        self->m_loadStoreVecMutex.unlock();
    }

public:

    /**
     * @brief Construct a new Asset Manager
     * Explicetly deleted - an instance is required for an asset manager
     */
    AssetManager() = delete;

    /**
     * @brief Construct a new Asset Manager
     * 
     * @param name the name of the asset manager - should be unique
     * @param instance a reference to the instance the asset manager will belong to
     */
    AssetManager(std::string name, Instance& instance)
     : InstAttachableClass(&instance, ATTACHMENT_TYPE_ASSET_MANAGER, name),
       m_storage({{}, &instance}),
       m_reloadThread(reloadThread, this)
    {}

    /**
     * @brief Destroy the Asset Manager
     * This deletes all assets loaded with this manager
     */
    virtual ~AssetManager();

    /**
     * @brief load any kind of asset
     * 
     * @tparam T the type of asset to load
     * @tparam Args the additional arguments for the constructor EXCLUDING the file path and asset manager - they are passed automatically
     * @param file the path to the filesystem to load
     * @param args the additional arguments for the constructor EXCLUDING the file path and asset manager - they are passed automatically
     * @return T* a pointer to the newly loaded asset - the asset is owned by the asset manager
     */
    template <typename T, typename ... Args> T* load(std::filesystem::path file, Args ... args)
    {
        //T is only defined for classes that inherite from Asset
        static_assert(std::is_base_of<Asset, T>::value, "T must be derived from Asset to be loaded to an asset manager");

        //create the new asset
        T* asset = new T(file, args..., &m_storage);
        //extract the name to check if it is valid
        const std::string& name = asset->getName();

        //check if the name is NOT contained in the asset map
        auto pos = m_storage.assets.find(name);
        //check if the position is at the end (not found)
        if (pos == m_storage.assets.end())
        {
            //the asset can be stored safly
            m_storage.assets[name] = asset;
            //return the new asset
            return asset;
        }

        //else, delete the asset - possibly loaded allready
        delete asset;
        //return 0 - loading failed
        return 0;
    }

    /**
     * @brief load an asset asynchrone
     * 
     * @tparam T the type of asset to load. Must inherit from the asset class
     * @tparam Args a list of arguments to parse to the constructor of the asset
     * @param store a pointer to a pointer to store the new asset into. Set to 0 to store the asset only in the manager
     * @param file the file to load the asset from
     * @param args a list of arguments to parse to the constructor of the asset
     */
    template <typename T, typename ... Args> void loadAsync(T** store, std::filesystem::path file, Args ... args)
    {
        //asset is not loaded, zero the pointer
        *store = 0;
        //T is only defined for classes that inherite from Asset
        static_assert(std::is_base_of<Asset, T>::value, "T must be derived from Asset to be loaded to an asset manager");
        //run the load function on a new thread
        std::thread thread(loadAsyncFunc<T>, this, store, file, args...);
        thread.detach();
        //safely add the thread to the vector
        m_loadStoreVecMutex.lock();
        m_loadStoreThreads.push_back(move(thread));
        m_loadStoreVecMutex.unlock();
    }

    /**
     * @brief get if there is stuff to load
     * 
     * @return true : there are still assets being loaded on other threads
     * @return false : nothing is being loaded asynchroniusly - all loads are finished
     */
    bool hasPendingLoads() const noexcept {return m_loadStoreThreads.size();}

    /**
     * @brief wait untill a specific asset is loaded
     * 
     * @tparam T the type of asset to load
     * @param asset a pointer to the asset to load
     */
    template<typename T> void waitTillLoaded(T** asset) const noexcept
    {
        //wait while the pointer holds data
        while (!(*asset)) {std::this_thread::sleep_for(std::chrono::microseconds(100));}
    }

    /**
     * @brief wait till all pending asynchrone loads are finished
     */
    void waitUntillAllLoaded() const noexcept {while (m_loadStoreThreads.size()) {std::this_thread::sleep_for(std::chrono::microseconds(100));}}

    /**
     * @brief Get the limiter that limits the rate of re-freshing the content's of the assets
     * 
     * @return Limiter& a reference to the limiter that reloads the contents
     */
    inline Limiter& getReloadLimiter() noexcept {return m_reloadLimiter;}

    /**
     * @brief Get a stored asset by it's name
     * 
     * @tparam T the type of the asset to querry
     * @param name the name or path to the asset
     * @return T* a pointer to the asset or 0 if it wasn't found
     */
    template <typename T> T* getAssetNamed(const std::string& name)
    {
        //T is only defined for classes that inherite from Asset
        static_assert(std::is_base_of<Asset, T>::value, "T must be derived from Asset to be querried from an asset manager");

        //store the querry name
        std::string querry = name;
        //store the querry to pass down
        std::string pass = "";
        //find the first occourance of a seperator character
        auto pos = name.find_first_of(GLGE_ASSET_SUB_ASSET_SEPERATOR);
        //check if it was found
        if (pos != std::string::npos)
        {
            //set the querry name to the first part of the string
            querry = name.substr(0, pos);
            //store the passdown name
            pass = name.substr(pos+1, name.size()-pos);
        }

        //search the querry element
        auto querriedPos = m_storage.assets.find(querry);
        //if the element was not found, stop
        if (querriedPos == m_storage.assets.end())
        {
            //print an error that the asset was not found
            std::stringstream stream;
            stream << "Failed to find the requested asset with name / path \"" << name << "\" from asset manager named " << m_name;
            m_instance->log(stream, MESSAGE_TYPE_ERROR);
            //return 0 to mark that the element was not found
            return 0;
        }

        //check if the pass element is empty
        if (pass == "")
        {
            //if it is empty, return the element imdeiatly
            return (T*)querriedPos->second;
        }

        //else, failed to querry the element
        //print an error that the asset was not found
        std::stringstream stream;
        stream << "Failed to find the requested asset with path \"" << "\" from asset manager named " << m_name << " because an asset on the path didn't support sub-assets";
        m_instance->log(stream, MESSAGE_TYPE_ERROR);
        //return 0 to mark that the element was not found
        return 0;
    }

protected:

    //store the asset storage
    AssetStorage m_storage;

    /**
     * @brief a mutex to stop multiple threads from acesing the load store thread vector at onve
     */
    std::mutex m_loadStoreVecMutex;
    /**
     * @brief store a list of threads that are used for loading and storing assets
     */
    std::vector<std::thread> m_loadStoreThreads;

    /**
     * @brief store the maximum iteration rate of the reload thread
     */
    Limiter m_reloadLimiter = 1;
    /**
     * @brief store the function for the reload thread of an asset manager
     * 
     * @param self a pointer to the asset manager that called the function
     */
    static void reloadThread(AssetManager* self) noexcept;
    /**
     * @brief store if the reload thread should be active
     */
    bool m_runReloadThread = true;
    /**
     * @brief store the thread for reloading assets that are out of date
     */
    std::thread m_reloadThread;

};

#endif //end of C++ specific section

#endif