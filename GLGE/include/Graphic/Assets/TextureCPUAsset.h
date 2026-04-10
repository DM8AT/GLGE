/**
 * @file TextureCPUAsset.h
 * @author DM8AT
 * @brief define an asset for textures
 * @version 0.1
 * @date 2026-02-06
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef _GLGE_GRAPHIC_ASSET_TEXTURE_CPU_
#define _GLGE_GRAPHIC_ASSET_TEXTURE_CPU_

//add CPU textures
#include "Graphic/TextureCPU.h"
//add assets
#include "Core/AssetManager.h"
//add optionals
#include "Core/utils/Optional.h"

//use the library namespace
namespace GLGE::Graphic::Asset {

    /**
     * @brief define an asset for a CPU texture
     */
    class TextureCPU : public GLGE::Asset {
    public:

        /**
         * @brief the formats suported for textures
         */
        enum Format : u32 {
            /**
             * @brief the GLGE custom texture format
             * 
             * Stores an image per mip level as well as mip count
             */
            GLGE = 0
        };

        /**
         * @brief load the texture from a GLGE data stream
         * 
         * @param data the data to load from
         * @return `u64` the amount of loaded bytes
         */
        virtual u64 load(const std::vector<u8>& data) override;

        /**
         * @brief store an texture to a GLGE data stream
         * 
         * @param data the data stream to write into
         */
        virtual void store(std::vector<u8>& data) override;

        /**
         * @brief load an cpu texture from a file
         * 
         * @param file the file to load the texture from
         * @param format the format of the texture
         */
        virtual void import_from(const std::filesystem::path& file, u32 format) noexcept(false) override;

        /**
         * @brief store the cpu texture in a file
         * 
         * @param file the path to the file to write to
         * @param format the format of the file to store
         */
        virtual void export_as(const std::filesystem::path& file, u32 format) noexcept(false) override;

        /**
         * @brief access the texture of the asset
         * 
         * @return `GLGE::Graphic::TextureCPU&` a reference to the texture
         */
        inline GLGE::Graphic::TextureCPU& texture() noexcept
        {return *m_tex;}

        /**
         * @brief access the texture of the asset
         * 
         * @return `GLGE::Graphic::TextureCPU&` a reference to the texture
         */
        inline operator GLGE::Graphic::TextureCPU&() noexcept
        {return *m_tex;}

    protected:

        /**
         * @brief store the CPU texture
         */
        GLGE::Optional<GLGE::Graphic::TextureCPU> m_tex;

    };

}

#endif