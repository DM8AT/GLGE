#version 460 core

layout (location = 0) in vec3 v_pos; //Note: In world space

layout (location = 0) flat out vec4 v_color;

struct PerDraw {
    float pointSize;
    int cameraIdx;
    uint padding[2];

    vec4 color;
};

struct CameraData {
    mat4 matrix;
};

layout (binding = 0, std430) buffer u_camBuff {
    CameraData camData[];
} cameraBuff;

layout (binding = 1, std430) buffer u_perDraw {
    PerDraw drawData[];
} perDraw;

void main() {
    PerDraw data = perDraw.drawData[gl_BaseInstance];
    vec4 clip = cameraBuff.camData[data.cameraIdx].matrix * vec4(v_pos, 1);
    gl_Position = clip;

    v_color = data.color;

    if (data.pointSize != 0.f) 
    {gl_PointSize = data.pointSize;}
}