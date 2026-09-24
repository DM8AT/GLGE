#version 460 core

layout (location = 0) in vec4 f_color;

layout (location = 0) out  vec4 FragColor;

void main() {
    FragColor = f_color;
}