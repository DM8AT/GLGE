#version 460 core

layout (location = 0) in vec2 Position;
layout (location = 1) in vec2 UV;
layout (location = 2) in vec4 Color;

layout (binding = 0, std430) readonly buffer s_projMtxBuff {
    mat4 projMtx[];
} projMtx;

layout (location = 0) out vec2 Frag_UV;
layout (location = 1) out vec4 Frag_Color;

void main() {
    Frag_UV = UV;
    Frag_Color = Color;
    gl_Position = projMtx.projMtx[gl_BaseInstance] * vec4(Position.xy,0,1);
}