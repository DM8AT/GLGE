/**
 * @file ImageCPUAsset.cpp
 * @author DM8AT
 * @brief implement the loading functionality for the CPU asset
 * @version 0.1
 * @date 2026-02-05
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add CPU assets
#include "Graphic/Assets/ImageCPUAsset.h"

//include stb stuff
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

//add file IO
#include <fstream>
//add miniz (for zip compression / decompression)
#include "miniz/miniz.h"

GLGE::u64 GLGE::Graphic::Asset::ImageCPU::load(const std::vector<u8>& inp) {
    //grab the uncompressed size
    u64 expectedSize = 0;
    memcpy((u8*)&expectedSize, inp.data(), 8);
    //store the final data
    std::vector<u8> data;
    data.reserve(expectedSize);
    //decompress everything
    mz_ulong out_len = expectedSize;
    int status = mz_uncompress(data.data(), &out_len, inp.data()+8, inp.size()-8);
    //sanity check the status
    if (status != Z_OK)
    {throw Exception("Failed to decompress data", "GLGE::Graphic::Asset::ImageCPU::load");}

    //read the size
    uvec2 size;
    size.x = *((u32*)data.data());
    size.y = *(((u32*)data.data()) + 1);
    //read the format
    PixelFormat form = *((PixelFormat*)(data.data() + 8));
    //create the internal image
    m_img = GLGE::Graphic::ImageCPU(data.data() + (8 + sizeof(PixelFormat)), form, size);

    //return the amount of used bytes
    return data.size()+8;
}

void GLGE::Graphic::Asset::ImageCPU::store(std::vector<u8>& out) {
    //compute the full size of the data to store
    size_t bitsPerPixel = (m_img.getFormat().r_Bitcount + m_img.getFormat().g_Bitcount + m_img.getFormat().b_Bitcount + m_img.getFormat().a_Bitcount);
    size_t bytesPerPixel = bitsPerPixel >> 3;
    size_t req = 2*4 + sizeof(GLGE::Graphic::PixelFormat); //size of the x,y dimension storage and format metadata
    size_t dataSize = bytesPerPixel * m_img.getSize().x * m_img.getSize().y;
    req += dataSize;
    //make enough space in the temporary
    std::vector<u8> data;
    data.reserve(data.size() + req);
    //store the size
    uvec2 size = m_img.getSize();
    data.insert(data.end(), (u8*)(&size.x), (u8*)((&size.x) + 1));
    data.insert(data.end(), (u8*)(&size.y), (u8*)((&size.y) + 1));
    //store the format
    GLGE::Graphic::PixelFormat format = m_img.getFormat();
    data.insert(data.end(), (u8*)&format, (u8*)(&format + 1));
    //store all the binary data
    data.insert(data.end(), (u8*)m_img.getRaw(), ((u8*)m_img.getRaw()) + dataSize);

    //compress the data using miniz
    mz_ulong compressedSize = mz_compressBound(data.size());
    size_t offs = out.size() + 8; //also store the size as a u64
    out.resize(offs + compressedSize);
    int status = mz_compress(out.data() + offs, &compressedSize, data.data(), data.size());

    //sanity check the status
    if (status != Z_OK) 
    {throw Exception("Failed to compress CPU image during image storing", "GLGE::Graphic::Asset::ImageCPU::store");}

    //store the before-compression size
    u64 beforeSize = data.size();
    memcpy(out.data() + (offs-8), (u8*)&beforeSize, 8);
    //make the size fit exactly
    out.resize(offs + compressedSize);
}

void GLGE::Graphic::Asset::ImageCPU::import_from(const std::filesystem::path& file, u32 format) noexcept(false) {
    //empty path ("") means to just stop
    if (file == "") {return;}

    //some data is needed to be handled specially
    if (format == Format::GLGE) {
        //open the file in a binary data stream, create a vector from it and just use the GLGE load function
        std::ifstream f(file, std::fstream::binary | std::fstream::ate);

        //sanity check
        if (!f.is_open()) {
            std::stringstream stream;
            stream << "Failed to open file " << file << " during importing CPU image";
            throw Exception(stream.str(), "GLGE::Graphic::Asset::ImageCPU::import_from");
        }

        size_t s = f.tellg();
        f.seekg(std::fstream::beg);
        std::vector<u8> data;
        data.resize(s);
        f.read((char*)data.data(), s);

        //parse the GLGE data
        load(data);
    } else if (format == Format::HDR) {
        //hdr loads as floats
        ivec2 size;
        int channels;
        float* data = stbi_loadf(file.c_str(), &size.x, &size.y, &channels, 0);

        //construct the image format
        GLGE::Graphic::PixelFormat form = (channels == 1) ? GLGE::Graphic::PIXEL_FORMAT_R_32_FLOAT : 
                                          (channels == 2) ? GLGE::Graphic::PIXEL_FORMAT_RG_32_FLOAT : 
                                          (channels == 3) ? GLGE::Graphic::PIXEL_FORMAT_RGB_32_FLOAT : 
                                          GLGE::Graphic::PIXEL_FORMAT_RGBA_32_FLOAT;
        //load the data to the local image
        m_img = GLGE::Graphic::ImageCPU(data, form, uvec2{size.x, size.y});
        //clean up
        STBI_FREE(data);
    } else {
        //all other data is loaded with the same function
        ivec2 size;
        int channels;
        void* data = stbi_load(file.c_str(), &size.x, &size.y, &channels, 4);
        //format fixed as RGBA8_UNORM
        //load the data to the local image
        m_img = GLGE::Graphic::ImageCPU(data, GLGE::Graphic::PIXEL_FORMAT_RGBA_8_UNORM, uvec2{size.x, size.y});
        //clean up
        STBI_FREE(data);
    }
}

void GLGE::Graphic::Asset::ImageCPU::export_as(const std::filesystem::path& file, u32 format) noexcept(false) {
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
    //export as PNG
    case Format::PNG: {
            //convert to an acceptable format
            GLGE::Graphic::ImageCPU img = m_img.toFormat(GLGE::Graphic::PIXEL_FORMAT_RGBA_8_UNORM);
            //store the image
            stbi_write_png(file.c_str(), img.getSize().x, img.getSize().y, 4, img.getRaw(), 0);
        }
        break;
    //export as JPG
    case Format::JPG: {
            //convert to an acceptable format
            GLGE::Graphic::ImageCPU img = m_img.toFormat(GLGE::Graphic::PIXEL_FORMAT_RGBA_8_UNORM);
            //store the image
            stbi_write_jpg(file.c_str(), img.getSize().x, img.getSize().y, 4, img.getRaw(), 0);
        }
        break;
    //export as tga
    case Format::TGA: {
            //convert to an acceptable format
            GLGE::Graphic::ImageCPU img = m_img.toFormat(GLGE::Graphic::PIXEL_FORMAT_RGBA_8_UNORM);
            //store the image
            stbi_write_tga(file.c_str(), img.getSize().x, img.getSize().y, 4, img.getRaw());
        }
        break;
    //export as bitmap
    case Format::BMP: {
            //convert to an acceptable format
            GLGE::Graphic::ImageCPU img = m_img.toFormat(GLGE::Graphic::PIXEL_FORMAT_RGBA_8_UNORM);
            //store the image
            stbi_write_bmp(file.c_str(), img.getSize().x, img.getSize().y, 4, img.getRaw());
        }
        break;
    //export as hdr
    case Format::HDR: {
            //convert to an acceptable format (always use RGBA_32_FLOAT)
            GLGE::Graphic::ImageCPU img = m_img.toFormat(GLGE::Graphic::PIXEL_FORMAT_RGBA_32_FLOAT);
            //store the image
            stbi_write_hdr(file.c_str(), img.getSize().x, img.getSize().y, 4, (const float*)img.getRaw());
        }
        break;
    
    default:
        break;
    }
}