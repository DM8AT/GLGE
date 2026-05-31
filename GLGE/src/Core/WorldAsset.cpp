/**
 * @file WorldAsset.cpp
 * @author DM8AT
 * @brief implement the world asset
 * @version 0.1
 * @date 2026-05-31
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//include the world asset
#include "Core/WorldAsset.h"

//add file stuff
#include <fstream>

/*
Format description:
1. Magic Number ("W_AS")
2. Version ID
3. Object count (u64)
4. World name (size-prefixed string)
5. Object list
    5.1. Object Size (size of this object) (u32)
    5.2. Entity ID (u32)
    5.3. Object name component (size-prefixed string)
    5.4. Parent entity ID
    5.5. Amount of serialized components
    5.5. Additional serialized component + type ID + serialized component size combo
*/

//store the current version
#define CURRENT_VERSION 1

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

GLGE::u64 GLGE::WorldAsset::load(AssetManager*, const std::vector<u8>& data) {
    //sanity check the size
    if (data.size() < 20)
    {throw GLGE::Exception("Failed to load the world asset: too small", "GLGE::WorldAsset::load");}
    //check the magic number
    if (data[0] != 'W' || data[1] != '_' || data[2] != 'A' || data[3] != 'S')
    {throw GLGE::Exception("Failed to load the world asset: wrong magic number", "GLGE::WorldAsset::load");}
    //store the current offset in the data
    u64 offs = 4;
    //get the version and check if it is known
    u32 version = readFromBytes<u32>(data, offs);
    if (version > CURRENT_VERSION) 
    {throw GLGE::Exception("Failed to load the world asset: too new version", "GLGE::WorldAsset::load");}
    //get the object count
    u64 objs = readFromBytes<u64>(data, offs);

    //store an entity blob resolution map
    //this is required since the entity IDs may not map in the same way
    std::unordered_map<u32, Tiny::ECS::Entity> entityMap;
    //store all created objects with the parent node
    std::vector<std::pair<Object, u32>> created;
    //prepare the storages
    entityMap.reserve(objs);
    created.reserve(objs);

    //read the name
    u32 nameLen = readFromBytes<u32>(data, offs);
    std::string name;
    name.reserve(nameLen);
    for (size_t i = 0; i < nameLen; ++i)
    {name += readFromBytes<char>(data, offs);}
    //set the name
    m_world.setName(name);

    //iterate over all objects and create them
    for (size_t objId = 0; objId < objs; ++objId) {
        //store the offset at the start
        u64 startOffs = offs;
        //get the size of the object
        u32 objSize = readFromBytes<u32>(data, offs);
        //sanity check
        if (startOffs + sizeof(objSize) + objSize > data.size())
        {throw Exception("Invalid size", "GLGE::WorldAsset::load");}
        //get the old object ID
        u32 id = readFromBytes<u32>(data, offs);
        //get the name
        u32 nameLen = readFromBytes<u32>(data, offs);
        std::string name;
        name.reserve(nameLen);
        for (u32 i = 0; i < nameLen; ++i)
        {name += readFromBytes<char>(data, offs);}
        //get the parent
        u32 parent = readFromBytes<u32>(data, offs);

        //create the actual entity and store it
        Object obj = m_world.create(name);
        created.emplace_back(obj, parent);
        //deserialize all stored components
        u32 compCount = readFromBytes<u32>(data, offs);
        for (size_t i = 0; i < compCount; ++i) {
            //get the type
            u64 typeHash = readFromBytes<u64>(data, offs);
            //get the size
            u32 compSize = readFromBytes<u32>(data, offs);
            //deserialize the component
            auto entry = ms_compReg.m_typeEntries.find(typeHash);
            if (entry == ms_compReg.m_typeEntries.end()) {
                //entry is unknown, skip it
                std::cerr << "[GLGE WARNING] Failed to find the deserialization function for a serialized component of an object, skipping unknown component. Component hash: 0x"
                          << std::hex << typeHash << std::dec << "\n";
                offs += compSize;
                continue;
            }
            //make the span so that out of bounds reads are discovered
            std::span<const u8> subVec(data.begin() + offs, data.begin() + (offs + compSize));
            entry->second.loadFn(obj, subVec, &m_world);
            //step the offset
            offs += compSize;
        }
        //also, add the object mapping
        entityMap.insert_or_assign(id, static_cast<Tiny::ECS::Entity>(obj));

        //regardless of how much was read, set the offset
        offs = startOffs + objSize + sizeof(objSize);
    }

    //then, iterate over all created objects and correctly set the hierarchy
    for (const auto& [obj, parent] : created) {
        //check if root is the parent
        if (parent == Tiny::ECS::Entity::getInvalid().getBlob())
        {continue;}
        //set the parent entity
        m_world.setParent(obj, entityMap.at(parent));
    }

    //store how much was read
    return offs;
}

