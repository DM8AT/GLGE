/**
 * @file MeshAsset.cpp
 * @author DM8AT
 * @brief implement the class to load mesh assets
 * @version 0.1
 * @date 2026-03-30
 * 
 * @copyright Copyright (c) 2026
 * 
 * The mesh asset format looks as follows:
 * 1. Header
 *      1.1: Magic Number: "MESH"
 *      1.2: Version (3x u8)
 *  => 7 bytes
 * 2. Layout description table
 *      2.1 Size of the vertex in bytes (u64)
 *      2.2 Index size in bytes (u8)
 *  => 9 bytes
 * 3. LOD Offset table
 *      3.1: LOD Count (u8)
 *      3.2: LOD Offset + size list
 *           3.2.1: LOD byte offset from data section start (u64)
 *           3.2.2: Mesh data section size in bytes (u64)
 *  => 1 byte + lodCount * 16 bytes
 * 4. LOD data table
 *      4.1 Start of LOD data storage section
 *          4.1.1 Uncompressed total size (u64)
 *          ZIP-Compression start
 *          4.1.2 Size of vertex blob in bytes (u64)
 *          4.1.3 Vertex data blob (undefined)
 *          4.1.4 Size of index blob in bytes (u64)
 *          4.1.5 Index data blob (undefined)
 *          ZIP-Compression end
 *      4.2 End of LOD data section
 *  => Variable size (read size from LOD offset table)
 */
//add the mesh asset
#include "Graphic/Assets/MeshAsset.h"
//add assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
//include files
#include <fstream>
//add mini-z for zip compression
#include "miniz/miniz.h"

//store the current version constants
static const constexpr GLGE::u8 VERSION_MAJOR = 1;
static const constexpr GLGE::u8 VERSION_MINOR = 0;
static const constexpr GLGE::u8 VERSION_PATCH = 0;

/**
 * @brief define the structure for the imported vertices
 * 
 * This structure is defined by the import type (assimp) as it has a fixed format
 */
struct Vertex {
    GLGE::vec3 pos {0,0,0};
    GLGE::vec2 uv_0 {0,0};
    GLGE::vec3 normal {0,0,0};
    GLGE::vec3 tangent {0,0,0};
    GLGE::vec4 color_0 {0,0,0,0};
};

//define the default vertex format
static const GLGE::Graphic::VertexAttribute defaultAttributes[] = {
    GLGE::Graphic::VertexAttribute(GLGE::Graphic::VertexAttribute::Type::Position, GLGE::Graphic::VertexAttribute::Format::vec3, offsetof(Vertex, pos),     0),
    GLGE::Graphic::VertexAttribute(GLGE::Graphic::VertexAttribute::Type::UV_0,     GLGE::Graphic::VertexAttribute::Format::vec2, offsetof(Vertex, uv_0),    1),
    GLGE::Graphic::VertexAttribute(GLGE::Graphic::VertexAttribute::Type::Normal,   GLGE::Graphic::VertexAttribute::Format::vec3, offsetof(Vertex, normal),  2),
    GLGE::Graphic::VertexAttribute(GLGE::Graphic::VertexAttribute::Type::Tangent,  GLGE::Graphic::VertexAttribute::Format::vec3, offsetof(Vertex, tangent), 3),
    GLGE::Graphic::VertexAttribute(GLGE::Graphic::VertexAttribute::Type::Color_0,  GLGE::Graphic::VertexAttribute::Format::vec4, offsetof(Vertex, color_0), 4),
};

