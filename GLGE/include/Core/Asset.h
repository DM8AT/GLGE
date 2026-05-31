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
//add recursive thread shared mutex
#include "utils/RecursiveThreadMutexShared.h"

//use the library namespace
namespace GLGE {

    //asset managers are defined later
    class AssetManager;
    //asset handles are defined later
    template <typename T>
    class AssetHandle;

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
         * @param manager a pointer to the asset manager used for loading
         * @param data the raw binary data to load the asset from
         * @return `u64` the amount of loaded bytes
         */
        virtual u64 load(AssetManager* manager, const std::vector<u8>& data) = 0;

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
         * @param manager a pointer to the asset manager used for loading
         * @param file the file to import the asset from
         * @param format the format of the asset
         */
        virtual void import_from(AssetManager* manager, const std::filesystem::path& file, u32 format) noexcept(false) = 0;

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
        //asset references are friends
        template <typename T2>
        friend class AssetReference;

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
        /**
         * @brief store if the asset is currently being destroyed
         */
        std::atomic_bool m_destroying{false};

    };

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
        std::shared_lock<RecursiveThreadMutexShared> m_lock;

    };

    /**
     * @brief store the type-independent data
     */
    struct AssetHandleData {
        /**
         * @brief store the UUID of the asset the handle refers to
         */
        UUID uuid = 0;
        /**
         * @brief store the asset manager of the asset the handle refers to
         * 
         * if `manager == nullptr`, the handle is invalid
         */
        AssetManager* manager = nullptr;
        /**
         * @brief store the type hash of the asset's type
         */
        u64 type = 0;
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
         * The handle will be invalid
         */
        AssetHandle() = default;

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
            return m_data.uuid;
        }

        /**
         * @brief Get the Asset Manager of the asset
         * 
         * @return `AssetManager*` a pointer to the asset manager used by the asset the asset handle references. `nullptr` means that the handle is invalid. 
         */
        inline AssetManager* getAssetManager() const noexcept {
            GLGE_PROFILER_SCOPE();
            return m_data.manager;
        }

        /**
         * @brief Get the type hash of the stored assets type
         * 
         * @return `u64` the hashed type of the stored asset
         */
        inline u64 getType() const noexcept {
            GLGE_PROFILER_SCOPE();
            return m_data.type;
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
            if (getTypeHash64<T>() != m_data.type)
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
            return m_data.manager != nullptr;
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
         * @brief say that no reference counting should be used
         */
        struct NoRefCountTag {u8 blob = 0;};

        /**
         * @brief Construct a new Asset Handle
         * 
         * @param data the data to create from
         */
        AssetHandle(AssetHandleData data, NoRefCountTag)
         : m_data(data)
        {}

        /**
         * @brief Construct a new Asset Handle
         * 
         * @param data the data to create from
         */
        AssetHandle(AssetHandleData data)
         : AssetHandle(data.uuid, data.type, data.manager)
        {}

        /**
         * @brief decrement the reference count of the asset and potentially clean it up
         */
        void decrement();

        //untyped asset handles are friends
        friend class UntypedAssetHandle;

        /**
         * @brief store the actual data
         */
        AssetHandleData m_data;

    };

    /**
     * @brief a wrapper for an asset handle that can be stored without a template
     */
    class UntypedAssetHandle {
    public:

        /**
         * @brief Construct a new Untyped Asset Handle
         * 
         * The created untyped handle is invalid
         */
        UntypedAssetHandle() = default;

        /**
         * @brief Construct a new Asset Handle
         * 
         * @tparam T the type of the asset handle to move from
         * @param other the handle to copy
         */
        template <typename T>
        UntypedAssetHandle(const AssetHandle<T>& other) {
            //create the lambdas
            __createLambdas<T>();
            //use the copy lambda
            (*m_copy)(&m_data, &other.m_data);
        }

        /**
         * @brief Construct a new Asset Handle
         * 
         * @tparam T the type of the asset handle to move from
         * @param other the handle to move from
         */
        template <typename T>
        UntypedAssetHandle(AssetHandle<T>&& other) {
            //create the lambdas
            __createLambdas<T>();
            //use the move lambda
            (*m_move)(&m_data, &other.m_data);
        }

        /**
         * @brief copy an asset handle
         * 
         * @tparam T the type of the asset handle to move from
         * @param other the asset handle to copy
         * @return `UntypedAssetHandle&` a reference to the filled out handle
         */
        template <typename T>
        UntypedAssetHandle& operator=(const AssetHandle<T>& other) {
            //clean up the old handle
            if (m_destroy)
            {(*m_destroy)(&m_data);}
            //then, create the new lambdas
            __createLambdas<T>();
            //just use the copy function
            (*m_copy)(&m_data, &other.m_data);
            //return a reference to this
            return *this;
        }

        /**
         * @brief move an asset handle
         * 
         * @tparam T the type of the asset handle to move from
         * @param other the asset handle to move from
         * @return `UntypedAssetHandle&` a reference to this asset handle after it is filled out
         */
        template <typename T>
        UntypedAssetHandle& operator=(AssetHandle<T>&& other) {
            //clean up the old handle
            if (m_destroy)
            {(*m_destroy)(&m_data);}
            //then, create the new lambdas
            __createLambdas<T>();
            //just use the move function
            (*m_move)(&m_data, &other.m_data);
            //return a reference to this
            return *this;
        }

        /**
         * @brief Construct a new Asset Handle
         * 
         * @param other the handle to copy
         */
        UntypedAssetHandle(const UntypedAssetHandle& other) {
            //copy the lambdas from the other handle
            m_move = other.m_move;
            m_copy = other.m_copy;
            m_destroy = other.m_destroy;
            //then, use the copy call
            if (m_copy)
            {(*m_copy)(&m_data, &other.m_data);}
        }

        /**
         * @brief Construct a new Asset Handle
         * 
         * @param other the handle to move from
         */
        UntypedAssetHandle(UntypedAssetHandle&& other) {
            //copy the lambdas from the other handle
            m_move = other.m_move;
            m_copy = other.m_copy;
            m_destroy = other.m_destroy;
            //then, use the copy call
            if (m_move)
            {(*m_move)(&m_data, &other.m_data);}
        }

        /**
         * @brief copy an asset handle
         * 
         * @param other the asset handle to copy
         * @return `UntypedAssetHandle&` a reference to the filled out handle
         */
        UntypedAssetHandle& operator=(const UntypedAssetHandle& other) {
            //stop self assignment
            if (this == &other) {return *this;}
            //clean up the old handle
            if (m_destroy)
            {(*m_destroy)(&m_data);}
            //copy the lambdas from the other handle
            m_move = other.m_move;
            m_copy = other.m_copy;
            m_destroy = other.m_destroy;
            //then, use the copy call
            if (m_copy)
            {(*m_copy)(&m_data, &other.m_data);}
            //return a reference to this
            return *this;
        }

        /**
         * @brief move an asset handle
         * 
         * @param other the asset handle to move from
         * @return `UntypedAssetHandle&` a reference to this asset handle after it is filled out
         */
        UntypedAssetHandle& operator=(UntypedAssetHandle&& other) {
            //stop self assignment
            if (this == &other) {return *this;}
            //clean up the old handle
            if (m_destroy)
            {(*m_destroy)(&m_data);}
            //copy the lambdas from the other handle
            m_move = other.m_move;
            m_copy = other.m_copy;
            m_destroy = other.m_destroy;
            //then, use the move call
            if (m_move)
            {(*m_move)(&m_data, &other.m_data);}
            //return a reference to this
            return *this;
        }

        /**
         * @brief Destroy the Asset Handle
         */
        ~UntypedAssetHandle() {
            //if a destructor exists, delete it
            if (m_destroy) 
            {(*m_destroy)(&m_data);}
        }

        /**
         * @brief Get the typed asset handle
         * 
         * This creates a new asset handle. 
         * 
         * @tparam T the type of the handle to get
         * @return `AssetHandle<T>` the typed handle
         */
        template <typename T>
        AssetHandle<T> getTyped() {
            //if the types match, return the handle
            if (m_data.type == getTypeHash64<T>())
            {return AssetHandle<T>(m_data);}
            //return an invalid handle
            return AssetHandle<T>();
        }

        /**
         * @brief Get the type hash
         * 
         * @return `u64` the type hash
         */
        u64 getType() const noexcept
        {return m_data.type;}

        /**
         * @brief check if the handle is valid
         * 
         * @return `true` if the handle is valid, `false` if not
         */
        inline bool isValid() const noexcept
        {return m_data.manager != nullptr;}

        /**
         * @brief check if the handle is valid
         * 
         * @return `true` if the handle is valid, `false` if not
         */
        inline operator bool() const noexcept
        {return isValid();}

    protected:

        /**
         * @brief store the data
         */
        AssetHandleData m_data;

        /**
         * @brief create all lambda functions
         * 
         * @tparam T the type of the original handle
         */
        template <typename T>
        void __createLambdas() {
            m_move = [](AssetHandleData* destination, AssetHandleData* source) {
                //just copy the type-erased data
                *destination = *source;

                //clear the from data
                source->uuid = 0;
                source->manager = nullptr;
                source->type = 0;

                //=> +/- 0
            };
            m_copy = [](AssetHandleData* destination, const AssetHandleData* source) {
                //create the handles
                AssetHandle<T> src(*source, typename AssetHandle<T>::NoRefCountTag{}); //+0
                AssetHandle<T> dst(src);                                             //+1

                //copy data over
                *destination = dst.m_data;

                //invalidate the managers
                //setting the manager to `nullptr` prevents the manager from updating the refcount
                //since the refcount lives in the asset manager
                dst.m_data.manager = nullptr;
                src.m_data.manager = nullptr;
                // +/- 0

                //=> Total +1
            };
            m_destroy = [](AssetHandleData* data) {
                //stop if the data is invalid
                if (!data->manager)
                {return;}

                //else, create the handle
                AssetHandle<T> handle(*data, typename AssetHandle<T>::NoRefCountTag{}); //+0

                //clear the handle
                data->uuid = 0;
                data->manager = nullptr;
                data->type = 0;

                //decrement is now called automatically in the handle destructor

                //=> Total -1
            };
        }

        /**
         * @brief a function to move the asset handle
         */
        void (*m_move)(AssetHandleData*, AssetHandleData*) = nullptr;

        /**
         * @brief a function to copy the asset handle
         */
        void (*m_copy)(AssetHandleData*, const AssetHandleData*) = nullptr;

        /**
         * @brief a function to destroy the asset handle
         */
        void (*m_destroy)(AssetHandleData*) = nullptr;

    };

}

#endif