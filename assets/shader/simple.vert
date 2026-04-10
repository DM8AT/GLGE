#version 460 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec2 v_tex;
layout (location = 2) in vec3 v_normal;

layout (location = 0) out vec2 f_tex;
layout (location = 1) out vec3 f_normal;
layout (location = 2) out vec3 f_pos;

layout (set = 0, binding = 0, std140) uniform cam_buffer {
    float x;
    float y;
    float z;
    uint quat_i_j;
    uint quat_k_w;
    float fov;
    float near;
    float far;
    mat4 proj;
} cam;

struct TransformData {
    float x;
    float y;
    float z;
    uint quat_i_j;
    uint quat_k_w;
    float sx;
    float sy;
    float sz;
};

layout (set = 0, binding = 1, std430) buffer transf_buffer {
    TransformData transf[];
} transforms;

vec4 decodeQuat(uint i_j, uint k_w) {
    //decompress and return the quaternions as a vector
    return vec4(
        (float((i_j & 0xffff) & 0xffff) / float(0xffff)) * 2.f - 1.f,
        (float((i_j >> 16)    & 0xffff) / float(0xffff)) * 2.f - 1.f,
        (float((k_w & 0xffff) & 0xffff) / float(0xffff)) * 2.f - 1.f,
        (float((k_w >> 16)    & 0xffff) / float(0xffff)) * 2.f - 1.f
    );
}

mat3 quatToMat3(vec4 q) {
    float x = q.x, y = q.y, z = q.z, w = q.w;

    return mat3(
        1.0 - 2.0*y*y - 2.0*z*z,
        2.0*x*y + 2.0*w*z,
        2.0*x*z - 2.0*w*y,

        2.0*x*y - 2.0*w*z,
        1.0 - 2.0*x*x - 2.0*z*z,
        2.0*y*z + 2.0*w*x,

        2.0*x*z + 2.0*w*y,
        2.0*y*z - 2.0*w*x,
        1.0 - 2.0*x*x - 2.0*y*y
    );
}

void main() {
    //decode camera rotation
    vec4 q = decodeQuat(cam.quat_i_j, cam.quat_k_w);
    mat3 rot = quatToMat3(q);

    //load the transform
    TransformData tData = transforms.transf[gl_BaseInstance];
    //decode transform rotation
    vec4 tq = decodeQuat(tData.quat_i_j, tData.quat_k_w);
    mat3 vRot = quatToMat3(tq);

    //camera position
    vec3 camPos = vec3(cam.x, cam.y, cam.z);

    //view transform
    vec3 worldPos = (vRot * (v_pos * vec3(tData.sx, tData.sy, tData.sz))) + vec3(tData.x, tData.y, tData.z);
    vec3 viewPos = (worldPos - camPos) * rot;

    //final projection
    gl_Position = cam.proj * vec4(viewPos, 1);

    //pass through
    f_tex    = v_tex;
    f_normal = vRot * v_normal;
    //set the fragment world position
    f_pos    = worldPos;
}