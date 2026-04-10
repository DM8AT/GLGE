/**
 * @file MeshAsset.h
 * @author DM8AT
 * @brief define an asset that can load meshes
 * @version 0.1
 * @date 2026-03-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_ASSET_MESH_
#define _GLGE_GRAPHIC_ASSET_MESH_

//add the asset system
#include "Core/AssetManager.h"
//add meshes
#include "Graphic/Mesh.h"

//use the library namespace
namespace GLGE::Graphic::Asset {

    /**
     * @brief define an asset for meshes
     */
    class Mesh : public GLGE::Asset {
    public:

        /**
         * @brief the formats suported for meshes
         */
        enum Format : u32 {
            /**
             * @brief the GLGE custom mesh format
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
            GLGE = 0,
            /**
             * @brief some file format that can be read using Assimp
             * 
             * This should be used for:
             *  - FBX
             *  - glTF
             *  - glTF 2.0
             *  - Wavefront (OBJ files)
             *  - STL
             *  - PLY
             *  - 3MF
             *  - Blender files
             *  - 3D Studio Max files
             *  - ASE files
             *  - AC3D files
             *  - MilkShape 3D files
             *  - Quake MD2, MD3 and MD5 files
             *  - Doom 3 (.md5*) files
             *  - Unreal Engine psk files
             *  - Valve SMD or VTA mesh files
             *  - Ogre mesh files
             *  - Irrlicht mesh files
             * 
             * 
             * The produced vertex layout is always as follows:
             * ```cpp
             * struct Vertex {
             *     GLGE::vec3 pos;
             *     GLGE::vec2 uv_0;
             *     GLGE::vec3 normal;
             *     GLGE::vec3 tangent;
             *     GLGE::vec4 color_0;
             * };
             * ```
             * The used index format is also always a `GLGE::u32`. This means the size is always 4 bytes per index. 
             */
            ASSIMP = 1
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
         * @brief get the loaded mesh
         * 
         * @return `GLGE::Graphic::Mesh&` a reference to the stored mesh
         */
        inline GLGE::Graphic::Mesh& mesh() noexcept
        {return *m_mesh;}

        /**
         * @brief get the loaded mesh
         * 
         * @return `GLGE::Graphic::Mesh&` a reference to the stored mesh
         */
        inline operator GLGE::Graphic::Mesh&() noexcept
        {return *m_mesh;}

    protected:
    
        /**
         * @brief store the CPU texture
         */
        GLGE::Optional<GLGE::Graphic::Mesh> m_mesh;

    };

}

#endif