void GLGE::WorldAsset::store(std::vector<u8>& data) {
    //first step: Discover all objects in the world
    //also store the name and the position in the world structure
    std::unordered_map<u32, ObjSerializationData> objs;
    m_world.each<Component::Name, Component::HierarchyNode>([&objs](const Tiny::ECS::Entity& ent, const Component::Name& name, const Component::HierarchyNode& node) {
        ObjSerializationData& entry = objs[ent.getBlob()];
        entry.name = name;
        entry.node = node;
    });
    //then, discover all registered types
    for (const auto& [typeHash, funcs] : ms_compReg.m_typeEntries) {
        //gather all objects with this component
        std::vector<Object> objList;
        funcs.gatherFn(&m_world, &objList);
        //then, add the serialization function
        for (const Object& obj : objList)
        {objs[static_cast<Tiny::ECS::Entity>(obj).getBlob()].compSerializers.emplace_back(typeHash, funcs.storeFn);}
    }

    //now, all objects are known, the name and nodes for all objects are known and the serialization functions for it are known too. 

    //the serialization can begin
    //store the magic number and the amount of objects
    std::vector<u8> serialized = {'W', '_', 'A', 'S'};
    appendToVector<u32>(serialized, u32(CURRENT_VERSION));
    appendToVector<u64>(serialized, u64(objs.size()));
    //add the name
    const std::string& name = m_world.getName();
    appendToVector<u32>(serialized, u32(name.size()));
    for (const auto& c : name) {appendToVector<char>(serialized, c);}
    //iterate over all objects and serialize them
    for (const auto& [obj, objData] : objs) {
        //store the serialized object
        std::vector<u8> serializedObj;
        //add the entity ID
        appendToVector<u32>(serializedObj, obj);
        //store the name
        const std::string& name = m_world.getObjectName(Object(GLGE::Tiny::ECS::Entity(obj)));
        appendToVector<u32>(serializedObj, u32(name.size()));
        for (const auto& c : name)
        {appendToVector<char>(serializedObj, static_cast<char>(c));}
        //store the parent node
        appendToVector<u32>(serializedObj, u32(objData.node.parent.getBlob()));
        //serialize the other elements
        appendToVector<u32>(serializedObj, u32(objData.compSerializers.size()));
        for (const auto& comp : objData.compSerializers) {
            //add the type
            appendToVector<u64>(serializedObj, comp.first);
            //then serialize
            std::vector<u8> serializedComp;
            (*comp.second)(Tiny::ECS::Entity(obj), serializedComp, &m_world);
            //after serialization, store the size and data
            appendToVector<u32>(serializedObj, serializedComp.size());
            serializedObj.insert(serializedObj.end(), serializedComp.begin(), serializedComp.end());
        }
        //append the object
        appendToVector<u32>(serialized, u32(serializedObj.size()));
        serialized.insert(serialized.end(), serializedObj.begin(), serializedObj.end());
    }

    //store the serialized data
    data.insert(data.end(), serialized.begin(), serialized.end());
}

void GLGE::WorldAsset::import_from(AssetManager* manager, const std::filesystem::path& file, u32 format) noexcept(false) {
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
            throw Exception(stream.str(), "GLGE::WorldAsset::import_from");
        }

        size_t s = static_cast<size_t>(f.tellg());
        f.seekg(std::fstream::beg);
        std::vector<u8> data;
        data.resize(s);
        f.read((char*)data.data(), s);

        //parse the GLGE data
        load(manager, data);
    } else {
        //no other formats known
        throw GLGE::Exception("Unknown format for a compound asset", "GLGE::WorldAsset::import_from");
    }

}

void GLGE::WorldAsset::export_as(const std::filesystem::path& file, u32 format) noexcept(false) {
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
        throw GLGE::Exception("Unknown format for a compound asset", "GLGE::WorldAsset::import_from");
        break;
    }
}