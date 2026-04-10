/**
 * @file Asset.h
 * @author DM8AT
 * @brief define what an asset must contain
 * @version 0.1
 * @date 2026-01-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_CORE_ASSET_
#define _GLGE_CORE_ASSET_

//add common stuff
#include "Core/Common.h"
//add the UUID system
#include "Core/UUID.h"
//add the filesystem API
#include <filesystem>
//for shared locks
#include <shared_mutex>
//add atomics
#include <atomic>
//add exceptions
#include "Exception.h"
//add the type info
#include "TypeInfo.h"

//use the library namespace
namespace GLGE {

    /**
     * @brief a base class that all assets must implement
     */
    class Asset {
    public:

        /**
         * @brief define the type of asset UUIDs
         */
        using UUID = u64;

        /**
         * @brief Destroy the Asset
         */
        virtual ~Asset() = default;

        /**
         * @brief load the asset from raw safe data
         * 
         * @param data the raw binary data to load the asset from
         * @return `u64` the amount of loaded bytes
         */
        virtual u64 load(const std::vector<u8>& data) = 0;

        /**
         * @brief store the asset in binary
         * 
         * @warning do NOT assume that data is empty at the start. It should be APPENDED and not overridden. 
         * 
         * @param data a vector to append the raw binary data to
         */
        virtual void store(std::vector<u8>& data) = 0;

        /**
         * @brief import the asset from a stand-alone file
         * 
         * @param file the file to import the asset from
         * @param format the format of the asset
         */
        virtual void import_from(const std::filesystem::path& file, u32 format) noexcept(false) = 0;

        /**
         * @brief export the asset in a specific file format
         * 
         * @param file a path to the file to store the asset into
         * @param format the file format to use
         */
        virtual void export_as(const std::filesystem::path& file, u32 format) noexcept(false) = 0;

        /**
         * @brief get the assets UUID
         * 
         * @return `UUID` the UUID of the asset
         */
        inline UUID getUUID() const noexcept {
            GLGE_PROFILER_SCOPE();
            return m_uuid;
        }

        /**
         * @brief Get the hash of the asset type
         * 
         * @return `u64` the hash of the asset type
         */
        inline u64 getType() const noexcept {
            GLGE_PROFILER_SCOPE();
            return m_typeHash;
        }

    private:

        /**
         * @brief give access to the asset manager
         */
        friend class AssetManager;
        //asset handles are friends
        template <typename T1>
        friend class AssetHandle;

        /**
         * @brief a function to fully setup the asset
         * 
         * @warning automatically called by the asset manager
         * 
         * @param typeHash the type hash of the asset type
         */
        void __make_valid(u64 typeHash) {
            GLGE_PROFILER_SCOPE();
            m_uuid = ms_registry.generate();
            m_typeHash = typeHash;
        }

        /**
         * @brief store the asset's uuid registry
         */
        inline static UUIDRegistry ms_registry;

        /**
         * @brief store the asset's UUID
         */
        UUID m_uuid;
        /**
         * @brief store the type hash
         */
        u64 m_typeHash = 0;
        /**
         * @brief store the reference count
         */
        std::atomic_uint64_t m_references = 0;

    };

    //asset managers are defined later
    class AssetManager;
    //asset handles are defined later
    template <typename T>
    class AssetHandle;

    /**
     * @brief a structure to refer to an asset
     * 
     * This structure makes using an asset safe
     * 
     * @tparam T the type of asset to refer to
     */
    template <typename T>
    class AssetReference {
    public:

        /**
         * @brief Destroy the Asset Reference
         */
        ~AssetReference() = default;

        /**
         * @brief Get the Asset
         * 
         * @return `T&` a reference to the stored asset
         */
        inline T& getAsset() noexcept {
            GLGE_PROFILER_SCOPE();
            return *m_asset;
        }

        /**
         * @brief Get the Asset
         * 
         * @return `T&` a reference to the stored asset
         */
        inline T& operator*() noexcept {
            GLGE_PROFILER_SCOPE();
            return *m_asset;
        }

        /**
         * @brief Get the Asset
         * 
         * @return `T&` a reference to the stored asset
         */
        inline T* operator->() noexcept {
            GLGE_PROFILER_SCOPE();
            return m_asset;
        }

        /**
         * @brief Get the Asset
         * 
         * @return `const T&` a constant reference to the stored asset
         */
        inline const T& getAsset() const noexcept {
            GLGE_PROFILER_SCOPE();
            return *m_asset;
        }

        /**
         * @brief Get the Asset
         * 
         * @return `const T&` a constant reference to the stored asset
         */
        inline const T& operator*() const noexcept {
            GLGE_PROFILER_SCOPE();
            return *m_asset;
        }

        /**
         * @brief Get the Asset
         * 
         * @return `const T&` a constant reference to the stored asset
         */
        inline const T* operator->() const noexcept {
            GLGE_PROFILER_SCOPE();
            return m_asset;
        }

    protected:

        //asset handles are friends
        template <typename T1>
        friend class AssetHandle;

        /**
         * @brief Construct a new Asset Reference
         * 
         * @param handle a pointer to the handle the reference belongs to
         */
        AssetReference(AssetHandle<T>* handle);

        /**
         * @brief store the handle the reference was created from
         */
        AssetHandle<T>* m_handle = nullptr;
        /**
         * @brief store the asset the reference holds
         * 
         * `nullptr` means invalid
         */
        T* m_asset = nullptr;
        /**
         * @brief store the own lock on the asset
         */
        std::shared_lock<std::shared_mutex> m_lock;

    };

    /**
     * @brief store a reference to an asset
     * 
     * @tparam `T` the type of asset the handle is pointing to
     */
    template <typename T>
    class AssetHandle {
    public:

        /**
         * @brief Construct a new Asset Handle
         * 
         * @param other the handle to copy
         */
        AssetHandle(const AssetHandle<T>& other);

        /**
         * @brief Construct a new Asset Handle
         * 
         * @param other the handle to move from
         */
        AssetHandle(AssetHandle<T>&& other);

        /**
         * @brief copy an asset handle
         * 
         * @param other the asset handle to copy
         * @return `AssetHandle&` a reference to the filled out handle
         */
        AssetHandle<T>& operator=(const AssetHandle<T>& other);

        /**
         * @brief move an asset handle
         * 
         * @param other the asset handle to move from
         * @return `AssetHandle&` a reference to this asset handle after it is filled out
         */
        AssetHandle<T>& operator=(AssetHandle<T>&& other);

        /**
         * @brief Destroy the Asset Handle
         */
        ~AssetHandle() {decrement();}

        /**
         * @brief get the UUID of the asset
         * 
         * @return `UUID` the UUID of the asset the handle refers to
         */
        inline UUID getUUID() const noexcept {
            GLGE_PROFILER_SCOPE();
            return m_uuid;
        }

        /**
         * @brief Get the Asset Manager of the asset
         * 
         * @return `AssetManager*` a pointer to the asset manager used by the asset the asset handle references. `nullptr` means that the handle is invalid. 
         */
        inline AssetManager* getAssetManager() const noexcept {
            GLGE_PROFILER_SCOPE();
            return m_manager;
        }

        /**
         * @brief Get the type hash of the stored assets type
         * 
         * @return `u64` the hashed type of the stored asset
         */
        inline u64 getType() const noexcept {
            GLGE_PROFILER_SCOPE();
            return m_type;
        }

        /**
         * @brief refer to the asset the handle holds
         * 
         * @tparam T the type of the asset to refer to
         * @return `AssetReference<T>` A rvalue to the new asset reference
         */
        inline AssetReference<T> reference() {
            GLGE_PROFILER_SCOPE();
            //in debug to type checking
            #if GLGE_DEBUG
            if (getTypeHash64<T>() != m_type)
            {throw Exception("Type mismatch detected while referencing an asset", "GLGE::AssetHandle::reference");}
            #endif
            //create the new asset handle
            return AssetReference<T>(this);
        }

        /**
         * @brief check if the asset handle is valid
         * 
         * @return `true` if the handle is valid, `false` if not
         */
        inline bool isValid() const noexcept {
            GLGE_PROFILER_SCOPE();
            return m_manager != nullptr;
        }

        /**
         * @brief check if the asset is valid
         * 
         * @return `true` if the handle is valid, `false` if not
         */
        inline operator bool()  const noexcept {
            GLGE_PROFILER_SCOPE();
            return isValid();
        }

    private:

        //asset managers are friends
        friend class AssetManager;
        //asset references are friends
        template <typename T1>
        friend class AssetReference;

        /**
         * @brief Construct a new Asset Handle
         * 
         * @warning only the asset manager may create a handle
         * 
         * @param uuid the universally unique ID of the stored asset
         * @param type the type hash of the used type
         * @param manager a pointer to the asset manager that created the handle
         */
        AssetHandle(UUID uuid, u64 type, AssetManager* manager);

        /**
         * @brief decrement the reference count of the asset and potentially clean it up
         */
        void decrement();

        /**
         * @brief store the UUID of the asset the handle refers to
         */
        UUID m_uuid = 0;
        /**
         * @brief store the asset manager of the asset the handle refers to
         * 
         * if `m_manager == nullptr`, the handle is invalid
         */
        AssetManager* m_manager = nullptr;
        /**
         * @brief store the type hash of the asset's type
         */
        u64 m_type = 0;

    };

}

#endif