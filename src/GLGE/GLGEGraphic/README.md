# GLGE Graphic
This file gives an overview on the GLGE Graphic package. The package belongs to the default packages that are included in GLGE. 

## What is the GLGE Graphic package
The Graphic package abstracts the functionality of low level graphic APIs like Open GL and Vulkan to unite the working of them under one API. The Graphic package uses the abstracted API instead of revealing it directly. The abstracted graphic API is used to further abstract working with graphics. This is done to simplify the working so no manual command list need to be filled. 

## Contents
The Graphic package contains a list of classes, enums and structures that manage the graphics. Following is a table of all classes and structures with a description of the purpose. 
| Name | Type | Description | 
|-|-|-|
| ColorSpace | Enum | This enum defines values to use for different color spaces. It also defines where the alpha channel for each color space is stored |
| Color | Class | This class is used to change the color spaces of colors and to shift the position of the alpha channel |
| Framebuffer | Class | This class contains one ore more Textures that can be renderd to. These Textures can be filled with texture or depth data | 
| StageType | Enum | This enum specifies the type of a render stage |
| RenderStage | Struct | This structure stores all the information for a single stage of the Render Pipeline |
| RenderPipeline | Class | This class manages the sequential execution of different rendering stages |
| Texture | Class | This class stores a 2D array of color or depth values. The type of the stored colors is defined by the texture's purpose |
| WindowSettings | Struct | This structure stores a list of boolean values that define the settings for a window |
| Window | Class | This class stores the context of a window that is shown to the end user |

For C usage, there exist also structs for all classes. The structs will be prefixed with a customizable prefix. Default value for the prfix ist "c_". 

## Working of the Graphic package
The graphic