#version 460 core

layout (location = 0) in vec3 v_pos; //Note: In world space

layout (location = 0) flat out vec4 v_color;

struct PerDraw {
    float pointSize;
    int cameraIdx;
    int targetIdx;
    uint padding;

    vec4 color;
};

struct CameraData {
    mat4 matrix;
};

struct TargetData {
    uvec2 extent;
};

layout (binding = 0, std430) buffer s_camBuff {
    CameraData camData[];
} cameraBuff;

layout (binding = 1, std430) buffer s_perDraw {
    PerDraw drawData[];
} perDraw;

layout (binding = 2, std430) buffer s_targetInfo {
    TargetData targetData[];
} targetInfo;

void main() {
    PerDraw data = perDraw.drawData[gl_BaseInstance];
    uvec2 extent = targetInfo.targetData[data.targetIdx].extent;
    float aspect = float(extent.y) / float(extent.x);
    mat4 cameraMatrix = cameraBuff.camData[data.cameraIdx].matrix;
    cameraMatrix[0][0] *= aspect;
    vec4 clip = cameraMatrix * vec4(v_pos, 1);
    gl_Position = clip;

    v_color = data.color;

    if (data.pointSize != 0.f) {
        float pixelSize = data.pointSize * cameraMatrix[1][1] * extent.y / (2.0 * clip.w);
        gl_PointSize = pixelSize;
    }
}