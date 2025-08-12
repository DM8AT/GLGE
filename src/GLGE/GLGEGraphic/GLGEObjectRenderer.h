/**
 * @file GLGEObjectRenderer.h
 * @author DM8AT
 * @brief define a class that can be added to an object to make it renderable
 * @version 0.1
 * @date 2025-07-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_OBJECT_RENDERER_
#define _GLGE_OBJECT_RENDERER_

//include objects and settings
#include "../GLGECore/Geometry/GLGEObject.h"
//include render materials and render meshes - the object renderer is responsible of pairing them up
#include "GLGERenderMaterial.h"
#include "GLGERenderMesh.h"

//check for C++
#if GLGE_CPP

/**
 * @brief map a render mesh to a render material
 */
struct RenderPair
{
    /**
     * @brief store the mesh for the pair
     * @warning NULL results in undefined behaviour, must be a valid pointer
     */
    RenderMesh* mesh;
    /**
     * @brief store the material for the pair
     * @warning NULL results in undefined behaviour, must be a valid pointer
     */
    RenderMaterial* material;

    /**
     * @brief Construct a new Render Pair
     * @warning DO NOT USE! this only exists for std::vectors
     */
    RenderPair() : mesh(0), material(0) {}

    /**
     * @brief Construct a new Render Pair
     * 
     * @param _mesh a pointer to the mesh for the pair
     * @param _material a pointer to the material for the pair
     */
    RenderPair(RenderMesh* _mesh, RenderMaterial* _material)
     : mesh(_mesh), material(_material)
    {}
};

/**
 * @brief a class that can be used to mark an object as renderable. This will manage all GPU-Side recources of the object. 
 * 
 * This class manages all render meshes applied to the object
 */
class ObjectRenderer : public ObjectAttatchable
{
public:

    /**
     * @brief Construct a new Object Renderer
     * Explicetly deleted, at least one render mesh - render material pair must exist
     */
    ObjectRenderer() = delete;

    /**
     * @brief Construct a new Object Renderer
     * 
     * @param pairs a pointer to an array of pairs
     * @param pairCount the amount of elements in the array
     */
    ObjectRenderer(RenderPair* pairs, uint64_t pairCount);

    /**
     * @brief Construct a new Object Renderer
     * 
     * @param pairs an std::vector of all mesh pairs to render
     */
    inline ObjectRenderer(std::vector<RenderPair> pairs)
     : ObjectRenderer(pairs.data(), pairs.size())
    {}

    /**
     * @brief Get all meshes mapped to the materials
     * 
     * @return const std::unordered_map<RenderMaterial*, std::vector<RenderMesh*>>& the map from material -> list of meshes
     */
    inline const std::unordered_map<RenderMaterial*, std::vector<RenderMesh*>>& getToRender() const noexcept {return m_toRender;}

    /**
     * @brief Get the Type Name of the object renderer
     * 
     * @return const char* the type name of the object renderer
     */
    inline virtual const char* getTypeName() const noexcept override {return "GLGE_OBJECT_RENDERER";}

    /**
     * @brief Get all data that this object requests to render
     * 
     * @return std::unordered_map<RenderMaterial*, std::vector<RenderMesh*>>& a reference to the data to render
     */
    inline std::unordered_map<RenderMaterial*, std::vector<RenderMesh*>>& getToRender() noexcept {return m_toRender;}

    /**
     * @brief hook that is called late in the update cycle
     * @warning don't use for movement
     */
    virtual void onLateUpdate() noexcept override;

    /**
     * @brief Get the Pointer to the transform data
     * 
     * @return const GraphicMemoryArena::GraphicPointer& the pointer to the transform data
     */
    inline const GraphicMemoryArena::GraphicPointer& getPointer() const noexcept {return m_objPointer;}

    /**
     * @brief store the data to transfear to the GPU
     */
    struct GPUData
    {
        /**
         * @brief store the local to world matrix
         */
        mat4 worldMat;
        /**
         * @brief store the inverse of the local to world matrix
         */
        mat4 invWorldMat;
        /**
         * @brief store only the roataion part of the local to world matrix
         */
        mat4 rotation;
    };

protected:

    /**
     * @brief hook when the renderer is attatched to the object
     */
    virtual void onAttatch() noexcept override;

    /**
     * @brief hook when the renderer is removed from the object
     */
    virtual void onRemove() noexcept override;

    /**
     * @brief store the GPU data for the instance
     */
    GPUData m_gpuData;

    /**
     * @brief store a mutex to stop updates from interfearing with the init and clean up
     */
    std::mutex m_updateMut;

    /**
     * @brief store the pointer to the buffer to store the transformation
     */
    GraphicMemoryArena::GraphicPointer m_objPointer = {0,0};
    /**
     * @brief store a mapping from a render material to a list of render meshes that use the material
     * All meshes belong to the object this renderer belongs to
     */
    std::unordered_map<RenderMaterial*, std::vector<RenderMesh*>> m_toRender;

};

#endif //end of C++ section

#endif