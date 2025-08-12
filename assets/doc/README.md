# What is an asset?

## High level overwiev
An asset in GLGE refears to some sort of file that sotres information about a GLGE class. It can be loaded to fill the class with the content. After loading the class can be accessed in the program. Not all assets are always available. Some assets are bound to specific cores (like the graphic core) and are only accessable if that core is included. Else, the enum identifyer will still exist, but be unused. 

## Internal structure
Internally, an asset is made up of a path to a file, a union that stores the data of the asset and an enum to identify what type of asset is stored. The data can hold any sub-class of the asset. The prefix for an asset sub-class is always "Asset" followed by the type of the asset. The sub-classes do NOT inherite from the asset class, since they are needed to create the asset class itself, making it impossible for the subclass to derive from the asset class. 

## Asset trees
One asset can contain multiple other assets when making use of compund assets. A single compound assets contains a mapping from names to other assets, making it possible to create asset trees. This is used to load multiple assets at once or if multiple asset types are compressed into a single file (as for example an .fbx file may contain textures AND model data). 
```
   File.fbx
      |
      V
Compound Asset
      |-------> Texture Asset
      |
      |-------> Material Asset
      |
      |-------> Render Mesh
```
An asset tree is not limited to one compound asset. A single compound asset can contain an unlimited amound of other compound assets. 

## Accessing data created during runtime in assets
Assets can access other assets as well as data that is created during runtime. To access an element that was created during runtime, all that needs to be knwon is the type and name of the element. The name is the name passed to the instance during the object's creation. When the asset is loaded, the element will be pulled from the instance. If the same name is registerd to the instance twice, the first element registerd with that name will be used. If the element can't be found on the instance or the type dosn't match the expected type, an error will be logged and the asset won't be able to access the element. Further handeling depends on what exactly the asset is and what the data to access was. 

Here is an example where a buffer for a shader is created during runtime and the shader is loaded from an asset:
C++ source code:
```cpp
//create the buffer. Take note here that the name is "Data Buffer". This name will be passed as-is to the instance. 
Buffer buff("Data Buffer", MEMORY_USAGE_UNIFORM, inst);
```
Asset file:
```xml
<!-- This would be the line in the shader that maps the buffer. Take note here that the name parameter is NOT what determines the buffer to load, but the buffer paramter matches with the buffer's name -->
<buffer name="Some data", binding="0", buffer="Data Buffer">
```

## Asset names
All assets that can be loaded directly require names. All names within an asset storage must be unique, else the loading of the asset failes. Names may not contain a `/` character. If the name contains this character, it will be replaced by `%`. All UTF-8 characters except for `/` and `\0` (a null byte) are valid characters for the name of an asset. The `/` seperator character and the `%` replace character can be changed by changing the values in the defines `GLGE_ASSET_SUB_ASSET_SEPERATOR` and `GLGE_REPLACE_SUB_ASSET_SEPERATOR_WITH` with custom characters. After that GLGE must be re-compiled for the changes to take effect. 

# Storing assets
Assets can be stored as files. A stored asset can be encoded in different ways. Here is a table with all ways to store assets, the version the loader was updated the last time and what version they where introduced in:
| File Type | Introduced in | Latest update |
|-----------|---------------|---------------|
| XML       | 0.1.0         | 0.1.0         |

## XML files
Assets can be stored as xml files. The type of stored asset is determined by the names of the children of the root node, NOT by the file suffix. The file sufficies used in the examples are compleatly arbituary. The XML file should be stored in UTF-8 and without compression. The file can contain comments. Because it is human readable and can contain comments, XML files are good for developement enviroments. 

When an asset is stored in an XML file the structure may contain an attribute called `file`. File contains a path relative to the root of the project that points to a different file that contains the data for the asset. As an alternative the asset can be stored directly in place. For larger assets this is not recomended as it may lead to data duplication. 
Here is an example on how the reference to another file works:
```xml
<!-- Some larger data structure contains a material -->
<!-- But instead of actualy containing the definition of the material, a file attribute points to a file 
     called "someFile.mat" that holds the definition for the material -->
<material file="someFile.mat"> </material>
```
someFile.mat:
```xml
<!-- Here, the material is actually defined -->
<material>
      <!-- definition of the material element -->
</material>
```

### Structure: GLGE header
All asset files must include an element named "GLGE". This element stores important information like the version of GLGE with wich the asset was saved to make backwards and forwards compatability possible and warn the user if some verison is so outdated or experimental that it may no longer be possible to load correctly. This element can also contain text. If it does so, the text is interpreted as a comment. During loading the comment is printed to the console. Artists and programs may use the comment for credit. 

All attributes for the GLGE element:
| Name | Type | Optional | Since | Usage |
|------|------|----------|-------|-------|
| version | String with format `"%d.%d.%d"` | :x: | 0.1.0 | This element stores the version the asset was stored in |

### Structure: Materials
A material asset stores settings for the RenderMaterial class. It can only be loaded if the graphic core is compiled into GLGE, else the loading will fail. The asset itself depends on the shader and vertex layout asset. 

The material asset contains 3 sub-elements:
 - a shader element
 - a vertex layout element
 - a settings element
 
The settings has a few attributes that must be set for the material to be valid
| Name | Type | Optional | Since | Usage |
|------|------|----------|-------|-------|
| culling | Bool | :x: | 0.1.0 | True: Back faces of meshes renderd with this material will be culled, else they will be shwon |
| depthTest | Enum | :x: | 0.1.0 | The value must a string contianing an enum value of the DepthTest Enum. They dictate the operation to use for depth test |
| depthWrite | Bool | :x: | 0.1.0 | True: the depth values of renderd meshes will be stored in a depth buffer (if one exists), else the depth value will be discarded |