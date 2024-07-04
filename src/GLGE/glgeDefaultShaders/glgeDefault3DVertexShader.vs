#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 vColor;
layout (location = 2) in vec2 vTexcoord;
layout (location = 3) in vec3 vNormal;

#include <glgeObject>
#include <glgeCamera>

out vec4 color;
out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;
out vec3 vPos;

void main()
{
    color = vColor;
    texCoord = vTexcoord;
    fragPos = (vec4(pos, 1) * glgeModelMat).xyz;
    normal = normalize(vec4(vNormal, 1) * glgeRotMat).xyz;
    gl_Position = vec4(fragPos,1)*glgeCamMat;
    vPos = pos;
}