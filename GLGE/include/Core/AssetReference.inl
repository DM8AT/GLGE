/**
 * @file AssetReference.inl
 * @author DM8AT
 * @brief implement the asset manager system. This must be included after asset manager exists. 
 * @version 0.1
 * @date 2026-02-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_CORE_ASSET_REFERENCE_
#define _GLGE_CORE_ASSET_REFERENCE_

#include "AssetManager.h"

//use the library namespace
namespace GLGE {

    //implement the constructor
    template <typename T>
    AssetReference<T>::AssetReference(AssetHandle<T>* handle) 
     : m_handle(handle)
    {
        GLGE_PROFILER_SCOPE();
        //lock type map
        std::shared_lock lock(m_handle->m_data.manager->m_typeLock);
        //quarry the specific type storage
        auto it = m_handle->m_data.manager->m_typeStorage.find(m_handle->m_data.type);
        if (it == m_handle->m_data.manager->m_typeStorage.end())
        {throw Exception("Referencing an invalid asset, asset reference could not be created", "AssetReference<T>::AssetReference");}
        AssetManager::TypeStorage<T>* storage = reinterpret_cast<AssetManager::TypeStorage<T>*>(it->second);
        //lock the asset
        m_lock = std::shared_lock(storage->mtx);
        //quarry by UUID, store the asset if valid
        auto uit = storage->uuid_to_index.find(m_handle->m_data.uuid);
        if (uit != storage->uuid_to_index.end()) {
            m_asset = storage->assets.at(uit->second);
            //check if the asset is being destroyed
            if (static_cast<Asset*>(m_asset)->m_destroying.load(std::memory_order_acquire))
            {throw Exception("Trying to reference an asset that is currently being destroyed", "AssetReference<T>::AssetReference");}
        }
        else 
        {throw Exception("Referencing an invalid asset, asset reference could not be created", "AssetReference<T>::AssetReference");}
    }



    //implement the normal constructor
    template <typename T>
    AssetHandle<T>::AssetHandle(UUID uuid, u64 type, AssetManager* manager) 
     : m_data(AssetHandleData{
        .uuid = uuid,
        .manager = manager,
        .type = type
     })
    {
        GLGE_PROFILER_SCOPE();
        //lock type map
        std::shared_lock lock(m_data.manager->m_typeLock);
        //register that the handle exists
        auto it = m_data.manager->m_typeStorage.find(m_data.type);
        if (it == m_data.manager->m_typeStorage.end()) {
            m_data.manager = nullptr;
            return;
        }
        AssetManager::TypeStorage<T>* storage = (AssetManager::TypeStorage<T>*)(it->second);
        std::shared_lock lock2(storage->mtx);
        auto ait = storage->uuid_to_index.find(m_data.uuid);
        if (ait == storage->uuid_to_index.end()) {
            m_data.manager = nullptr;
            return;
        }
        //check if the asset is being destroyed
        if (static_cast<Asset*>(storage->assets[ait->second])->m_destroying.load(std::memory_order_acquire)) {
            m_data.manager = nullptr;
            return;
        }
        storage->assets[ait->second]->m_references.fetch_add(1, std::memory_order_acq_rel);
    }

    //implement the copy constructor
    template <typename T>
    AssetHandle<T>::AssetHandle(const AssetHandle<T>& other) 
     : m_data(other.m_data)
    {
        GLGE_PROFILER_SCOPE();
        //do NOT decrement the old asset - this handle did not exist before this, no old reference to clean

        //early-path for invalid other
        if (other.m_data.manager == nullptr) {
            //just clear this one too
            m_data.manager = nullptr;
            m_data.type = 0;
            m_data.uuid = 0;
            return;
        }

        //lock type map
        std::shared_lock lock(m_data.manager->m_typeLock);
        //register that the handle exists
        auto it = m_data.manager->m_typeStorage.find(m_data.type);
        if (it == m_data.manager->m_typeStorage.end()) {
            m_data.manager = nullptr;
            return;
        }
        AssetManager::TypeStorage<T>* storage = (AssetManager::TypeStorage<T>*)(it->second);
        std::shared_lock lock2(storage->mtx);
        auto ait = storage->uuid_to_index.find(m_data.uuid);
        if (ait == storage->uuid_to_index.end()) {
            m_data.manager = nullptr;
            return;
        }
        //check if the asset is being destroyed
        if (static_cast<Asset*>(storage->assets[ait->second])->m_destroying.load(std::memory_order_acquire)) {
            m_data.manager = nullptr;
            return;
        }
        static_cast<Asset*>(storage->assets[ait->second])->m_references.fetch_add(1, std::memory_order_acq_rel);
    }

    //implement the move constructor
    template <typename T>
    AssetHandle<T>::AssetHandle(AssetHandle<T>&& other)
     : m_data(other.m_data)
    {
        GLGE_PROFILER_SCOPE();
        //handle count is kept the same

        //invalidate the handle
        other.m_data.manager = nullptr;
        other.m_data.type = 0;
        other.m_data.uuid = 0;
    }

    //implement the copy asign operator
    template <typename T>
    AssetHandle<T>& AssetHandle<T>::operator=(const AssetHandle<T>& other) {
        GLGE_PROFILER_SCOPE();
        //prevent self-assignment
        if (this == &other) {return *this;}
        //decrement old asset
        decrement();

        //early path for invalid other
        if (other.m_data.manager == nullptr) {
            //just clear this one too
            m_data.manager = nullptr;
            m_data.type = 0;
            m_data.uuid = 0;
            return *this;
        }

        //get the data
        m_data = other.m_data;

        //lock type map
        std::shared_lock lock(m_data.manager->m_typeLock);
        //register that the handle exists
        auto it = m_data.manager->m_typeStorage.find(m_data.type);
        if (it == m_data.manager->m_typeStorage.end()) {
            m_data.manager = nullptr;
            return *this;
        }
        AssetManager::TypeStorage<T>* storage = (AssetManager::TypeStorage<T>*)(it->second);
        std::shared_lock lock2(storage->mtx);
        auto ait = storage->uuid_to_index.find(m_data.uuid);
        if (ait == storage->uuid_to_index.end()) {
            m_data.manager = nullptr;
            return *this;
        }
        //check if the asset is being destroyed
        if (static_cast<Asset*>(storage->assets[ait->second])->m_destroying.load(std::memory_order_acquire)) {
            m_data.manager = nullptr;
            return *this;
        }
        static_cast<Asset*>(storage->assets[ait->second])->m_references.fetch_add(1, std::memory_order_acq_rel);

        //return self
        return *this;
    }

    //implement the move asign operator
    template <typename T>
    AssetHandle<T>& AssetHandle<T>::operator=(AssetHandle<T>&& other) {
        GLGE_PROFILER_SCOPE();
        //prevent self-assignment
        if (this == &other) {return *this;}
        //decrement old asset
        decrement();

        //get the data
        m_data = other.m_data;

        //handle count is kept the same

        //invalidate other
        other.m_data.manager = nullptr;
        other.m_data.type = 0;
        other.m_data.uuid = 0;

        //return self
        return *this;
    }

    //implement the destructor
    template<typename T>
    void AssetHandle<T>::decrement() {
        if (!isValid()) return;
        GLGE_PROFILER_SCOPE();

        //store the potential asset to erase
        //this must happen outside the locked scope
        T* ass = nullptr;
        {
        std::unique_lock typeLock(m_data.manager->m_typeLock);

        auto it = m_data.manager->m_typeStorage.find(m_data.type);
        if (it == m_data.manager->m_typeStorage.end()) return;

        auto* storage = static_cast<AssetManager::TypeStorage<T>*>(it->second);

        //lock storage after type lock
        std::unique_lock storageLock(storage->mtx);

        //get the index of the asset using the UUID
        auto ait = storage->uuid_to_index.find(m_data.uuid);
        if (ait == storage->uuid_to_index.end()) return;

        //get the asset
        T* asset = storage->assets[ait->second];
        Asset* a = static_cast<Asset*>(asset);

        //reject multi-deletion
        if (a->m_destroying.load(std::memory_order_acquire)) {
            m_data.manager = nullptr;
            m_data.type = 0;
            m_data.uuid = 0;
            return;
        }

        //decrement reference count, stop if too large
        if (a->m_references.fetch_sub(1) != 1) {
            m_data.manager = nullptr;
            m_data.type = 0;
            m_data.uuid = 0;
            return;
        }

        //refcount is now 0, so erase asset
        ass = asset;
        ass->m_destroying.store(true, std::memory_order_release);

        //drop locks
        }

        //stop if nothing to erase
        if (ass == nullptr) {return;}

        //it is now safe to delete the asset (no locks exist)
        delete ass;

        {
        //re-acquire locks
        std::unique_lock typeLock(m_data.manager->m_typeLock);
        auto it = m_data.manager->m_typeStorage.find(m_data.type);
        if (it == m_data.manager->m_typeStorage.end()) return;
        auto* storage = static_cast<AssetManager::TypeStorage<T>*>(it->second);
        std::unique_lock storageLock(storage->mtx);

        //index must be re-queried, it may have changed
        auto ait = storage->uuid_to_index.find(m_data.uuid);
        if (ait == storage->uuid_to_index.end()) return;

        size_t index = ait->second;
        size_t last = storage->assets.size() - 1;

        if (index != last) {
            UUID lastUUID = storage->assets[last]->getUUID();
            std::swap(storage->assets[index], storage->assets[last]);
            storage->uuid_to_index[lastUUID] = index;
        }
        //asset is now at the back, remove the back
        storage->assets.pop_back();
        storage->uuid_to_index.erase(m_data.uuid);
        }

        //clean up
        m_data.manager = nullptr;
        m_data.type = 0;
        m_data.uuid = 0;

        //do NOT delete storage here, it's too dangerous
    }

}

#endif