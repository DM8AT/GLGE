# What is an asset?
## High level overwiev
An asset in GLGE refears to some sort of file that sotres information about a GLGE class. It can be loaded to fill the class with the content. After loading the class can be accessed in the program. Not all assets are always available. Some assets are bound to specific cores (like the graphic core) and are only accessable if that core is included. Else, the enum identifyer will still exist, but be unused. 
## Internal structure
Internally, an asset is made up of a path to a file, a union that stores the data of the asset and an enum to identify what type of asset is stored. The data can hold any sub-class of the asset. The prefix for an asset sub-class is always "Asset" followed by the type of the asset. The sub-classes do NOT inherite from the asset class, since they are needed to create the asset class itself, making it impossible for the subclass to derive from the asset class. 
# Asset trees
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
# Accessing data created during runtime in assets
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
# Storing assets
