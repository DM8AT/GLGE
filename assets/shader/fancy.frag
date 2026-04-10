#version 450 core

layout (location = 0) in vec2 f_tex;
layout (location = 1) in vec3 f_normal;
layout (location = 2) in vec3 f_pos;

layout (location = 0) out vec4 FragColor;

const vec3 lightCol = vec3(1); // vec3(0.7,0.8,0.4);
const float lightStr = 3.f;
const vec3 lightDir = normalize(vec3(0.2,1,-0.3));

const float shininess = 32.0;
const vec3 color = vec3(238, 69, 61)/vec3(255);

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

void main() {
    float lambertian = dot(lightDir, f_normal)*0.5+0.5;

    vec3 viewDir = normalize(f_pos - vec3(cam.x, cam.y, cam.z));
    vec3 halfDir = normalize(lightDir + viewDir);
    float specAngle = max(dot(halfDir, f_normal), 0.0);
    float specular = pow(specAngle, shininess) * ceil(lambertian);

    vec3 combColor = (color * lambertian + specular) * lightCol * lightStr;
    FragColor = vec4(combColor, 1);
}