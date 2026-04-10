/**
 * @file ImageCPUAsset.h
 * @author DM8AT
 * @brief define an asset for a cpu images
 * @version 0.1
 * @date 2026-02-05
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_ASSET_IMAGE_CPU_
#define _GLGE_GRAPHIC_ASSET_IMAGE_CPU_

//add the asset system
#include "Core/AssetManager.h"
//add CPU images
#include "Graphic/ImageCPU.h"

//use the library namespace
namespace GLGE::Graphic {
/**
 * @brief a namespace that contains all graphic-related assets
 */
namespace Asset {

    /**
     * @brief store the CPU image asset
     */
    class ImageCPU : public GLGE::Asset {
    public:

        /**
         * @brief store the supported load / store formats
         */
        enum Format : u32 {
            /**
             * @brief use GLGE low-compression, fast load / store format
             * 
             * Utilized for engine-internal, default storage for assets. 
             * 
             * They can be stored in compound-assets. 
             */
            GLGE = 0,
            /**
             * @brief default PNG format
             * 
             * normal for lossless compressions
             */
            PNG,
            /**
             * @brief normal jpg / jpeg format
             * 
             * normal for lossy compression
             */
            JPG,
            /**
             * @brief A simple, lossless image format
             */
            TGA,
            /**
             * @brief a simple windows image format
             */
            BMP,
            /**
             * @brief used for high-dynamic range images
             * 
             * This is usefull if you have colors that range from more than 0.f to 1.f (or 0 to 255)
             */
            HDR,

            /**
             * @brief alias for jpg format (jpeg and jpg are the same format)
             */
            JPEG = JPG
        };

        /**
         * @brief load the image from a GLGE data stream
         * 
         * @param data the data to load from
         * @return `u64` the amount of loaded bytes
         */
        virtual u64 load(const std::vector<u8>& data) override;

        /**
         * @brief store an image to a GLGE data stream
         * 
         * @param data the data stream to write into
         */
        virtual void store(std::vector<u8>& data) override;

        /**
         * @brief load an cpu image from a file
         * 
         * @param file the file to load the image from
         * @param format the format of the image
         */
        virtual void import_from(const std::filesystem::path& file, u32 format) noexcept(false) override;

        /**
         * @brief store the cpu image in a file
         * 
         * @param file the path to the file to write to
         * @param format the format of the file to store
         */
        virtual void export_as(const std::filesystem::path& file, u32 format) noexcept(false) override;

        /**
         * @brief access the image of the asset
         * 
         * @return `GLGE::Graphic::ImageCPU&` a reference to the internal image
         */
        inline ::GLGE::Graphic::ImageCPU& image() noexcept
        {return m_img;}

        /**
         * @brief access the image of the asset
         * 
         * @return `GLGE::Graphic::ImageCPU&` a reference to the internal image
         */
        inline operator ::GLGE::Graphic::ImageCPU&() noexcept
        {return m_img;}

    protected:

        /**
         * @brief store the image
         */
        GLGE::Graphic::ImageCPU m_img;

    };

}
}

#endif