<!-- This file contains an example for a shader asset -->

<!-- The GLGE component stores informatoin about the version of the loader that the file was saved with as well as an optional comment -->
<GLGE version="0.1.0">
    An example for a simple shader asset
</GLGE>

<!-- The shader component can contain a path to a file to load it from or the data for the shader itself -->
<shader name="Example Shader">

    <!-- The source argument contains the source code of the shader. If the file argument does not exist, the node itself may contain text as the source code. -->
    <source file="shader/default.gs"></source>

    <!-- Textures is a list of texture elements where each element is a texture -->
    <textures>
        <!-- A texture consists of a name that is used to access that element and a path to a file to load the texture from. In a later update it could be possible to have raw texture data in the texture element, but not as of writing this (GLGE_VERSION = 0.1.0) -->
        <texture name="cube", file="assets/textures/cubeTexture.png"></texture>
    </textures>

    <!-- Buffers contains a list of buffers the shader can access -->
    <buffers>
        <!-- A buffer consists of a name, a binding point and a string to identify what buffer to bind to the shader durring loading -->
        <buffer name="Textures", binding="0", buffer="GLGE_BUILDIN_INSTANCE_TEXTURES"></buffer>

    </buffers>

    <!-- The texture and buffer list are not complete. They may be extended in the code using user-defined buffers. -->

</shader>