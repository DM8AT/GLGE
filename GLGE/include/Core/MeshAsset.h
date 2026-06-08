/**
 * @file MeshAsset.h
 * @author DM8AT
 * @brief define a mesh asset
 * @version 0.1
 * @date 2026-06-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_CORE_MESH_ASSET_
#define _GLGE_CORE_MESH_ASSET_

//add assets
#include "Asset.h"
//add meshes
#include "Mesh.h"

//use the library namespace
namespace GLGE {

    /**
     * @brief define the mesh class
     */
    class MeshAsset : public Asset {
    public:

        /**
         * @brief define the supported file formats
         */
        enum Format : u32 {
            /**
             * @brief the file is a GLGE-Specific mesh
             */
            GLGE = 0,
            /**
             * @brief use ASSIMP to import a general purpose mesh
             * 
             * This will build an LOD chain automatically and generate per-LOD BVHs. 
             * The parameters for the LOD chain are automatically determined internally and the BVH is build using high building quality. 
             */
            ASSIMP
        };

        /**
         * @brief Construct a new Mesh Asset
         */
        MeshAsset() = default;

        /**
         * @brief Destroy the Asset
         */
        virtual ~MeshAsset() override {}

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
         * @brief Set the Mesh
         * 
         * @param mesh a shared pointer to the mesh
         */
        inline void setMesh(const std::shared_ptr<Mesh>& mesh)
        {m_mesh = mesh;}

        /**
         * @brief Get the Mesh
         * 
         * @return `const std::shared_ptr<Mesh>&` a constant reference to the mesh
         */
        inline const std::shared_ptr<Mesh>& getMesh() const noexcept
        {return m_mesh;}

    protected:

        /**
         * @brief store the loaded mesh
         */
        std::shared_ptr<Mesh> m_mesh;

    };

}

#endif