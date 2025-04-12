#version 460 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec2 v_tex;
layout (location = 2) in vec3 v_norm;

layout (location = 0) out vec2 f_tex;

layout (location = 1) flat out uint v_submeshIndex;

struct ObjectData {
    mat4 rotMat;
    mat4 transMat;
};

layout (set = 2, binding = 0) readonly buffer objData
{
    ObjectData perObject[];
};

struct CameraData {
    mat4 rotMat;
    mat4 transMat;
    mat4 projMat;
    mat4 toWorld;
};

layout (set = 3, binding = 0) readonly buffer camData
{
    CameraData perCamera[];
};

struct WindowData {
    uvec4 posSize;
    uint camIdx;
};

layout (set = 4, binding = 0, std430) readonly buffer winData
{
    WindowData perWindow[];
};

layout (push_constant) uniform constants {
    uint submeshIndex;
    uint materialIndex;
    uint objectIndex;
    uint windowIndex;
};

void main()
{
    //just pass through the texture coordinates
    f_tex = v_tex;
    //calculate the window's aspect ratio
    float asp = float(perWindow[windowIndex].posSize.w) / float(perWindow[windowIndex].posSize.z);
    //store the camera index
    int camIdx = int(perWindow[windowIndex].camIdx);

    //compute the world position
    vec4 pos = vec4(v_pos,1) * perObject[objectIndex].transMat;
    //check if a camera exists
    if (camIdx != 4294967295)
    {
        //convert to camera and then clip space
        pos *= perCamera[camIdx].toWorld * perCamera[camIdx].projMat;
    }
    //correct the window's aspect ratio
    pos.x *= asp;
    //store the position
    gl_Position = pos;
}