//specify that this shader is for at least GLGE version 0.1.0
#version GLGE 0.1.0

//enable bindless textures
#extension GL_ARB_bindless_texture : require

//include the buffers to access textures
//this pre-processor directive is added by GLGE
#include <GLGETextures>

//specify how many threads are used per invocation per axis
//good are a lot, this helps to speed up rendering
//I tested on my GPU (RTX 5070 TI) with different counts for this shader:
// 1x1x1 instances per execution
//  --> 214 µs per execution
// 8x8x1 instances per execution
//  --> 12 µs per execution
// 32x32x1 instances per execution
//  --> 11.75 µs per execution
// => higher values are better, from my experience the difference is stronger on AMD than NVidia
//    no idea how the difference is on Intel GPUs
layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std140, binding = 0) uniform cmpInfo {
    int writeTo;
    int tex;
};

//entry point for the compute shader
@main(stage_compute)
void main()
{
    //read the size of the output image (image 0)
    ivec2 imgSize = imageSize(image2D(glge_Image[writeTo]));
    //compute the aspect ratio of the image to prevent warping
    float aspect = float(imgSize.x) / float(imgSize.y);

    //calculate the pixel position this invocation should work on
    ivec2 i = ivec2(gl_GlobalInvocationID.xy);

    //calculate the position of the shader invocation in a stretched NDC coordinate space. 
    //all values will range from -1 to 1 on x and y
    //but, what breaks the NDC space, the x axis is stretched by the aspect so x reaches from -aspect to +aspect
    vec2 pos = vec2(i) / vec2(imgSize);
    pos = pos * 2. - 1.;
    //scale by the aspect. This breaks the NDC space
    pos.x *= aspect;

    //calculate the distance of th pixel from the center
    float dist = distance(pos, vec2(0));
    
    //store the color to set
    vec4 col = (dist > 0.99) ? vec4(0) : texture(sampler2D(glge_Texture[tex]), pos.xy).rgba;

    //check if the value is inside the circle
    if (dist <= 1.)
    {
        //draw a color from the texture to the image
        imageStore(image2D(glge_Image[writeTo]), i, col);
        //imageStore(image2D(glge_Image[writeTo]), i, vec4(1,0,0,1));
    }
}