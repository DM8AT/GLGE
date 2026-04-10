#version 460 core

layout (location = 0) in vec2 f_tex;
layout (location = 1) in vec3 f_normal;

layout (location = 0) out vec4 FragColor;

const vec3 lightCol = vec3(1); // vec3(0.7,0.8,0.4);
const float lightStr = 3.f;
const vec3 lightDir = normalize(vec3(0.2,1,-0.3));

const vec3 color = vec3(238, 69, 61)/vec3(255);

void main() {
    float str = dot(lightDir, f_normal)*0.5 + 0.5;
    vec3 combColor = color * str * lightStr * lightCol;
    FragColor = vec4(combColor, 1);
}