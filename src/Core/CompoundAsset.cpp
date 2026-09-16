/**
 * @file CompoundAsset.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-05-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add compound assets
#include "Core/CompoundAsset.h"
//add asset managers
#include "Core/AssetManager.h"

//for file reading / writing
#include <fstream>

//add miniz
#include "../external/miniz/miniz.h"

/**
 * @brief define the used zip compression level
 */
inline static constexpr int ZIP_COMPRESSION_LEVEL = 9;

//Layout definition
/*
0. Magic Bytes "COMP" (UTF-8 encoded) -> 4 Bytes
1. Entry Count (u64) -> 8 bytes
2. Entry Table (all entries are tightly packed)
    2.0 Name -> 4 + n bytes
        2.0.0 Character count (u32) -> 4 Bytes
        2.0.1 String characters, non-null terminated -> 1 byte per entry (UTF-8 encoding)
    2.1 Type Hash (u64) -> 8 bytes
    2.2 Payload Offset (in bytes from start of data section, allows for alignment or padding) (u64) -> 8 bytes
    2.3 Payload size (compressed size) (u64) -> 8 bytes
    2.4 Payload size (uncompressed size) (u64) -> 8 bytes
     INFO: ZIP-Compression with a strength of 9 should be used. No encryption is used. 
3. Data section (raw data blob)
*/

/**
 * @brief a helper function to read a value from binary data
 * 
 * @tparam T the type to read
 * @param src the source buffer to read from
 * @param offset a REFERENCE to the offset. It is used to read from and then incremented by `sizeof(T)`
 * @return `T` the read value
 */
template<typename T>
static T readFromBytes(const std::vector<GLGE::u8>& src, GLGE::u64& offset) {
    //check the size
    if (offset + sizeof(T) > src.size()) 
    {throw GLGE::Exception("Out of bounds archive layout read attempt", "CompoundAsset Binary Reader");}

    //read the value into an aligned buffer (source buffer may be unaligned)
    T val;
    memcpy(&val, &src[offset], sizeof(T));

    //step forward
    offset += sizeof(T);
    //return the value
    return val;
}

/**
 * @brief a helper function to append data to a vector
 * 
 * @warning the data alignment in the destination buffer is not guaranteed to be equal or greater than `alignof(T)`. 
 * 
 * @tparam T the type of the data to append
 * @param dest the vector to append to
 * @param value the value to append
 */
template<typename T>
static void appendToVector(std::vector<GLGE::u8>& dest, const T& value) {
    //get a pointer to the data beginning
    const GLGE::u8* bytes = reinterpret_cast<const GLGE::u8*>(&value);
    //append to the end
    dest.insert(dest.end(), bytes, bytes + sizeof(T));
}


GLGE::CompoundAsset::iterator& GLGE::CompoundAsset::iterator::operator++() {
    //if this is not the end, step and update
    if (!m_isEnd && m_it != m_end) {
        ++m_it;
        update();
    }
    //return a reference to this
    return *this;
}

GLGE::CompoundAsset::iterator GLGE::CompoundAsset::iterator::operator++(int) {
    //copy the iterator
    iterator tmp = *this;
    //step this
    ++(*this);
    //return the copy
    return tmp;
}

bool GLGE::CompoundAsset::iterator::operator==(const iterator& other) const {
    //if both are at the end, they are equal
    if (m_isEnd && other.m_isEnd) 
    {return true;}
    //if either is at the end, but the other not, they are not equal
    if (m_isEnd != other.m_isEnd) 
    {return false;}
    //else, compare the iterators
    return m_it == other.m_it;
}

bool GLGE::CompoundAsset::iterator::operator!=(const iterator& other) const {
    //check if they are equal, and just invert it
    return !(*this == other);
}

void GLGE::CompoundAsset::iterator::update() {
    //do not move past the end
    if (m_isEnd || m_it == m_end) {
        m_isEnd = true;
        return;
    }
    
    //check if the current entry is another CompoundAsset (a nested directory)
    bool isDir = (m_it->second.reference.fileType == getTypeHash64<CompoundAsset>());
    
    //construct the proxy entry
    m_entry = GLGE::CompoundAsset::Entry(
        m_it->first, 
        isDir, 
        m_it->second.reference.uncompressedSize,
        m_it->second.reference.compressedSize,
        m_it->second.reference.fileType
    );
}



