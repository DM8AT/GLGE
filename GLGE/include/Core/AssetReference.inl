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
        std::shared_lock lock(m_handle->m_manager->m_typeLock);
        //quarry the specific type storage
        auto it = m_handle->m_manager->m_typeStorage.find(m_handle->m_type);
        if (it == m_handle->m_manager->m_typeStorage.end())
        {throw Exception("Referencing an invalid asset, asset reference could not be created", "AssetReference<T>::AssetReference");}
        AssetManager::TypeStorage<T>* storage = reinterpret_cast<AssetManager::TypeStorage<T>*>(it->second);
        //lock the asset
        m_lock = std::shared_lock(storage->mtx);
        //quarry by UUID, store the asset if valid
        auto uit = storage->uuid_to_index.find(m_handle->m_uuid);
        if (uit != storage->uuid_to_index.end()) {
            m_asset = storage->assets.at(uit->second);
        }
        else 
        {throw Exception("Referencing an invalid asset, asset reference could not be created", "AssetReference<T>::AssetReference");}
    }



    //implement the normal constructor
    template <typename T>
    AssetHandle<T>::AssetHandle(UUID uuid, u64 type, AssetManager* manager) 
     : m_uuid(uuid), m_manager(manager), m_type(type)
    {
        GLGE_PROFILER_SCOPE();
        //lock type map
        std::shared_lock lock(m_manager->m_typeLock);
        //register that the handle exists
        auto it = m_manager->m_typeStorage.find(m_type);
        if (it == m_manager->m_typeStorage.end()) {
            m_manager = nullptr;
            return;
        }
        AssetManager::TypeStorage<T>* storage = (AssetManager::TypeStorage<T>*)(it->second);
        std::shared_lock lock2(storage->mtx);
        auto ait = storage->uuid_to_index.find(m_uuid);
        if (ait == storage->uuid_to_index.end()) {
            m_manager = nullptr;
            return;
        }
        storage->assets[ait->second]->m_references.fetch_add(1, std::memory_order_acq_rel);
    }

    //implement the copy constructor
    template <typename T>
    AssetHandle<T>::AssetHandle(const AssetHandle<T>& other) 
     : m_uuid(other.m_uuid), m_manager(other.m_manager), m_type(other.m_type)
    {
        GLGE_PROFILER_SCOPE();
        //do NOT decrement the old asset - this handle did not exist before this, no old reference to clean

        //early-path for invalid other
        if (other.m_manager == nullptr) {
            //just clear this one too
            m_manager = nullptr;
            m_type = 0;
            m_uuid = 0;
            return;
        }

        //lock type map
        std::shared_lock lock(m_manager->m_typeLock);
        //register that the handle exists
        auto it = m_manager->m_typeStorage.find(m_type);
        if (it == m_manager->m_typeStorage.end()) {
            m_manager = nullptr;
            return;
        }
        AssetManager::TypeStorage<T>* storage = (AssetManager::TypeStorage<T>*)(it->second);
        std::shared_lock lock2(storage->mtx);
        auto ait = storage->uuid_to_index.find(m_uuid);
        if (ait == storage->uuid_to_index.end()) {
            m_manager = nullptr;
            return;
        }
        ((Asset*)&storage->assets[ait->second])->m_references.fetch_add(1, std::memory_order_acq_rel);
    }

    //implement the move constructor
    template <typename T>
    AssetHandle<T>::AssetHandle(AssetHandle<T>&& other)
     : m_uuid(other.m_uuid), m_manager(other.m_manager), m_type(other.m_type) 
    {
        GLGE_PROFILER_SCOPE();
        //handle count is kept the same

        //invalidate the handle
        other.m_manager = nullptr;
        other.m_type = 0;
        other.m_uuid = 0;
    }

    //implement the copy asign operator
    template <typename T>
    AssetHandle<T>& AssetHandle<T>::operator=(const AssetHandle<T>& other) {
        GLGE_PROFILER_SCOPE();
        //decrement old asset
        decrement();

        //early path for invalid other
        if (other.m_manager == nullptr) {
            //just clear this one too
            m_manager = nullptr;
            m_type = 0;
            m_uuid = 0;
            return *this;
        }

        //get the data
        m_uuid = other.m_uuid;
        m_manager = other.m_manager;
        m_type = other.m_type;

        //lock type map
        std::shared_lock lock(m_manager->m_typeLock);
        //register that the handle exists
        auto it = m_manager->m_typeStorage.find(m_type);
        if (it == m_manager->m_typeStorage.end()) {
            m_manager = nullptr;
            return *this;
        }
        AssetManager::TypeStorage<T>* storage = (AssetManager::TypeStorage<T>*)(it->second);
        std::shared_lock lock2(storage->mtx);
        auto ait = storage->uuid_to_index.find(m_uuid);
        if (ait == storage->uuid_to_index.end()) {
            m_manager = nullptr;
            return *this;
        }
        ((Asset*)&storage->assets[ait->second])->m_references.fetch_add(1, std::memory_order_acq_rel);

        //return self
        return *this;
    }

    //implement the move asign operator
    template <typename T>
    AssetHandle<T>& AssetHandle<T>::operator=(AssetHandle<T>&& other) {
        GLGE_PROFILER_SCOPE();
        //decrement old asset
        decrement();

        //get the data
        m_uuid = other.m_uuid;
        m_manager = other.m_manager;
        m_type = other.m_type;

        //handle count is kept the same

        //invalidate other
        other.m_manager = nullptr;
        other.m_type = 0;
        other.m_uuid = 0;

        //return self
        return *this;
    }

    //implement the destructor
    template<typename T>
    void AssetHandle<T>::decrement() {
        GLGE_PROFILER_SCOPE();
        if (!isValid()) return;

        std::unique_lock typeLock(m_manager->m_typeLock);

        auto it = m_manager->m_typeStorage.find(m_type);
        if (it == m_manager->m_typeStorage.end()) return;

        auto* storage = static_cast<AssetManager::TypeStorage<T>*>(it->second);

        //lock storage after type lock
        std::unique_lock storageLock(storage->mtx);

        auto ait = storage->uuid_to_index.find(m_uuid);
        if (ait == storage->uuid_to_index.end()) return;

        Asset& a = *storage->assets[ait->second];

        if (a.m_references.fetch_sub(1) != 1)
            return;

        //refcount is now 0, so erase asset

        size_t index = ait->second;
        size_t last = storage->assets.size() - 1;

        if (index != last) {
            UUID lastUUID = storage->assets[last]->getUUID();
            std::swap(storage->assets[index], storage->assets[last]);
            storage->uuid_to_index[lastUUID] = index;
        }

        delete storage->assets.back();
        storage->assets.pop_back();
        storage->uuid_to_index.erase(m_uuid);

        //do NOT delete storage here, it's too dangerous
    }

}

#endif