/**
 * The .gs ending specifies a shading language implemented by GLGE. It is a wrapper around GLSL. It adds
 * utilities that are converted to raw GLSL code by the pre-compiler and converter implemented by GLGE. 
 * 
 * Pre-Processor directives: 
 * like in C, a pre-processor directive starts with a '#'. A list of pre-processor directives is shown below:
 * - #include
 *   The include directive has two forms: It can include a package or it can include another file. 
 *  -Packages:
 *       A package is a block of code that is treated like a library. A package must first be loaded into the shader 
 *       compilation unit to be found by the linking unit. To specify the linking of a package, use < >
 *       An example package inclusion could look like this:
 *       #include <test>
 *       This will include the package named test if it is loaded into the shader compiler. 
 *  -Files:
 *       A file is included if another shader source file is loaded into the source code. The included shader 
 *       source code will be stripped from the following:
 *           - the version identifyer
 *           - all entry point declarations
 *       The path to a file inclusion is relative to this file. To include a file, use ' ' or " ". 
 *       An example file inclusion could look like this:
 *       #include "../dir/test.gs"
 *       This will include the file named test.gs that is located in the directory dir that lies one directory above 
 *       this file's locatoin
 *
 * Defines set by GLGE:
 *  - GLGE_SHADER_TYPE
 *      A unsigned integer that defines the 
 */

//specify the GLGE version. The GLSL version will be translated to the newest version the graphic API supports. 
#version GLGE 0.1

//include the test package and all contained elements
#include <test>

vec3 verts[] = vec3[](
    vec3(1,1,0),
    vec3(0,1,0),
    vec3(0,1,0)
);

// say that the next function is a main function in the stage specified in the brackets. Here, it is in the vertex shader stage. 
@main(vertex)
void mainVert()
{
    //simply select the correct vertex for the index
    gl_Position = vec4(verts[gl_VertexID], 1);
}

#define GLGE_SHADER_TYPE 0

#if (GLGE_SHADER_TYPE == 1)
out vec4 FragColor;

@main(fragment)
void mainFrag()
{
    //simply write red
    FragColor = vec4(1,0,0,1);
}
#endif