GLGE::u64 GLGE::CompoundAsset::load(AssetManager* manager, const std::vector<u8>& data) {
    //store the inputted manager and prepare the data
    m_manager = manager;
    m_virtualEntryMap.clear();
    m_rawBlob.clear();

    //sanity-check the size early
    if (data.size() < 12) 
    {throw GLGE::Exception("Invalid compound asset: File too small", "GLGE::CompoundAsset::load");}

    //store the current read offset
    u64 offs = 0;

    //verify magic number
    if ((data[0] != 'C') || (data[1] != 'O') || (data[2] != 'M') || (data[3] != 'P'))
    {throw GLGE::Exception("Invalid compound asset: Invalid magic number", "GLGE::CompoundAsset::load");}
    offs += 4;

    //read the amount of entires
    u64 entries = readFromBytes<u64>(data, offs);

    //reserve space for the entries, then load them
    m_virtualEntryMap.reserve(entries);
    for (u64 i = 0; i < entries; ++i) {
        //read the name
        u32 nameLen = readFromBytes<u32>(data, offs);
        std::string name(nameLen, ' ');
        for (u32 j = 0; j < nameLen; ++j) {
            name[j] = static_cast<char>(readFromBytes<i8>(data, offs));
        }
        //get the type hash
        u64 tHash = readFromBytes<u64>(data, offs);
        //get the offsets
        u64 payloadOffs = readFromBytes<u64>(data, offs);
        //get the sizes
        u64 compSize = readFromBytes<u64>(data, offs);
        u64 ucompSize = readFromBytes<u64>(data, offs);

        //store the entry
        m_virtualEntryMap[name] = FileEntry {
            .reference = VirtualFileRef {
                .offset = payloadOffs,
                .compressedSize = compSize,
                .uncompressedSize = ucompSize,
                .fileType = tHash
            },
            .handle = {}
        };
    }
    //store the data section start offset
    u64 dataSecStart = offs;

    //iterate over all blocks and read them, then re-write the offsets to compress the loaded buffer size
    //keep everything compressed, keeping unneeded gigantic assets loaded in RAM at all time is a really bad idea
    //games can be large and poor distribution across packages may consume gigabytes of RAM if decompressed
    for (auto& [name, entry] : m_virtualEntryMap) {
        //store the old offset
        u64 oldOffs = entry.reference.offset + dataSecStart;
        //write the offset
        entry.reference.offset = m_rawBlob.size();
        //load the data
        m_rawBlob.insert(m_rawBlob.end(), data.begin() + oldOffs, data.begin() + oldOffs + entry.reference.compressedSize);
        if (oldOffs + entry.reference.compressedSize > offs)
        {offs = oldOffs + entry.reference.compressedSize;}
    }

    //load all detected compound sub-assets
    for (auto& [name, entry] : m_virtualEntryMap) {
        //check if the asset type is a compound asset
        if (entry.reference.fileType != getTypeHash64<CompoundAsset>()) {continue;}

        //load the sub-asset
        std::vector<u8> dat;
        getUncompressed(dat, entry.reference);
        entry.handle = m_manager->load<CompoundAsset>(dat);

        //this recursive loading ensures that sub-asset paths are fully accessible
    }

    //return how much was read (this MAY not be the full data)
    return offs;
}

void GLGE::CompoundAsset::store(std::vector<u8>& data) {
    //do NOT clear. There may be valid data allready in the vector. This is intentional. 

    //add the magic bytes
    data.push_back('C'); data.push_back('O'); data.push_back('M'); data.push_back('P');

    //append entry count
    appendToVector(data, u64(m_virtualEntryMap.size()));

    //add the entry metadata table
    for (const auto& [name, entry] : m_virtualEntryMap) {
        //store the length-prefixed name
        appendToVector(data, u32(name.size()));
        for (u32 i = 0; i < name.size(); ++i)
        {appendToVector(data, char(name[i]));}

        //store the type hash
        appendToVector(data, u64(entry.reference.fileType));
        //store the offset in the data section
        appendToVector(data, u64(entry.reference.offset));
        //store the sizes
        appendToVector(data, u64(entry.reference.compressedSize));
        appendToVector(data, u64(entry.reference.uncompressedSize));
    }

    //append the data buffer
    data.insert(data.end(), m_rawBlob.begin(), m_rawBlob.end());
}

