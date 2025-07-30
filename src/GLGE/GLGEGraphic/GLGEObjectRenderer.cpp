/**
 * @file GLGEObjectRenderer.cpp
 * @author DM8AT
 * @brief implement the object renderer that can be attatched to an object
 * @version 0.1
 * @date 2025-07-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the object renderer
#include "GLGEObjectRenderer.h"

ObjectRenderer::ObjectRenderer(RenderPair* pairs, uint64_t pairCount)
{
    //iterate over all pairs
    for (uint64_t i = 0; i < pairCount; ++i)
    {
        //check if the material exists
        if (m_toRender.find(pairs[i].material) == m_toRender.end())
        {
            //if it dosn't
            //add a new element with the material and the mesh
            m_toRender.try_emplace(pairs[i].material, std::vector<RenderMesh*>({pairs[i].mesh}));
        }
        else
        {
            //if it exists
            //just add the element to the vector
            m_toRender[pairs->material].emplace_back(pairs[i].mesh);
        }
    }
}

void ObjectRenderer::onLateUpdate() noexcept
{
    //lock the update mutex
    m_updateMut.lock();
    //if the object was not moved, rotated or scaled than just stop
    if (!(m_object->didMove() || m_object->didRotate() || m_object->wasScaled())) {m_updateMut.unlock();;return;}

    //stop if the graphic instance is deleted
    if (!m_object->getInstance()->getGraphicInstance()) {m_updateMut.unlock();return;}

    //re-calculate the rotation
    m_gpuData.rotation = m_object->getRot().getMat4();
    //calculate the new to world matrix
    m_gpuData.worldMat = (mat4(
        m_object->getScale().x, 0, 0, 0,
        0, m_object->getScale().y, 0, 0,
        0, 0, m_object->getScale().z, 0, 
        0, 0, 0,                      1
    ) * m_gpuData.rotation) * mat4(
        1, 0, 0, m_object->getPos().x, 
        0, 1, 0, m_object->getPos().y,
        0, 0, 1, m_object->getPos().z, 
        0, 0, 0, 1
    );
    //calculate the inverse
    m_gpuData.invWorldMat = m_gpuData.worldMat.inverse();

    //re-upload the data
    m_object->getInstance()->getGraphicInstance()->getObjectTransfBuffer()->getMemoryArena()->update(m_objPointer, &m_gpuData);

    //unlock the update mutex - update finished
    m_updateMut.unlock();
}

void ObjectRenderer::onAttatch() noexcept
{
    //lock the update mutex - no update should occoure
    m_updateMut.lock();
    //allocate the space in the GPU buffer
    m_objPointer = m_object->getInstance()->getGraphicInstance()->getObjectTransfBuffer()->getMemoryArena()->allocate(sizeof(m_gpuData));
    //updates may occoure again
    m_updateMut.unlock();
}

void ObjectRenderer::onRemove() noexcept
{
    //lock the update mutex - update should not occoure
    m_updateMut.lock();
    //free the data from the memory arena if it exists
    if (m_object->getInstance()->getGraphicInstance())
    {m_object->getInstance()->getGraphicInstance()->getObjectTransfBuffer()->getMemoryArena()->release(m_objPointer);}
}