/**
 * @file GLGECamera.cpp
 * @author DM8AT
 * @brief implement the camera attatchment
 * @version 0.1
 * @date 2025-07-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the camera attatchment
#include "GLGECamera.h"

//the graphic instance stores the data - include the graphic instance
#include "GraphicAPI/GLGEGraphicAPI_Instance.h"
//buffers are also needed to access the data
#include "GLGEBuffer.h"

void Camera::onLateUpdate() noexcept
{
    //update the matricies
    updateMatricies();
}

void Camera::onAttatch() noexcept
{
    //setup the GPU data
    initGPUData();
}

void Camera::onRemove() noexcept
{
    //clean the GPU data
    cleanGPUData();
}

void Camera::initGPUData()
{
    //allocate a reagion on the Graphic card
    m_ptr = m_object->getInstance()->getGraphicInstance()->getCameraBuffer()->getMemoryArena()->allocate(sizeof(m_data));

    //init done
    m_update.unlock();

    //update the matricies as part of the GPU data initalization
    updateMatricies();
}

void Camera::cleanGPUData()
{
    //stop if the GPU instance dosn't exist
    if (!m_object->getInstance()->getGraphicInstance()) {return;}
    //free the GPU pointer
    m_object->getInstance()->getGraphicInstance()->getCameraBuffer()->getMemoryArena()->release(m_ptr);
}

void Camera::updateMatricies()
{
    //check if the GPU context does still exist
    if (!m_object->getInstance()->getGraphicInstance()) {return;}

    //check for the aspect
    float asp = ((float)m_target->getSize().y) / ((float)m_target->getSize().x);

    //check if the aspect changed
    if (asp != m_aspect)
    {
        //store the new projection matrix
        m_aspect = asp;
        //and queue the update for the projection matrix
        m_updateProj = true;
    }

    //if nothing changed, early out
    if (!(m_updateProj || m_object->didMove() || m_object->didRotate())) {return;}
    
    //wait for the start of an update
    m_update.lock();

    //check if the projection matrix should be updated
    if (m_updateProj)
    {
        //re-calculate the projection matrix
        float tfh = tan((m_fov * GLGE_DEGREES_TO_RADIANS) * 0.5);
        float ifmn = 1. / (m_far - m_near);
        m_data.projection = mat4(
            tfh * m_aspect,0,0,0,
            0,tfh,0,0,
            0,0, -(m_far + m_near) * ifmn, -(2.*m_far*m_near)*ifmn,
            0,0,-1,0
        );
        //also re-calcualte the inverse of the matrix
        m_data.invProjection = m_data.projection.inverse();

        //projection data may have change, re-uploade it
        m_data.projectionSetup = vec3(m_fov, m_near, m_far);
    }

    //check if the rotation should change
    if (m_object->didRotate())
    {
        //re-calculate the rotation matrix
        m_data.rotation = m_object->getRot().getUVNMatrix();
    }

    //if ether rotation or view has changed re-compute the inverse view
    if (!m_updateProj)
    {
        //re-calculate the view matrix
        m_data.view = m_data.rotation * mat4(
                        1,0,0, -m_object->getPos().x, 
                        0,1,0, -m_object->getPos().y, 
                        0,0,1, -m_object->getPos().z, 
                        0,0,0, 1
        );
        //re-compute the inverse
        m_data.invView = m_data.view.inverse();
    }

    //re-uploade the data
    m_object->getInstance()->getGraphicInstance()->getCameraBuffer()->getMemoryArena()->update(m_ptr, (void*)(&m_data));

    //remove the update flag for the projection
    m_updateProj = false;
    //the update flags of the object will be cleared by the object

    //update is done
    m_update.unlock();
}