void GLGE::CompoundAsset::import_from(AssetManager* manager, const std::filesystem::path& file, u32 format) noexcept(false) {
    //empty path ("") means to just stop
    if (file == "") {return;}

    //some data is needed to be handled specially
    if (format == Format::GLGE) {
        //open the file in a binary data stream, create a vector from it and just use the GLGE load function
        std::ifstream f(file, std::fstream::binary | std::fstream::ate);

        //sanity check
        if (!f.is_open()) {
            std::stringstream stream;
            stream << "Failed to open file " << file << " during importing compound asset";
            throw Exception(stream.str(), "GLGE::CompoundAsset::import_from");
        }

        size_t s = f.tellg();
        f.seekg(std::fstream::beg);
        std::vector<u8> data;
        data.resize(s);
        f.read((char*)data.data(), s);

        //parse the GLGE data
        load(manager, data);
    } else {
        //no other formats known
        throw GLGE::Exception("Unknown format for a compound asset", "GLGE::CompoundAsset::import_from");
    }
}

void GLGE::CompoundAsset::export_as(const std::filesystem::path& file, u32 format) noexcept(false) {
    //switch over the format to select the correct saving function
    switch (format)
    {
    //export as GLGE-custom format
    case Format::GLGE: {
            //compute binary data
            std::vector<u8> blob;
            store(blob);
            //store the binary blob in a file
            std::ofstream f(file, std::fstream::binary);
            f.clear();
            f.write((char*)blob.data(), blob.size());
        }
        break;

    default:
        //unknown format
        throw GLGE::Exception("Unknown format for a compound asset", "GLGE::CompoundAsset::import_from");
        break;
    }
}

bool GLGE::CompoundAsset::hasEntry(const std::filesystem::path& virtualPath) {
    //get only the top most entry
    //the actual virtual file that is looked for may be in sub-directories

    //sanity check if the front exists
    auto beg = virtualPath.begin();
    if (beg == virtualPath.end()) {return false;}

    //check for the top most entry to exist
    bool exists = m_virtualEntryMap.contains(beg->string());

    //if it does not exist, stop
    if (!exists) {return false;}

    //else, potentially recurse deeper
    beg++;
    if (beg == virtualPath.end()) {return exists;}
    if (m_virtualEntryMap.at(beg->string()).reference.fileType == getTypeHash64<CompoundAsset>())
    {return m_virtualEntryMap.at(beg->string()).handle.getTyped<CompoundAsset>().reference()->hasEntry(*beg);}
    //at this point -> fail
    return false;
}

bool GLGE::CompoundAsset::erase(const std::filesystem::path& virtualPath) {
    //todo
    return false;
}

GLGE::u64 GLGE::CompoundAsset::getUncompressed(std::vector<u8>& data, VirtualFileRef reference) {
    //prepare uncompression
    mz_ulong mzUncompressed = reference.uncompressedSize;
    mz_ulong mzCompressed = reference.compressedSize;
    //resize the data to make enough space
    data.resize(reference.uncompressedSize);
    //decompress
    int status = mz_uncompress2(reinterpret_cast<unsigned char*>(data.data()), &mzUncompressed, reinterpret_cast<unsigned char*>(&m_rawBlob[reference.offset]), &mzCompressed);
    if (status != MZ_OK) {
        std::string err = mz_error(status);
        throw GLGE::Exception(std::string("miniz decompression failure: ") + err, "GLGE::CompoundAsset::getUncompressed");
    }
    //return the uncompressed size
    return u64(mzUncompressed);
}

void GLGE::CompoundAsset::compress(const std::vector<u8>& from, std::vector<u8>& to) {
    //estimate the compressed size
    mz_ulong compSize = mz_compressBound(mz_ulong(from.size()));
    //prepare the target buffer
    to.resize(size_t(compSize));

    //compress the data
    if (mz_compress2(reinterpret_cast<unsigned char*>(to.data()), &compSize, reinterpret_cast<const unsigned char*>(from.data()), from.size(), ZIP_COMPRESSION_LEVEL) != MZ_OK) {
        //retry with new size
        to.resize(size_t(compSize));
        if (mz_compress2(reinterpret_cast<unsigned char*>(to.data()), &compSize, reinterpret_cast<const unsigned char*>(from.data()), from.size(), ZIP_COMPRESSION_LEVEL) != MZ_OK) {
            //compression error
            throw GLGE::Exception("Failed to compress the data", "GLGE::CompoundAsset::compress");
        }
    }

    //resize to fit the compressed size exactly
    to.resize(size_t(compSize));
}