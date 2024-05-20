#version 450 core

#define PI 3.14159265

precision highp float;

layout(location = 4) out vec4 FragColor;
layout(location = 6) out vec4 LightData;
in vec2 texCoords;

uniform sampler2D glgeAlbedoMap;
uniform sampler2D glgeNormalMap;
uniform sampler2D glgePositionMap;
uniform sampler2D glgeRoughnessMap;

layout (std140, binding = 1) uniform glgeCameraData
{
    mat4 glgeCamMat;
    mat4 glgeProjMat;
    mat4 glgeCamTransfMat;
    mat4 glgeCamRotMat;
    vec3 glgeCameraPos;
    vec3 glgeCameraRot;
    float glgeNear;
    float glgeFar;
    float glgeFov;
};

#include <glgeLightEvaluationFunction>

layout (std140, binding=3) uniform glgeLightData
{
    int glgeActiveLights;
    layout(offset = 16) glgeLight glgeLights[128];
};

void main()
{
    vec4 aMap = texture(glgeAlbedoMap,   texCoords);
    vec4 nMap = texture(glgeNormalMap,   texCoords);
    vec4 pMap = texture(glgePositionMap, texCoords);

    vec3 vPos = vec3(aMap.w,nMap.w,pMap.w);

    vec3 tmp = texture(glgeRoughnessMap, texCoords).rgb;

    if ((glgeActiveLights == 0) || (tmp.z == 0.f))
    {
        FragColor = vec4(aMap.rgb, 1);
        return;
    }

    glgePixel d = glgePixel(
        aMap.rgb,
        pMap.rgb,
        normalize(nMap.rgb),
        tmp.x, tmp.y
    );

    FragColor = vec4(0,0,0,1);

    for (int i = 0; i < glgeActiveLights; i++)
    {
        vec3 light = glgeEvaluateLight(glgeLights[i], d);
        FragColor.xyz += light;
    }
}