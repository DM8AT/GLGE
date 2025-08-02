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
# Storing assets
