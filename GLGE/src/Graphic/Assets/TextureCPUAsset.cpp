/**
 * @file TextureCPUAsset.cpp
 * @author DM8AT
 * @brief implement the loading and storing for textures
 * @version 0.1
 * @date 2026-02-06
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the CPU texture asset
#include "Graphic/Assets/TextureCPUAsset.h"
//add the CPU image asset (CPU textures use images internally)
#include "Graphic/Assets/ImageCPUAsset.h"

//add zip stuff
#include "miniz/miniz.h"

//for file io
#include <fstream>

GLGE::u64 GLGE::Graphic::Asset::TextureCPU::load(const std::vector<u8>& data) {
    //cumulate the amount of used bytes
    u64 sumUsed = 1;
    //get the mip count
    u8 mipCount = data.front();

    //store an iterator to the current starting position
    auto it = data.begin();
    ++it;
    //iterate over all mips and load the images
    std::vector<GLGE::Graphic::ImageCPU> mips;
    mips.reserve(mipCount);
    for (u8 i = 0; i < mipCount; ++i) {
        //use a temporary image asset to load the image
        GLGE::Graphic::Asset::ImageCPU asset;
        u64 used = asset.load(std::vector<u8>(it, data.end()));
        it += used;
        sumUsed += used;
        //store the mip
        mips.push_back(asset.image());
    }

    //store the texture
    m_tex = GLGE::Graphic::TextureCPU(mips.data(), mipCount);

    //return the final byte count
    return sumUsed;
}

void GLGE::Graphic::Asset::TextureCPU::store(std::vector<u8>& data) {
    //store the mip level (1 byte)
    data.push_back(m_tex->getMipCount());
    //iterate over all images and store them
    for (auto& [level, img] : *m_tex) {
        //store the image using an temporary asset
        GLGE::Graphic::Asset::ImageCPU asset;
        asset.image() = *img;
        //now store the image
        asset.store(data);
    }
}

void GLGE::Graphic::Asset::TextureCPU::import_from(const std::filesystem::path& file, u32 format) noexcept(false) {
    //if the file is "" stop
    if (file == "")
    {return;}

    //open the file in binary mode, at the end to get the size
    std::ifstream f(file, std::fstream::binary | std::fstream::ate);
    //if the file wasn't opened, error
    if (!f.is_open()) {
        std::stringstream stream;
        stream << "Failed to open file " << file << " to load a texture";
        throw Exception(stream.str(), "GLGE::Graphic::Asset::TextureCPU::import_from");
    }
    //get the size
    size_t s = f.tellg();
    f.seekg(std::fstream::beg);
    //prepare the binary blob
    std::vector<u8> blob(s);
    //read the data
    f.read((char*)blob.data(), s);
    
    //load the full data
    load(blob);
}

void GLGE::Graphic::Asset::TextureCPU::export_as(const std::filesystem::path& file, u32 format) noexcept(false) {
    //compute the full binary blob for the image
    std::vector<u8> blob;
    store(blob);

    //write the data to a file
    std::ofstream f(file, std::fstream::binary);
    f.clear();
    f.write((char*)blob.data(), blob.size());
}