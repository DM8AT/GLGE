/**
 * The ending .gp specifies that the file is a package file. A package is similar to a library in C. It contains 
 * functions, structurs, defines and constants, but no actual entry point. All rules for the GLGE wrapper of 
 * GLSL still apply for packages. A package can be included using the #include statement. 
 * A package is parsed to a shader compiler and can only afterwards be used in inclusion. 
 * 
 * A package is like a header in C. The content is simply pasted into the source, then the pre-processor is run again. 
 * The converter is only run on the output of the fully pre-processed code. 
 */

//header guards can be done like in C. 
#ifndef _TEST_
#define _TEST_

/**
 * structs are like structs in C. They can not contain functions. The purpose is to structure memory. 
 */
struct Vertex
{
    vec3 pos;
    vec2 tex;
    vec3 norm;
};

/**
 * Create an input buffer for vertices. 
 * In contrast to GLSL, a buffer does not need a binding location. These are managed by GLGE under the hood, but they can be overriden. 
 * A layout specifier is optional. If none is provided, std140 is assumed. If nether is used, layoud and braces can be removed. 
 * 
 * A buffer can specify if it is in, out or inout. If none is specified, it is assumed to be inout. Specifying the input or output 
 * status can be helpful for the driver so it can optimize the data in or output for the data. 
 * The buffer keyword is mapped to shader storage buffers. 
 * A buffer has a name with wich it can be accessed in GLGE and also in the shader. 
 * In contrast to vanilla GLSL, if a buffer has only one element the content dosn't need to be wrapped in curly braces. 
 * 
 * The simpelest buffer would look something like this:
 * buffer int name;
 * This buffer is a single integer with the name "name". The data of the buffer can be written using GLGE as well as the shader. 
 * 
 * The buffer below is a dynamicaly sized array of Vertices. It can be used like a vertex buffer. It is set by GLGE and can not be 
 * written to by the shader. 
 * 
 * WARNING: Currently not implemented
 */
//in buffer Vertex glge_Vertices[];

layout (std430, binding = 0) readonly buffer glge_VertexBuffer
{
    Vertex glge_Vertices[];
};

/**
 * The buffer below is a dynamicaly sized array of Indices. It can be used like a index buffer. It is set by GLGE and can not be
 * written to by the shader. 
 *
 * WARNING: Currently not implemented
 */
//in buffer int glge_Indices[];

#endif // end of header guard