GLGE::u64 GLGE::Graphic::Asset::Mesh::load(const std::vector<u8>& data) {
    //confirm the magic number (first for bytes are "MESH")
    if (data.size() < 4) {throw Exception("Failed to load GLGE mesh file - Invalid format", "GLGE::Graphic::Asset::Mesh::load");}
    if (!(data[0] == 'M' && data[1] == 'E' && data[2] == 'S' && data[3] == 'H')) {throw Exception("Failed to load GLGE mesh file - Invalid format", "GLGE::Graphic::Asset::Mesh::load");}
    //format validity confirmed (hopefully)

    //header must exist
    if (data.size() < 18)
    {throw Exception("Failed to read required size and lod count information - Invalid / corrupted data", "GLGE::Graphic::Asset::Mesh::load");}

    //read the version
    struct Version {u8 major; u8 minor; u8 patch;} version{};
    version.major = data[4];
    version.minor = data[5];
    version.patch = data[6];

    //read the size information
    u64 vertSize = 0;
    memcpy(&vertSize, data.data() + 7, sizeof(vertSize));
    u8  idxSize  = 0;
    memcpy(&idxSize, data.data() + 15, sizeof(idxSize));

    //read the LOD count
    u8 lodCount = 0;
    memcpy(&lodCount, data.data() + 16, sizeof(lodCount));
    //read the LOD data
    u64 maxLodSize = 0;
    std::vector<std::pair<u64, u64>> lodData(lodCount);
    for (u8 i = 0; i < lodCount; ++i) {
        //sanity check truncation
        if (data.size() < (18 + (i+1)*16))
        {throw Exception("File truncated - Invalid / corrupted data", "GLGE::Graphic::Asset::Mesh::load");}

        memcpy(&lodData[i].first, data.data() + 17 + i*16, sizeof(lodData[i].first));
        memcpy(&lodData[i].second, data.data() + 25 + i*16, sizeof(lodData[i].second));
        maxLodSize = (lodData[i].second > maxLodSize) ? lodData[i].second : maxLodSize;
    }
    //store the amount of read bytes (here: 17 + size of LOD table)
    u64 read = 17 + 16*lodCount;

    //for all LODs load them, unzip them and store the unzipped data while filling out the LOD information
    std::vector<u8> vertices;
    std::vector<u8> indices;
    std::vector<GLGE::Graphic::Mesh::LODInfo> LODs(lodCount);
    for (u8 i = 0; i < lodCount; ++i) {
        //sanity check truncation
        if (data.size() < (lodData[i].first + lodData[i].second))
        {throw Exception("File truncated - Invalid / corrupted data", "GLGE::Graphic::Asset::Mesh::load");}

        //get the total
        u64 total = 0;
        memcpy(&total, data.data() + lodData[i].first, sizeof(total));

        //allocate the scratch buffer
        u8* scratch = reinterpret_cast<u8*>(malloc(total));
        if (!scratch) {throw Exception("Failed to allocate the scratch buffer - Memory allocation error", "GLGE::Graphic::Asset::Mesh::load");}

        //then unzip the data
        mz_ulong out_len = total;
        const unsigned char* start = reinterpret_cast<const unsigned char*>(data.data() + lodData[i].first + sizeof(total));
        int status = mz_uncompress(scratch, &out_len, start, lodData[i].second);
        //sanity check the status
        if (status != Z_OK)
        {throw Exception("Failed to decompress data", "GLGE::Graphic::Asset::Mesh::load");}

        //check that the buffer is actually correctly decompressed
        if (out_len != total)
        {throw Exception("Mismatch between expected decompressed size and actual decompressed size - Invalid / Corrupted data", "GLGE::Graphic::Asset::Mesh::load");}

        //read the vertex buffer size
        u64 vSize = 0;
        memcpy(&vSize, scratch, sizeof(vSize));
        //read the index buffer size
        u64 iSize = 0;
        memcpy(&iSize, scratch + sizeof(u64) + vSize, sizeof(iSize));

        //sanity check the read data - they must be multiples of the vertex / index element sizes
        if ((vSize % vertSize) != 0)
        {throw Exception("Invalid vertex buffer size detected. The vertex buffer size must be a multiple of the vertex element size. Invalid / Corrupted data", "GLGE::Graphic::Asset::Mesh::load");}
        if ((iSize % idxSize) != 0)
        {throw Exception("Invalid index buffer size detected. The index buffer size must be a multiple of the index type size. Invalid / Corrupted data", "GLGE::Graphic::Asset::Mesh::load");}

        //sanity that the sizes fit (16 = 2 u64s)
        if ((vSize + iSize + 16) != out_len)
        {throw Exception("Invalid index or vertex buffer size - the size of the vertex and index buffer does not align to the total decompressed buffer size - Invalid / Corrupted data", "GLGE::Graphic::Asset::Mesh::load");}

        //store the vertex and index data
        //vertex storage + LOD update
        size_t offset = vertices.size();
        vertices.resize(offset + vSize);
        memcpy(vertices.data() + offset, scratch + sizeof(vSize), vSize);

        LODs[i].vertex.offset = offset;
        LODs[i].vertex.count = vSize / vertSize;
        LODs[i].vertex.size = vertSize;

        //index storage + LOD update
        offset = indices.size();
        indices.resize(offset + iSize);
        memcpy(indices.data() + offset, scratch + sizeof(vSize) + vSize + sizeof(iSize), iSize);

        LODs[i].index.offset = offset;
        LODs[i].index.count = iSize / idxSize;
        LODs[i].index.size = idxSize;
        
        //update the amount of bytes read
        read += lodData[i].second;

        //clean up the scratch buffer
        free(scratch);
    }

    //store the mesh
    m_mesh.set(vertices.data(), vertSize, vertices.size()/vertSize, reinterpret_cast<u32*>(indices.data()), (indices.size()/idxSize), LODs.data(), LODs.size(), defaultAttributes, sizeof(defaultAttributes)/sizeof(*defaultAttributes));

    //return how much was read
    return read;
}

