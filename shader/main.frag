#version 460 core

#extension GL_EXT_nonuniform_qualifier : require

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 f_tex;

layout (set = 0, binding = 0) readonly buffer MaterialData{
    uint texs[];
};

layout (set = 1, binding = 0) uniform sampler2D textures[];

layout (location = 1) flat in uint v_submeshIndex;

layout (push_constant) uniform constants {
    uint submeshIndex;
    uint materialIndex;
    uint objectIndex;
};

void main()
{
    FragColor = texture(textures[texs[materialIndex]], f_tex);
}