/**
 * @file CompoundAsset.h
 * @author DM8AT
 * @brief define the compound asset asset type
 * @version 0.1
 * @date 2026-05-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_CORE_COMPOUND_ASSET_
#define _GLGE_CORE_COMPOUND_ASSET_

//include the asset base class
#include "Asset.h"
//add asset managers
#include "AssetManager.h"

//use the library namespace
namespace GLGE {

    //compound assets are defined later
    class CompoundAsset;

    /**
     * @brief define what a compound asset is
     */
    class CompoundAsset : public Asset {
    protected:

        /**
         * @brief store a virtual file reference
         * 
         * This is essential meta data for accessing the file
         */
        struct VirtualFileRef {
            /**
             * @brief the offset from the start of the data blob
             */
            u64 offset;
            /**
             * @brief store the compressed size of the entry
             */
            u64 compressedSize;
            /**
             * @brief store the uncompressed size of the entry
             */
            u64 uncompressedSize;
            /**
             * @brief store the type of stored asset
             */
            u64 fileType;
        };

        /**
         * @brief store the file entry
         */
        struct FileEntry {
            /**
             * @brief store a virtual file reference
             */
            VirtualFileRef reference;
            /**
             * @brief if the asset is loaded, store the handle
             */
            UntypedAssetHandle handle;
        };

    public:

        //iterators are declared later
        class iterator;

        /**
         * @brief store a single file system in a virtual directory
         * 
         * Similar to `std::filesystem::directory_entry`
         */
        class Entry {        
        public:

            /**
             * @brief Construct a new Entry
             */
            Entry() = default;

            /**
             * @brief Destroy the Entry
             */
            ~Entry() = default;

            /**
             * @brief get the path to the element
             * 
             * @return `const std::filesystem::path&` the path to the element
             */
            const std::filesystem::path& path() const noexcept 
            {return m_path;}

            /**
             * @brief check if this is a directory
             * 
             * @return `true` if this is a directory, `false` if not
             */
            bool is_directory() const noexcept 
            {return m_isDir;}

            /**
             * @brief just says if this is NOT a directory
             * 
             * @return `true` if this is NOT a directory, `false` if this is a directory
             */
            bool is_regular_file() const noexcept 
            {return !m_isDir;}

            /**
             * @brief get the size of the element
             * 
             * This is the UNCOMPRESSED size
             * 
             * @return `u64` the size of the element
             */
            u64 file_size() const noexcept
            {return m_size;}
            
            /**
             * @brief get the compressed size of the element
             * 
             * @return `u64` the compressed size of the element
             */
            u64 compressed_size() const noexcept 
            {return m_compSize;}

            /**
             * @brief get the type hash of the element
             * 
             * @return `u64` the type hash of the element's type
             */
            u64 type_hash() const noexcept 
            {return m_typeHash;}

        protected:

            /**
             * @brief Construct a new Entry
             * 
             * @param path the path to the element
             * @param isDir `true` if this is a directory, `false` if not
             * @param uncompSize the uncompressed size of the element
             * @param compSize the compressed size of the element
             * @param typeHash the type hash
             */
            Entry(const std::filesystem::path& path, bool isDir, u64 uncompSize, u64 compSize, u64 typeHash)
            : m_path(path), m_isDir(isDir), m_size(uncompSize), m_compSize(compSize), m_typeHash(typeHash)
            {}

            //iterators are friends
            friend class GLGE::CompoundAsset::iterator;

            /**
             * @brief store the path to the entry
             */
            std::filesystem::path m_path;
            /**
             * @brief `true` if this is a directory, `false` if not
             */
            bool m_isDir = false;
            /**
             * @brief store the uncompressed size
             */
            u64 m_size = 0;
            /**
             * @brief store the compressed size
             */
            u64 m_compSize = 0;
            /**
             * @brief store the type hash
             */
            u64 m_typeHash = 0;

        };

        /**
         * @brief an iterator for the compound assets elements
         */
        class iterator {
        public:

            /**
             * @brief define the category of the iterator
             */
            using iterator_category = std::forward_iterator_tag;
            /**
             * @brief define the value type of the iterator
             */
            using value_type        = Entry;
            /**
             * @brief store the type of a difference of iterators
             */
            using difference_type   = std::ptrdiff_t;
            /**
             * @brief store the pointer type
             */
            using pointer           = const Entry*;
            /**
             * @brief store the reference type
             */
            using reference         = const Entry&;

            /**
             * @brief Construct a new iterator
             */
            iterator() = default;

            /**
             * @brief get a reference to the entry
             * 
             * @return `reference` a constant reference to the entry
             */
            reference operator*() const 
            {return m_entry;}

            /**
             * @brief get a pointer to the entry
             * 
             * @return `pointer` a pointer to the entry
             */
            pointer operator->() const 
            {return &m_entry;}

            /**
             * @brief increment the iterator
             * 
             * @return `iterator&` a reference to the incremented iterator
             */
            iterator& operator++();
            /**
             * @brief increment the iterator
             * 
             * @return `iterator` a COPY of the incremented iterator
             */
            iterator operator++(int);

            /**
             * @brief compare two iterators if they are IDENTICAL
             * 
             * @param other the iterator to compare with
             * @return `true` if they are identical, `false` if not
             */
            bool operator==(const iterator& other) const;

            /**
             * @brief compare two iterators if they are NOT identical
             * 
             * @param other the iterator to compare with
             * @return `true` if they are not identical, `false` if they are identical
             */
            bool operator!=(const iterator& other) const;

        protected:

            /**
             * @brief Construct a new iterator
             * 
             * @param it the iterator to start at
             * @param end the iterator pointing to the end
             */
            iterator(std::unordered_map<std::string, FileEntry>::const_iterator it, std::unordered_map<std::string, FileEntry>::const_iterator end)
             : m_it(it), m_end(end), m_isEnd(it == end)
            {update();}

            //compound asset is friend
            friend class CompoundAsset;

            /**
             * @brief store the current iterator in the unordered map
             */
            std::unordered_map<std::string, FileEntry>::const_iterator m_it;
            /**
             * @brief store the end iterator of the unordered map
             */
            std::unordered_map<std::string, FileEntry>::const_iterator m_end;
            /**
             * @brief store the mapped entry
             */
            Entry m_entry;
            /**
             * @brief store if this is the end
             */
            bool m_isEnd = true;

            /**
             * @brief update the stored entry
             */
            void update();

        };

        /**
         * @brief define an enum for the different used formats
         */
        enum Format : u32 {
            /**
             * @brief define that the GLGE 
             */
            GLGE = 0
        };  

        /**
         * @brief Construct a new Compound Asset
         */
        CompoundAsset() = default;

        /**
         * @brief Destroy the Compound Asset
         */
        virtual ~CompoundAsset() {}

        /**
         * @brief load the asset from raw safe data
         * 
         * @param manager a pointer to the asset manager used for loading
         * @param data the raw binary data to load the asset from
         * @return `u64` the amount of loaded bytes
         */
        virtual u64 load(AssetManager* manager, const std::vector<u8>& data) override;

        /**
         * @brief store the asset in binary
         * 
         * @warning do NOT assume that data is empty at the start. It should be APPENDED and not overridden. 
         * 
         * @param data a vector to append the raw binary data to
         */
        virtual void store(std::vector<u8>& data) override;

        /**
         * @brief import the asset from a stand-alone file
         * 
         * @param manager a pointer to the asset manager used for loading
         * @param file the file to import the asset from
         * @param format the format of the asset
         */
        virtual void import_from(AssetManager* manager, const std::filesystem::path& file, u32 format) noexcept(false) override;

        /**
         * @brief export the asset in a specific file format
         * 
         * @param file a path to the file to store the asset into
         * @param format the file format to use
         */
        virtual void export_as(const std::filesystem::path& file, u32 format) noexcept(false) override;

        /**
         * @brief check if a file entry exists
         * 
         * @warning this function is recursive
         * 
         * @param virtualPath the virtual path to the virtual file
         * @return `true` if the virtual file was found, `false` if not
         */
        bool hasEntry(const std::filesystem::path& virtualPath);

        /**
         * @brief erase an entry from the compound asset
         * 
         * @param virtualPath the path to the file to erase
         * @return `true` if the file was erased successfully, `false` if erasing it failed
         */
        bool erase(const std::filesystem::path& virtualPath);

        /**
         * @brief write an entry to the compound asset
         * 
         * @warning if the entry already exists, it will be overridden. 
         * 
         * @tparam T the type of the asset to write
         * @param virtualPath the virtual path to the file to write
         * @param handle a handle to the asset to store
         */
        template <typename T>
        requires std::is_base_of_v<Asset, T>
        void write(const std::filesystem::path& virtualPath, AssetHandle<T> handle) {
            //get only the top most entry
            //the actual virtual file that is looked for may be in sub-directories

            //sanity check if the front exists
            auto beg = virtualPath.begin();
            if (beg == virtualPath.end()) {throw GLGE::Exception("Failed to open the virtual asset: The virtual path ended unexpectedly", "GLGE::CompoundAsset::open");}

            //check for the top most entry to exist
            bool exists = m_virtualEntryMap.contains(beg->string());

            //step to the next element
            auto ent = beg;
            ++beg;
            //check if the path is at its end
            if (beg == virtualPath.end()) {
                //if it does exist, remove it
                if (exists) {
                    //remove the old entry data
                    auto entr = m_virtualEntryMap.find(ent->string());
                    //it is known that m_virtualEntryMap contains ent->string(), no need to check again
                    VirtualFileRef ref = entr->second.reference;
                    m_virtualEntryMap.erase(entr);
                    //make sure that the alignment is kept
                    for (auto& [_, meta] : m_virtualEntryMap) {
                        if (meta.reference.offset > ref.offset) 
                        {meta.reference.offset -= ref.compressedSize;}
                    }
                    //remove the old data
                    m_rawBlob.erase(m_rawBlob.begin() + ref.offset, m_rawBlob.begin() + ref.offset + ref.compressedSize);
                }

                //write a new entry

                //get the name
                std::string name = ent->string();
                //add the metadata
                VirtualFileRef meta;
                meta.fileType = getTypeHash64<T>();
                meta.offset = m_rawBlob.size();

                //store the asset to the entry
                std::vector<u8> data;
                static_cast<Asset*>(&handle.reference().getAsset())->store(data);
                //compress the data
                std::vector<u8> compressed;
                compress(data, compressed);
                //add the sizes to the metadata
                meta.uncompressedSize = static_cast<u64>(data.size());
                meta.compressedSize = static_cast<u64>(compressed.size());
                //append the data and store the metadata
                m_rawBlob.insert(m_rawBlob.end(), compressed.begin(), compressed.end());
                m_virtualEntryMap[name].reference = meta;
                //store the handle
                m_virtualEntryMap[name].handle.getTyped<T>() = handle;
            } else {
                //if not, recurse deeper
                if (m_virtualEntryMap.at(beg->string()).reference.fileType == getTypeHash64<CompoundAsset>())
                {m_virtualEntryMap.at(beg->string()).handle.getTyped<CompoundAsset>().reference()->open<T>(*beg);}
            }
        }

        /**
         * @brief a function to open a virtual asset like a file
         * 
         * @warning this function may throw if the virtual path is invalid. 
         * 
         * @warning this function is recursive. 
         * 
         * @tparam T the type of the asset to open
         * @param virtualPath the virtual path to open the file at
         * @return `AssetHandle<T>` the filled asset handle
         */
        template <typename T>
        requires std::is_base_of_v<Asset, T>
        AssetHandle<T> open(const std::filesystem::path& virtualPath) {
            //get only the top most entry
            //the actual virtual file that is looked for may be in sub-directories

            //sanity check if the front exists
            auto beg = virtualPath.begin();
            if (beg == virtualPath.end()) {
                //return an invalid handle
                return AssetHandle<T>{};
            }

            //check for the top most entry to exist
            bool exists = m_virtualEntryMap.contains(beg->string());

            //if it does not exist, stop
            if (!exists) {
                //return an invalid handle
                return AssetHandle<T>{};
            }

            //else, check if the asset can be opend
            auto el = beg;
            ++beg;
            if (beg == virtualPath.end()) {
                //get the element
                FileEntry& entry = m_virtualEntryMap.at(el->string());
                //check if it is allready loaded
                if (entry.handle.isValid())
                {return entry.handle.getTyped<T>();}
                //sanity check the type
                if (entry.reference.fileType != getTypeHash64<T>()){
                    //return an invalid handle
                    return AssetHandle<T>{};
                }
                //extract the raw data
                std::vector<u8> data;
                entry.reference.uncompressedSize = getUncompressed(data, entry.reference);
                //actual load to the asset
                AssetHandle<T> handle = m_manager->load<T>(data);
                entry.handle.getTyped<T>() = handle;
                return handle;
            }
            //if not, recurse deeper
            if (m_virtualEntryMap.at(el->string()).reference.fileType == getTypeHash64<CompoundAsset>())
            {return m_virtualEntryMap.at(el->string()).handle.getTyped<CompoundAsset>().reference()->open<T>(*beg);}

            //return an invalid handle
            return AssetHandle<T>{};
        }

        /**
         * @brief get an iterator to the front
         * 
         * @return `iterator` an iterator pointing to the front
         */
        inline iterator begin() const
        {return iterator(m_virtualEntryMap.begin(), m_virtualEntryMap.end());}

        /**
         * @brief get an iterator to the end
         * 
         * @return `iterator` an iterator to the end
         */
        inline iterator end() const
        {return iterator(m_virtualEntryMap.end(), m_virtualEntryMap.end());}

    protected:

        /**
         * @brief Get the uncompressed data of a virtual file
         * 
         * @param data the data to write to
         * @param reference the reference to get the uncompressed data from
         * @return `u64` the actual uncompressed size
         */
        u64 getUncompressed(std::vector<u8>& data, VirtualFileRef reference);

        /**
         * @brief compress a binary blob into another vector
         * 
         * This uses ZIP-compression
         * 
         * @param from the buffer to compress
         * @param to the buffer to compress to
         */
        void compress(const std::vector<u8>& from, std::vector<u8>& to);

        /**
         * @brief store a pointer to the asset manager that this compound asset is loaded to
         */
        AssetManager* m_manager = nullptr;
        
        /**
         * @brief map the names to the entries
         */
        std::unordered_map<std::string, FileEntry> m_virtualEntryMap;
        /**
         * @brief store the raw binary blob loaded from the file
         * 
         * Data is stored compressed by default to not eat up too much ram
         */
        std::vector<u8> m_rawBlob;

    };

}

#endif