void GLGE::Graphic::Asset::Mesh::store(std::vector<u8>& out) {
    //a buffer for the output to later append, starting with the version
    std::vector<u8> data = {'M','E','S','H', VERSION_MAJOR,VERSION_MINOR,VERSION_PATCH};

    auto add = [&data](void* start, size_t size) 
        {data.insert(data.end(), reinterpret_cast<u8*>(start), reinterpret_cast<u8*>(start) + size);};

    //store vertex and index size
    u64 vertexSize = m_mesh->getVertexElementSize();
    u8 indexSize = m_mesh->getIndexTypeSize();
    add(&vertexSize, sizeof(vertexSize));
    add(&indexSize,  sizeof(indexSize));
    
    //store the LOD count
    u8 lodCount = m_mesh->getLODCount();
    add(&lodCount, sizeof(lodCount));
    //store LOD informations
    //7: Header, 5: Size info, 4: LOD Count, 16*lodCount: LOD offset Storage, then 1 for next byte
    u64 dataOffset = 7 + 5 + 4 + lodCount * 16 + 1; 
    //allocate a scratch buffer based on the LOD 0 size (this is the largest LOD)
    auto lod = m_mesh->getLODInfo(0);
    u64 size = 16 + lod.vertex.size*lod.vertex.count + lod.index.size*lod.index.count;
    u8* scratch = reinterpret_cast<u8*>(malloc(size));
    //store the zipped data, prepare the size for all LODs if LOD 0 is the largest
    std::vector<u8> zippedData;
    //this reserve call is conservative (better to over-allocate a bit then to resize). 
    //The assumption is that the LOD 0 is the maximum and that the total LOD sizes follows the curve `sizeof(LOD 0)*(3/4)^(lod ID)`
    //Integrating this gives an assumption for the total size of the LOD data: 
    //    F(lod ID) = sizeof(LOD 0) * ((3/4)^(lod ID)) / -0.28768
    //    F(0) = sizeof(LOD 0) / -0.28768
    //    Integral(0, lod Count) = (sizeof(LOD 0) * ((3/4)^(lod count))) / -0.28768) - (sizeof(LOD 0) / -0.28768)
    //  Note: -3.47608 = 1 / -0.28768, Float multiplication is faster than division
    zippedData.reserve((size*(glm::pow(0.75, lodCount) * -3.47608)) - (size * -3.47608));
    //store the current buffer offset
    u64 buffOffset = 0;
    //zip all the data
    for (size_t i = 0; i < lodCount; ++i) {
        lod = m_mesh->getLODInfo(i);
        //compute the total required data size
        u64 total = 2*sizeof(u64) + lod.vertex.count*lod.vertex.size + lod.index.count*lod.index.size;
        //if the total is larger than the scratch buffer, re-allocate it
        if (total > size) {
            free(scratch);
            scratch = reinterpret_cast<u8*>(malloc(total));
            size = total;
        }
        //prepare the data
        u64 vSize = lod.vertex.count*lod.vertex.size;
        memcpy(scratch, &vSize, sizeof(vSize));
        memcpy(scratch + sizeof(vSize), m_mesh->getVertexData(), vSize);
        u64 iSize = lod.index.count*lod.index.size;
        memcpy(scratch + sizeof(vSize) + vSize, &iSize, sizeof(iSize));
        memcpy(scratch + sizeof(vSize) + vSize + sizeof(iSize), m_mesh->getIndexData(), iSize);
        
        //write total and then zip the data
        mz_ulong reqCompressedSize = mz_compressBound(total) + sizeof(total);
        zippedData.resize(zippedData.size() + reqCompressedSize);
        //write the total size
        memcpy(zippedData.data() + buffOffset, &total, sizeof(total));
        //then zip
        int status = mz_compress(zippedData.data() + buffOffset + sizeof(total), &reqCompressedSize, scratch, total);
        //sanity check the status
        if (status != MZ_OK) 
        {throw Exception("Failed to compress some mesh data", "GLGE::Graphic::Asset::Mesh::store");}
        //size overridden, need to re-add the total size
        reqCompressedSize += sizeof(total);

        //store where the data is stored and the actual data size
        add(&dataOffset, sizeof(dataOffset));
        u64 s = reqCompressedSize;
        add(&s, sizeof(s));
        //increase the offset by the zipped size (now true zipped size)
        buffOffset += reqCompressedSize;
        dataOffset += reqCompressedSize;
    }
    //free the scratch buffer
    free(scratch);

    //store the actual data (buffOffset stores the sum of the actual sizes, so the total buffer size)
    add(zippedData.data(), buffOffset);

    //return
    size_t beg = out.size();
    out.insert(out.end(), data.begin(), data.end());
}

