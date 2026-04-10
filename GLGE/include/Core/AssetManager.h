/**
 * @file AssetManager.h
 * @author DM8AT
 * @brief define a class to manage assets
 * @version 0.1
 * @date 2026-02-03
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_CORE_ASSET_MANAGER_
#define _GLGE_CORE_ASSET_MANAGER_

//add assets
#include "Asset.h"
//add type stuff
#include "TypeInfo.h"
//for thread safety
#include <shared_mutex>
#include <atomic>

//add the profiler
#include "Profiler.h"

//use the library namespace
namespace GLGE {

    /**
     * @brief a class to create and store all assets
     */
    class AssetManager {
    public:

        //no movement nor copy -> stable pointers
        AssetManager(const AssetManager&) = delete;
        AssetManager(AssetManager&&) = delete;
        AssetManager& operator=(const AssetManager&) = delete;
        AssetManager& operator=(AssetManager&&) = delete;

        /**
         * @brief Construct a new Asset Manager
         */
        AssetManager() = default;

        /**
         * @brief load a new asset
         * 
         * @tparam `T` the type of the asset to load
         * @param from the path to load the asset from
         * @param format the format of the asset file (default is 0, meaning GLGE-Custom)
         * @return `AssetHandle<T>`a handle to the loaded asset
         */
        template <typename T>
        requires std::is_base_of_v<Asset, T> && std::is_default_constructible_v<T>
        AssetHandle<T> load(const std::filesystem::path& from, u32 format = 0) {
            GLGE_PROFILER_SCOPE();
            constexpr u64 type_hash = getTypeHash64<T>();
            //check if the type exists
            auto it = m_typeStorage.find(type_hash);
            //if the element does not exist, add it
            if (it == m_typeStorage.end()) {
                //lock and re-check (to avoid race condition)
                std::unique_lock lock(m_typeLock);
                it = m_typeStorage.find(type_hash);
                if (it == m_typeStorage.end()) {
                    m_typeStorage.insert_or_assign(type_hash, static_cast<void*>(new TypeStorage<T>{}));
                    it = m_typeStorage.find(type_hash);
                }
            }
            TypeStorage<T>* storage = static_cast<TypeStorage<T>*>(it->second);
            UUID uuid;
            {
                //make sure to obtain a lock
                std::unique_lock lock(storage->mtx);

                //now safe to work on the data

                //add the new element to the back
                storage->assets.push_back(new T());
                storage->assets.back()->__make_valid(type_hash);
                storage->assets.back()->import_from(from, format);
                uuid = storage->assets.back()->getUUID();
                storage->uuid_to_index.insert_or_assign(uuid, storage->assets.size()-1);
            }

            //return a new handle to it
            return AssetHandle<T>(uuid, type_hash, this);
        }

        /**
         * @brief Get the all assets of a specific type
         * 
         * @warning if `T` is not a known type, this function results in erroneous behaviour
         * 
         * @tparam `T` the type to quarry all assets for
         * @return `const std::vector<T*>&` a vector with all assets of that type
         */
        template <typename T>
        const std::vector<T*>& getAllOfType() const {
            GLGE_PROFILER_SCOPE();
            return static_cast<TypeStorage<T>*>(m_typeStorage.at(getTypeHash64<T>()))->assets;
        }

        /**
         * @brief check if a specific type is known
         * 
         * @tparam `T`the type to check if it is known
         * @return `true` if the type is known, `false` if not
         */
        template <typename T>
        inline bool hasType() const noexcept {
            GLGE_PROFILER_SCOPE();
            return m_typeStorage.find(getTypeHash64<T>()) != m_typeStorage.end();
        }

    protected:

        //asset handles are friends
        template <typename T1>
        friend class AssetHandle;
        //asset references are friends
        template <typename T2>
        friend class AssetReference;

        /**
         * @brief store assets and enable iteration + UUID lookup
         * 
         * @tparam `T` the type of asset to store
         */
        template <typename T>
        class TypeStorage final {
        public:

            /**
             * @brief store the actual assets
             */
            std::vector<T*> assets;

            /**
             * @brief mutex for operations on the type data
             */
            std::shared_mutex mtx;

            /**
             * @brief map uuids to indices
             */
            std::unordered_map<UUID, size_t> uuid_to_index;

        };

        /**
         * @brief a lock to make the type storage thread safe
         */
        std::shared_mutex m_typeLock;
        /**
         * @brief store typed asset storages
         */
        std::unordered_map<u64, void*> m_typeStorage;

    };

}

//fully implement asset reference system
#include "AssetReference.inl"

#endif