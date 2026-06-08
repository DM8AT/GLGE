/**
 * @file MeshAsset.cpp
 * @author DM8AT
 * @brief implement the core mesh asset
 * @version 0.1
 * @date 2026-06-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the mesh asset
#include "Core/MeshAsset.h"

//add miniz
#include "../external/miniz/miniz.h"

//add assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//include files
#include <fstream>

/*
General Assumptions:
- Assume little endian
- Nodes are 48 bytes in size

File Format:
1. Header [single entry]
    1.1 Magic Number: "MESH"
    1.2 version (u32), split: u16 major (first) + u16 minor (second)
    1.3 lod Count (u32)
    1.4 lod Directory Offset (u64)
    1.5 vertex Attribute Count (u32)
    1.6 vertex attribute table offset (u64)
    1.7 Payload offset (u64)

2. Vertex layout [single entry]
    2.1 Vertex Size (u64) //this is equal to the stride. It is NOT required to be equal to the sum of sizes of the attributes
    2.2 Lost of {
        1. type (u8)
        2. usageHash (u64)
        3. offset (u64)
    } //Order is important

3. LOD directory [list]
    3.1 uncompressed entry size (u64)
    3.2 compressed entry size (u64)
    3.3 Data section start offset (u64) //relative to beginning of file
    3.4 error (f32) //the error value of the LOD

    //all data entries are stored using individually compressed blobs. 
    //compression is done using ZIP-compression of level 9

4. LOD data [list, compressed: list element compressed]
    4.1 Header [single entry]
        4.1.1  vertex data offset (u64) //relative to beginning of section
        4.1.2  vertex data size (u64)
        4.1.3  vertex count (u64)

        4.1.4  index data offset (u64) //relative to beginning of section
        4.1.5  index data size (u64)
        4.1.6  index count (u64)

        4.1.7  BVH node offset (u64) //relative to beginning of section
        4.1.8  BVH node count (u64)
        4.1.9  BVH triangle index index offset (u64) //relative to beginning of section
        4.1.10 BVH triangle index index count (u64)
    
    4.2 Binary data blob [single entry]
      //this contains the vertex, index and BVH data
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

static void compress(const std::vector<GLGE::u8>& from, std::vector<GLGE::u8>& to, unsigned int compressionLevel) {
    //estimate the compressed size
    mz_ulong compSize = mz_compressBound(mz_ulong(from.size()));
    //prepare the target buffer
    to.resize(size_t(compSize));

    //compress the data
    if (mz_compress2(reinterpret_cast<unsigned char*>(to.data()), &compSize, reinterpret_cast<const unsigned char*>(from.data()), from.size(), compressionLevel) != MZ_OK) {
        //retry with new size
        to.resize(size_t(compSize));
        if (mz_compress2(reinterpret_cast<unsigned char*>(to.data()), &compSize, reinterpret_cast<const unsigned char*>(from.data()), from.size(), compressionLevel) != MZ_OK) {
            //compression error
            throw GLGE::Exception("Failed to compress the data", "GLGE::CompoundAsset::compress");
        }
    }

    //resize to fit the compressed size exactly
    to.resize(size_t(compSize));
}

static GLGE::u64 uncompress(GLGE::u64 uncompressedSize, const std::vector<GLGE::u8>& from, std::vector<GLGE::u8>& to) {
    //prepare uncompression
    mz_ulong mzUncompressed = uncompressedSize;
    mz_ulong mzCompressed = from.size();
    //resize the output to make enough space
    to.resize(uncompressedSize);
    //decompress
    int status = mz_uncompress2(reinterpret_cast<unsigned char*>(to.data()), &mzUncompressed, reinterpret_cast<const unsigned char*>(from.data()), &mzCompressed);
    if (status != MZ_OK) {
        std::string err = mz_error(status);
        throw GLGE::Exception(std::string("miniz decompression failure: ") + err, "GLGE::CompoundAsset::getUncompressed");
    }
    //return the uncompressed size
    return GLGE::u64(mzUncompressed);
}

static std::vector<float> buildLODErrors(int levels) {
    //create a vector for the amount of levels
    std::vector<float> errors;
    errors.reserve(levels);

    //store lower and upper bounds
    float e0 = 0.01f;
    float e1 = 0.3f;

    //fill out all levels
    for (int i = 0; i < levels; i++) {
        //compute the step
        float t = float(i) / float(levels - 1);

        //exponential distribution (bias toward low error detail)
        float e = std::pow(t, 2.0f);

        //store the error
        errors.push_back(e0 * (1.f - e) + e1 * e);
    }

    //return the error list
    return errors;
}

GLGE::u64 GLGE::MeshAsset::load(AssetManager* manager, const std::vector<u8>& data) {
    //sanity check the size
    if (data.size() < 4) 
    {throw Exception("Failed to load mesh: File too small", "GLGE::MeshAsset::load");}

    //check the magic number
    if (memcmp(data.data(), "MESH", 4) != 0) 
    {throw Exception("Failed to load mesh: Invalid mesh magic", "GLGE::MeshAsset::load");}

    //store the offset
    u64 offs = 4;

    //read the header
    const u16 major              = readFromBytes<u16>(data, offs);
    const u16 minor              = readFromBytes<u16>(data, offs);

    //check the version (this loader only supports 0.1)
    if (major != 0 || minor != 1) 
    {throw Exception("Failed to load mesh: Invalid version, only version 0.1 is supported.", "GLGE::MeshAsset::load");}

    //read the rest of the header
    const u32 lodCount           = readFromBytes<u32>(data, offs);
    const u64 lodDirectoryOffset = readFromBytes<u64>(data, offs);
    const u32 attributeCount     = readFromBytes<u32>(data, offs);
    const u64 vertexLayoutOffset = readFromBytes<u64>(data, offs);
    const u64 payloadOffset      = readFromBytes<u64>(data, offs);

    //jump to the vertex section
    offs = vertexLayoutOffset;

    //read the size of a single vertex
    u64 vertSize = readFromBytes<u64>(data, offs);

    //read the attributes
    std::vector<Mesh::VertexAttribute> attributes(attributeCount);
    for (u64 i = 0; i < attributeCount; ++i) {
        attributes[i].type   = static_cast<Mesh::VertexAttribute::Type>(readFromBytes<u8>(data, offs));
        attributes[i].usage  = readFromBytes<u64>(data, offs);
        attributes[i].offset = readFromBytes<u64>(data, offs);
    }
    //write the layout
    Mesh::VertexLayout layout(attributes, vertSize);

    //jump to the LOD section
    offs = lodDirectoryOffset;

    //load all the LODs
    struct LODInfos {
        u64 uncompSize = 0;
        u64 compSize = 0;
        u64 offset = 0;
        float error = 0.f;
    };
    std::vector<LODInfos> LODs(lodCount);
    for (size_t i = 0; i < lodCount; ++i) {
        LODs[i].uncompSize = readFromBytes<u64>(data, offs);
        LODs[i].compSize   = readFromBytes<u64>(data, offs);
        LODs[i].offset     = readFromBytes<u64>(data, offs);
        LODs[i].error      = readFromBytes<f32>(data, offs);
    }

    //create the mesh
    m_mesh = std::make_shared<Mesh>(lodCount, layout);

    //store the read end
    size_t readEnd = 0;
    //iterate over all LODs and load them
    for (size_t i = 0; i < lodCount; ++i) {
        //compute the end that was read to
        size_t end = LODs[i].offset + LODs[i].compSize;
        //bounds check
        if (end > data.size())
        {throw GLGE::Exception("Failed to create mesh: tried to read out of bounds", "GLGE::MeshAsset::load");}
        //get the data
        std::vector<u8> dat(data.begin() + LODs[i].offset, data.begin() + LODs[i].offset + LODs[i].compSize);

        //if this is the largest end, store it
        if (end > readEnd) {readEnd = end;}

        //uncompress the data
        std::vector<u8> uncompressed;
        uncompress(LODs[i].uncompSize, dat, uncompressed);

        //store the section-specific offset
        u64 sectionOffs = 0;

        //do a range check
        auto validateRange = [&](u64 offset, u64 size) {if (offset + size > uncompressed.size()){throw Exception("Failed to load out of range: Range check failed", "GLGE::MeshAsset::load");}};

        //load the LOD header
        const u64 vertexDataOffset = readFromBytes<u64>(uncompressed, sectionOffs);
        const u64 vertexDataSize   = readFromBytes<u64>(uncompressed, sectionOffs);
        const u64 vertexCount      = readFromBytes<u64>(uncompressed, sectionOffs);
        const u64 indexDataOffset  = readFromBytes<u64>(uncompressed, sectionOffs);
        const u64 indexDataSize    = readFromBytes<u64>(uncompressed, sectionOffs);
        const u64 indexCount       = readFromBytes<u64>(uncompressed, sectionOffs);
        const u64 nodeOffset       = readFromBytes<u64>(uncompressed, sectionOffs);
        const u64 nodeCount        = readFromBytes<u64>(uncompressed, sectionOffs);
        const u64 triIndexOffset   = readFromBytes<u64>(uncompressed, sectionOffs);
        const u64 triIndexCount    = readFromBytes<u64>(uncompressed, sectionOffs);

        //validation
        if (vertexDataSize != vertexCount * vertSize) 
        {throw Exception( "Invalid vertex section size", "GLGE::MeshAsset::load");}

        if (indexCount != 0) {
            if (indexDataSize % indexCount != 0) 
            {throw Exception("Invalid index section", "GLGE::MeshAsset::load");}

            //get the index stride
            const u64 indexStride = indexDataSize / indexCount;

            if (major == 0 && minor == 1 && indexStride != sizeof(u32)) 
            {throw Exception("Mesh v0.1 requires u32 indices", "GLGE::MeshAsset::load");}
        }

        //get the index data
        std::vector<u32> indices(indexCount);
        validateRange(indexDataOffset, indexDataSize);
        memcpy(indices.data(), uncompressed.data() + indexDataOffset, indexDataSize);
        //map to triangles
        if (indices.size() % 3 != 0)
        {throw GLGE::Exception("Failed to read mesh: Found an index count that was not mappable to triangles", "GLGE::MeshAsset::load");}
        std::vector<Triangle> triangles(indices.size()/3);
        for (size_t j = 0; j < triangles.size(); ++j)
        {triangles[j] = Triangle{
            .a = indices[j*3+0],
            .b = indices[j*3+1],
            .c = indices[j*3+2]
        };}
        
        //add the LOD to the mesh
        validateRange(vertexDataOffset, vertexDataSize);
        m_mesh->addLOD(uncompressed.data() + vertexDataOffset, vertexCount, triangles, LODs[i].error, false);

        //read the BVH nodes
        std::vector<Mesh::BVH::Node> nodes(nodeCount);
        validateRange(nodeOffset, sizeof(Mesh::BVH::Node)*nodeCount);
        memcpy(nodes.data(), uncompressed.data() + nodeOffset, sizeof(Mesh::BVH::Node)*nodeCount);
        //read the BVH triangle index indices
        std::vector<u64> triangleIdxIdx(triIndexCount);
        validateRange(triIndexOffset, sizeof(u64)*triIndexCount);
        memcpy(triangleIdxIdx.data(), uncompressed.data() + triIndexOffset, sizeof(u64)*triIndexCount);

        //create the BVH
        Mesh::BVH bvh(&m_mesh->getLOD(i), nodes, triangleIdxIdx);
        //store the BVH
        m_mesh->getLOD(i).setBVH(std::move(bvh));
    }

    //return the read end
    return readEnd;
}

void GLGE::MeshAsset::store(std::vector<u8>& data) {
    //if this is an empty asset, stop
    if (!m_mesh)
    {throw GLGE::Exception("Cannot store a mesh asset that holds no mesh", "GLGE::MeshAsset::store");}

    //store the data generated (for exception safety)
    //directly store magic number
    std::vector<u8> genData = {'M', 'E', 'S', 'H'};
    //store the version (current: 0.1)
    appendToVector<u16>(genData, 0);
    appendToVector<u16>(genData, 1);
    //store the LOD count
    appendToVector<u32>(genData, static_cast<u32>(m_mesh->getLODCount()));
    //compute the offsets
    constexpr u64 VertOffs = 4 + sizeof(u32) + sizeof(u32) + sizeof(u64) + sizeof(u32) + 2*sizeof(u64);
    const u64 LODOffs = VertOffs + sizeof(u64) + (sizeof(u8) + sizeof(u64) + sizeof(u64))*m_mesh->getLayout().getAttributeCount();
    const u64 DataOffs = LODOffs + (3*sizeof(u64))*m_mesh->getLODCount();
    //store the lod offset
    appendToVector<u64>(genData, LODOffs);
    //store the attribute data
    appendToVector<u32>(genData, m_mesh->getLayout().getAttributeCount());
    appendToVector<u64>(genData, VertOffs);
    //store the payload offset
    appendToVector<u64>(genData, DataOffs);

    //header done

    //write vertex layout
    //write the attribute count
    appendToVector<u64>(genData, m_mesh->getLayout().getSize());
    //write all the attributes
    for (size_t i = 0; i < m_mesh->getLayout().getAttributeCount(); ++i) {
        const auto& attr = m_mesh->getLayout().getAttribute(i);
        appendToVector<u8>(genData, static_cast<u8>(attr.type));
        appendToVector<u64>(genData, attr.usage);
        appendToVector<u64>(genData, attr.offset);
    }

    //compute all LOD data tables
    std::vector<std::vector<u8>> uncompLODDat;
    uncompLODDat.resize(m_mesh->getLODCount());
    for (size_t i = 0; i < uncompLODDat.size(); ++i) {
        //get the data vector
        auto& dat = uncompLODDat[i];
        //get the LOD
        const auto& lod = m_mesh->getLOD(i);

        //compute the size of the header
        constexpr size_t HeaderSize = sizeof(u64)*10;

        //write the header
        appendToVector<u64>(dat, HeaderSize);
        u64 vertSectionSize = lod.vertices().getCount()*m_mesh->getLayout().getSize();
        appendToVector<u64>(dat, vertSectionSize);
        appendToVector<u64>(dat, lod.vertices().getCount());

        appendToVector<u64>(dat, HeaderSize + vertSectionSize);
        u64 indSectionSize = lod.indices().getCount() * sizeof(u32);
        appendToVector<u64>(dat, indSectionSize);
        appendToVector<u64>(dat, lod.indices().getCount());

        appendToVector<u64>(dat, HeaderSize + vertSectionSize + indSectionSize);
        appendToVector<u64>(dat, lod.getBVH().getNodeCount());

        appendToVector<u64>(dat, HeaderSize + vertSectionSize + indSectionSize + sizeof(GLGE::Mesh::BVH::Node)*lod.getBVH().getNodeCount());
        appendToVector<u64>(dat, lod.getBVH().getTriangleIndexIndexCount());

        //write the actual vertex data
        std::vector<u8> vertData(vertSectionSize);
        memcpy(vertData.data(), lod.vertices().data(), vertData.size());
        dat.insert(dat.end(), vertData.begin(), vertData.end());

        //write the actual index data
        u64 offs = dat.size();
        dat.resize(lod.indices().getCount() * sizeof(u32) + lod.getBVH().getNodeCount()*sizeof(Mesh::BVH::Node) + lod.getBVH().getTriangleIndexIndexCount()*sizeof(u64));
        memcpy(dat.data() + offs, lod.indices().data(), lod.indices().getCount() * sizeof(u32));
        offs += lod.indices().getCount() * sizeof(u32);

        //write all the nodes
        memcpy(dat.data() + offs, lod.getBVH().getNodes().data(), lod.getBVH().getNodeCount()*sizeof(Mesh::BVH::Node));
        offs += lod.getBVH().getNodeCount()*sizeof(Mesh::BVH::Node);

        //write all the triangle index indices
        memcpy(dat.data() + offs, lod.getBVH().getTriangleIndexIndices().data(), lod.getBVH().getTriangleIndexIndexCount()*sizeof(u64));
    }

    //generate all the compressed data
    std::vector<std::vector<u8>> compressed;
    compressed.resize(uncompLODDat.size());
    for (size_t i = 0; i < uncompLODDat.size(); ++i) {
        //compress the data
        compress(uncompLODDat[i], compressed[i], 9);
    }

    //store the LOD table
    size_t entryOffs = DataOffs;
    for (size_t i = 0; i < m_mesh->getLODCount(); ++i) {
        appendToVector<u64>(genData, uncompLODDat[i].size());
        appendToVector<u64>(genData, compressed[i].size());
        appendToVector<u64>(genData, entryOffs);
        appendToVector<f32>(genData, m_mesh->getLOD(i).getError());
        entryOffs += compressed[i].size();
    }

    //store all the entries
    for (const auto& entry : compressed)
    {genData.insert(genData.end(), entry.begin(), entry.end());}

    //copy the data over
    data.insert(data.end(), genData.begin(), genData.end());
}

void GLGE::MeshAsset::import_from(AssetManager* manager, const std::filesystem::path& file, u32 format) noexcept(false) {
    //check if the file exists
    if (!std::filesystem::is_regular_file(file)) {
        std::stringstream stream;
        stream << "Failed to import mesh file " << file << " because the file was not found";
        throw Exception(stream.str(), "GLGE::Graphic::Asset::Mesh::import_from");
    }

    //check the format
    if (format == Format::GLGE) {
        //load the file
        std::ifstream f(file, std::ifstream::ate | std::ifstream::binary);
        size_t size = f.tellg();
        f.seekg(std::ifstream::beg);
        std::vector<uint8_t> data(size*sizeof(char));
        f.read(reinterpret_cast<char*>(data.data()), size);
        f.close();
        //load using the loading function
        load(nullptr, data);
    } else if (format == Format::ASSIMP) {
        //load the file using assimp
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(file.string(), aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_ImproveCacheLocality);
        //sanity check that there is a mesh to load
        if (!scene) {throw Exception(importer.GetErrorString(), "GLGE::Graphic::Asset::Mesh::import_from");}
        if (!scene->HasMeshes()) {throw Exception("Succeeded to load the scene, but no mesh was found", "GLGE::Graphic::Asset::Mesh::import_from");}

        //may only load a single mesh
        if (scene->mNumMeshes > 1) {throw Exception("When importing using assimp, there may only be a single mesh when loading a mesh asset", "GLGE::Graphic::Asset::Mesh::import_from");}

        //now it is validated that there is exactly one mesh to load. Load this mesh. 

        //get the mesh
        const aiMesh* mesh = scene->mMeshes[0];

        //create the vertex layout
        std::vector<Mesh::VertexAttribute> attributes;
        size_t offset = 0;
        if (mesh->HasPositions()) {
            attributes.emplace_back(Mesh::Type::vec3, getTypeHash64<VertexAttribute::Position>(), 0);
            offset += Mesh::VertexAttribute::getTypeInfo(Mesh::Type::vec3).size;
        } else 
        {throw GLGE::Exception("Tried to import a mesh that has no positions", "GLGE::MeshAsset::import_from");}
        if (mesh->HasNormals()) {
            attributes.emplace_back(Mesh::Type::vec3, getTypeHash64<VertexAttribute::Normal>(), 0);
            offset += Mesh::VertexAttribute::getTypeInfo(Mesh::Type::vec3).size;
        }
        if (mesh->HasTangentsAndBitangents()) {
            attributes.emplace_back(Mesh::Type::vec3, getTypeHash64<VertexAttribute::Tangent>(), 0);
            offset += Mesh::VertexAttribute::getTypeInfo(Mesh::Type::vec3).size;
            attributes.emplace_back(Mesh::Type::vec3, getTypeHash64<VertexAttribute::Bitangent>(), 0);
            offset += Mesh::VertexAttribute::getTypeInfo(Mesh::Type::vec3).size;
        }
        Mesh::VertexLayout layout(attributes, 0);

        //store the index and vertex buffer
        Mesh::LOD::Vertices verts(nullptr, mesh->mNumVertices, layout);
        std::vector<Triangle> indices(mesh->mNumFaces); //faces are always triangles due to triangulation

        //store all positions for later AABB construction
        std::vector<vec3> pos;
        pos.reserve(mesh->mNumVertices);

        //load all vertices
        for (size_t i = 0; i < mesh->mNumVertices; ++i) {
            //get an vertex iterator to the current vertex
            Mesh::Vertex curr = verts.get(i);

            //position is guaranteed to exist
            *curr->get<vec3, VertexAttribute::Position>() = vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            pos.push_back(vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));

            //check if normals exist
            if (mesh->HasNormals()) {
                //load the normals
                *curr->get<vec3, VertexAttribute::Normal>() = vec3(mesh->mNormals[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            }
            //check if tangents exist
            if (mesh->HasTangentsAndBitangents()) {
                //load the tangents
                *curr->get<vec3, VertexAttribute::Tangent>() = vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
                //load the bitangents
                *curr->get<vec3, VertexAttribute::Bitangent>() = vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
            }
        }

        //load the faces (each face is a triangle)
        for (size_t i = 0; i < mesh->mNumFaces; ++i) {
            //sanity check again in debug
            #if GLGE_DEBUG
            if (mesh->mFaces[i].mNumIndices != 3)
            {throw Exception("Found a face that is not a triangle even though triangulation is used when using assimp", "GLGE::Graphic::Asset::Mesh::import_from");}
            #endif
            //load the indices
            indices[i].a = mesh->mFaces[i].mIndices[0];
            indices[i].b = mesh->mFaces[i].mIndices[1];
            indices[i].c = mesh->mFaces[i].mIndices[2];
        }

        //create the bounding AABB
        AABB aabb(pos);

        //estimate the maximum acceptable error
        float maxErr = glm::length(aabb.getExtent()) * 0.1f;
        //store an acceptable error minimum
        const float minErr = 0.01f;
        //compute the amount of LOD steps
        //this is only a conservative estimation
        size_t steps = glm::ceil(glm::log(maxErr / minErr) / glm::log(2.f));

        //clamp the steps to some minimal range
        //this results in 2 to 8 LODs
        steps = glm::clamp<size_t>(steps, 1, 7);

        //create the actual mesh
        m_mesh = std::make_shared<Mesh>(steps, layout);

        //add the base LOD
        m_mesh->addLOD(verts.data(), verts.getCount(), indices, 0.f, true);

        //iteratively simplify and store until either the mesh LOD count is saturated or the simplification has no benefits
        //make sure to start at LOD 1 so that the previous LOD always exists
        float lastErr = 0.f;
        float newTargetErr = 0.01f;
        size_t i = 1;
        size_t staggeringCount = 0;
        while (true) {
            //simplify the mesh
            Mesh::LOD lod(&m_mesh->getLOD(0), newTargetErr);
            float err = lod.getError();
            //check if there were improvements
            //stop if there were no improvements over multiple tries
            const auto& last = m_mesh->getLOD(i-1);
            bool staggering = lod.getIndexCount() >= last.getIndexCount()*0.98f;
            if (staggering) {
                ++staggeringCount;
                if (staggeringCount > 2)
                {break;}
            } else 
            {staggeringCount = 0;}
            //reject a true duplicate (error the same, index count the same, vertex count the same)
            if (!staggering || !(lod.getVertexCount() == last.getVertexCount() && lod.getIndexCount() == last.getIndexCount() && lod.getError() == last.getError())) {
                //there were improvements, so store the new LOD
                m_mesh->addLOD(std::move(lod));
                //step
                ++i;
            }
            //store the last error
            lastErr = err;
            //compute the next target error
            newTargetErr *= staggering ? (4.f) : 2.f;
        }
    }
}

void GLGE::MeshAsset::export_as(const std::filesystem::path& file, u32 format) noexcept(false) {
    //check the format
    if (format == Format::GLGE) {
        //open the file
        std::ofstream f(file, std::ofstream::binary);
        //store the data
        std::vector<u8> output;
        store(output);
        //save the data to the file
        f.write(reinterpret_cast<char*>(output.data()), output.size()/sizeof(char));
    }
}