void GLGE::Graphic::Asset::Mesh::import_from(const std::filesystem::path& file, u32 format) {
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
        load(data);
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

        //store the index and vertex buffer
        std::vector<Vertex> vertices(mesh->mNumVertices);
        std::vector<u32> indices(mesh->mNumFaces * 3); //faces are always triangles, so times 3 is safe

        //load all vertices
        for (size_t i = 0; i < vertices.size(); ++i) {
            //position is guaranteed to exist
            vertices[i].pos.x = mesh->mVertices[i].x;
            vertices[i].pos.y = mesh->mVertices[i].y;
            vertices[i].pos.z = mesh->mVertices[i].z;

            //check if texture coordinates 0 exist
            if (mesh->HasTextureCoords(0)) {
                //load the texture coordinates
                vertices[i].uv_0.x = mesh->mTextureCoords[0][i].x;
                vertices[i].uv_0.y = mesh->mTextureCoords[0][i].y;
            }
            //check if normals exist
            if (mesh->HasNormals()) {
                //load the normals
                vertices[i].normal.x = mesh->mNormals[i].x;
                vertices[i].normal.y = mesh->mNormals[i].y;
                vertices[i].normal.z = mesh->mNormals[i].z;
            }
            //check if tangents exist
            if (mesh->HasTangentsAndBitangents()) {
                //load the tangents (bitangents can be inferred from normal and tangent using crossproducts)
                vertices[i].tangent.x = mesh->mTangents[i].x;
                vertices[i].tangent.x = mesh->mTangents[i].x;
                vertices[i].tangent.x = mesh->mTangents[i].x;
            }
            //check if vertex colors 0 exist
            if (mesh->HasVertexColors(0)) {
                //load the vertex colors
                vertices[i].color_0.r = mesh->mColors[0][i].r;
                vertices[i].color_0.g = mesh->mColors[0][i].g;
                vertices[i].color_0.b = mesh->mColors[0][i].b;
                vertices[i].color_0.a = mesh->mColors[0][i].a;
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
            indices[i*3+0] = mesh->mFaces[i].mIndices[0];
            indices[i*3+1] = mesh->mFaces[i].mIndices[1];
            indices[i*3+2] = mesh->mFaces[i].mIndices[2];
        }

        //generate the LOD info
        GLGE::Graphic::Mesh::LODInfo info {
            .vertex {
                .count = vertices.size(),
                .offset = 0,
                .size = sizeof(Vertex)
            }, 
            .index {
                .count = indices.size(),
                .offset = 0,
                .size = 4
            }
        };
        //construct the final mesh
        m_mesh.set(vertices.data(), sizeof(Vertex), vertices.size(), indices.data(), indices.size(), &info, 1, defaultAttributes, sizeof(defaultAttributes)/sizeof(*defaultAttributes));
    }
}

void GLGE::Graphic::Asset::Mesh::export_as(const std::filesystem::path& file, u32 format) {
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