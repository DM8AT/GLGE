/**
 * @file GLGECamera.h
 * @author DM8AT
 * @brief define the camera object attatchment for GLGE
 * @version 0.1
 * @date 2025-05-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef _GLGE_CAMERA_
#define _GLGE_CAMERA_

//include the settings
#include "../GLGECore/GLGESettings.h"
//include objects
#include "../GLGECore/Geometry/GLGEObject.h"
//include graphic memory arenas to store the data on the GPU
#include "GraphicAPI/GLGEGraphicAPI_MemoryArena.h"
//include framebuffers to provide a target
#include "GLGEFramebuffer.h"

//check for C++
#if GLGE_CPP

/**
 * @brief create the camera class. Cameras are object attatchments. 
 */
class Camera final : public ObjectAttatchable
{
public:

    /**
     * @brief Construct a new Camera
     * Explicetly deleted - a target must be provided
     */
    Camera() = delete;

    /**
     * @brief Construct a new Camera with the minimum of inputs
     * 
     * @param target a pointer to the framebuffer the camera will render to
     * 
     * FOV = 90°
     * near = 0.1 world units
     * far = 1000 world units
     */
    Camera(Framebuffer* target)
     : m_target(target), m_fov(90), m_near(0.1), m_far(1000)
    {m_update.lock();}

    /**
     * @brief Construct a new Camera
     * 
     * @param target the framebuffer the camera will render to
     * @param FOV the field of view in degrees
     * @param near the near clipping plane in world units
     * @param far the far clipping plane in world units
     * 
     * @warning the far clipping vlaue must be greater than the near clipping value. Everything else results in undefined behaviour
     */
    Camera(Framebuffer* target, float FOV, float near, float far)
     : m_target(target), m_fov(FOV), m_near(near), m_far(far)
    {m_update.lock();}

    /**
     * @brief Destroy the Camera
     */
    virtual ~Camera() {}

    /**
     * @brief update the camera element
     * Late update to catch all update flags
     */
    virtual void onLateUpdate() noexcept;

    /**
     * @brief Get the type name of the object attatchable
     * 
     * @return const char* the type-specific name to identify the camera
     */
    virtual const char* getTypeName() const noexcept {return "GLGE_CAMERA";}

    /**
     * @brief get the field of view used by the camera
     * 
     * @return float the field of view in degrees
     */
    inline float getFOV() const noexcept {return m_fov;}

    /**
     * @brief change the field of view of the camera
     * 
     * @param FOV the field of view of the camera
     */
    inline void setFOV(float FOV) noexcept {if (FOV != m_fov) {m_fov = FOV; m_updateProj = true;}}

    /**
     * @brief Get the near clipping plane of the camera
     * 
     * @return float the near clipping plane of the camera in world units
     */
    inline float getNear() const noexcept {return m_near;}

    /**
     * @brief Set the near clipping plane of the camera
     * 
     * @param near the near clipping plane of the camera in world units
     */
    inline void setNear(float near) noexcept {if (near != m_near) {m_near = near; m_updateProj = true;}}

    /**
     * @brief Get the far clippling plane of the camera
     * @warning must be smaller than the far clipping plane, everything else results in undefined behaviour
     * 
     * @return float the far clipping plane of the camera in world units
     */
    inline float getFar() const noexcept {return m_far;}

    /**
     * @brief Set the far clipping plane of the camera
     * @warning must be bigger than the near clipping plane, everything else results in undefined behaviour
     * 
     * @param far the far clipping plane of the camera in world units
     */
    inline void setFar(float far) {if (far != m_far) {m_far = far; m_updateProj = true;}}

    /**
     * @brief Get the render target of the camera
     * 
     * @return Framebuffer* a pointer to the target framebuffer object
     */
    inline Framebuffer* getTarget() const noexcept {return m_target;}

protected:

    /**
     * @brief a function that is called when the object is attatched to an object
     */
    virtual void onAttatch() noexcept override;

    /**
     * @brief a function that is called when the object is removed from an object
     */
    virtual void onRemove() noexcept override;

    /**
     * @brief initalize the GPU data storage
     */
    void initGPUData();
    /**
     * @brief clean up the GPU data storage
     */
    void cleanGPUData();
    /**
     * @brief calculate the update for all matricies
     */
    void updateMatricies();

    /**
     * @brief store a pointer to the render target of the camera
     */
    Framebuffer* m_target = 0;
    /**
     * @brief store the aspect ratio of the target framebuffer
     */
    float m_aspect = 1.;

    /**
     * @brief store the field of view of the camera
     */
    float m_fov = 90.;
    /**
     * @brief store the near clipping plane of the camera
     */
    float m_near = 0.1;
    /**
     * @brief store the far clipping plane of the camera
     */
    float m_far = 1000.;

    /**
     * @brief store if the projection matrix should be updated
     */
    bool m_updateProj = true;

    /**
     * @brief store a pointer to the GPU data
     */
    GraphicMemoryArena::GraphicPointer m_ptr = {0,0};

    /**
     * @brief store if the camera is set up
     */
    std::mutex m_update;

    /**
     * @brief store the camera data for the GPU
     */
    struct CameraData
    {
        /**
         * @brief store how the camera is set up
         * x = FOV
         * y = near clipping plane
         * z = far clipping plane
         */
        vec3 projectionSetup;

        /**
         * @brief buffer element for shader storage allignment
         */
        float buffer;

        /**
         * @brief store the view matrix
         */
        mat4 view;
        /**
         * @brief store the inverse view matrix
         */
        mat4 invView;

        /**
         * @brief store the rotation-only part of the view matrix
         */
        mat4 rotation;

        /**
         * @brief store the projection matrix
         */
        mat4 projection;
        /**
         * @brief store the inverse projection matrix
         */
        mat4 invProjection;
    } m_data;

};

#endif

#endif