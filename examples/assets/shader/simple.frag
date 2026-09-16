#version 460 core

layout (location = 0) in vec2 f_tex;
layout (location = 1) in vec3 f_normal;
layout (location = 2) in vec3 f_pos;

layout (location = 0) out vec4 FragColor;

struct PointLightData {
    float x;
    float y;
    float z;
    float radius;
    uint color;
    float intensity;
    float fallof_linear;
    float fallof_quadratic;
    float cullDistance;
};

layout (set = 0, binding = 2) readonly buffer buffer_pointLights {PointLightData lights[];} pointLights;

struct SpotLightData {
    float x;
    float y;
    float z;
    float dir_x;
    float dir_y;
    float dir_z;
    uint color;
    float intensity;
    float fallof_linear;
    float fallof_quadratic;
    float cos_cone_inner;
    float cos_cone_outer;
    float cullDistance;
};

layout (set = 0, binding = 3) readonly buffer buffer_spotLights {SpotLightData lights[];} spotLights;

struct DirectionalLightData {
    float dir_x;
    float dir_y;
    float dir_z;
    uint color;
    float intensity;
};

layout (set = 0, binding = 4) readonly buffer buffer_directionalLights {DirectionalLightData lights[];} directionalLights;

vec3 pointLightColor(vec3 camDir, vec3 camPos, vec3 fragPos, vec3 fragNormal) {
    //store the light sum
    vec3 total = vec3(0);
    //iterate over all point lights
    int lightCount = pointLights.lights.length();
    for (int i = 0; i < lightCount; ++i) {
        //get the light distance
        float dist = distance(fragPos, vec3(pointLights.lights[i].x, pointLights.lights[i].y, pointLights.lights[i].z));
        //potentially cull
        if (dist >= pointLights.lights[i].cullDistance) {continue;}
        //get the light direction
        vec3 lightDir = normalize(vec3(pointLights.lights[i].x, pointLights.lights[i].y, pointLights.lights[i].z) - fragPos);
        //compute the light intensity
        //get the light color
        const float normalizer = 1.f / 255.f;
        vec3 lightCol = vec3((pointLights.lights[i].color & (0xFFu<<24))>>24, (pointLights.lights[i].color & (0xFFu<<16))>>16, (pointLights.lights[i].color & (0xFFu<<8))>>8) * normalizer;
        //compute the intensity
        float intensity = pointLights.lights[i].intensity / (pointLights.lights[i].fallof_linear * dist + pointLights.lights[i].fallof_quadratic * dist*dist);
        float lambertian = clamp(dot(fragNormal, lightDir), 0.f, 1.f);
        //add to the total
        total += lightCol*intensity*lambertian;
    }
    //return the sum
    return total;
}

vec3 spotLightColor(vec3 camDir, vec3 camPos, vec3 fragPos, vec3 fragNormal) {
    //store the light sum
    vec3 total = vec3(0);
    //iterate over all point lights
    int lightCount = spotLights.lights.length();
    for (int i = 0; i < lightCount; ++i) {
        //get the light distance
        float dist = distance(fragPos, vec3(spotLights.lights[i].x, spotLights.lights[i].y, spotLights.lights[i].z));
        //potentially cull
        if (dist >= spotLights.lights[i].cullDistance) {continue;}
        //get the light direction
        vec3 lightDir = normalize(vec3(spotLights.lights[i].x, spotLights.lights[i].y, spotLights.lights[i].z) - fragPos);
        //get the light color
        const float normalizer = 1.f / 255.f;
        vec3 lightCol = vec3((spotLights.lights[i].color & (0xFFu<<24))>>24, (spotLights.lights[i].color & (0xFFu<<16))>>16, (spotLights.lights[i].color & (0xFFu<<8))>>8) * normalizer;

        //compute the light intensity for the distance (same as the point light)
        float intensity_distance = spotLights.lights[i].intensity / (spotLights.lights[i].fallof_linear * dist + spotLights.lights[i].fallof_quadratic * dist*dist);
        //compute the light intensity using the angle
        float theta = dot(-vec3(spotLights.lights[i].dir_x, spotLights.lights[i].dir_y, spotLights.lights[i].dir_z), lightDir);
        float intensity_angle = clamp((theta - spotLights.lights[i].cos_cone_outer) / (spotLights.lights[i].cos_cone_inner - spotLights.lights[i].cos_cone_outer), 0.f, 1.f);
        //compute the lambertian
        float lambertian = clamp(dot(fragNormal, lightDir), 0.f, 1.f);
        //sum to the total
        total += lightCol * intensity_distance * intensity_angle * lambertian;
    }
    //return the sum
    return total;
}

vec3 directionalLightColor(vec3 camDir, vec3 camPos, vec3 fragPos, vec3 fragNormal) {
    //store the light sum
    vec3 total = vec3(0);
    //iterate over all point lights
    int lightCount = directionalLights.lights.length();
    for (int i = 0; i < lightCount; ++i) {
        //compute the incoming direction
        vec3 dir = vec3(directionalLights.lights[i].dir_x, directionalLights.lights[i].dir_y, directionalLights.lights[i].dir_z);
        float intensity = directionalLights.lights[i].intensity;
        float lambertian = clamp(dot(fragNormal, dir), 0.f, 1.f);
        //get the light color
        const float normalizer = 1.f / 255.f;
        vec3 lightCol = vec3((directionalLights.lights[i].color & (0xFFu<<24))>>24, (directionalLights.lights[i].color & (0xFFu<<16))>>16, (directionalLights.lights[i].color & (0xFFu<<8))>>8) * normalizer;
        //add to the light total
        total += lightCol * intensity * lambertian;
    }
    //return the sum
    return total;
}

vec3 getLightColor(vec3 camDir, vec3 camPos, vec3 fragPos, vec3 fragNormal) 
{return pointLightColor(camDir, camPos, fragPos, fragNormal) + spotLightColor(camDir, camPos, fragPos, fragNormal) + directionalLightColor(camDir, camPos, fragPos, fragNormal);}

const vec3 baseColor = vec3(238, 69, 61)/vec3(255);

void main() {
    vec3 lightColor = getLightColor(vec3(0), vec3(0), f_pos, f_normal);
    FragColor = vec4(baseColor * lightColor, 1);
}