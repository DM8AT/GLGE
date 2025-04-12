# What is GLGE?
GLGE is a game engine written in C++ designed to design games in 3D and 2D enviroments. GLGE is able to run cross-platform and all GLGE scripts should also be able to run on any operating system as long as no additional operating system dependend librarys are used. GLGE is also able to run on all 64 bit devices that support OpenGL and C-99 and can perform floating point arithmatic. 

# Working with GLGE
## Including GLGE
When adding GLGE to a project, clone the source code into your project directory and include the file `GLGE/GLGE.h` into your project. This file includes all GLGE modules like mathematics, graphics, sound and physics. All modules will include all classes and sub-modules that belong to them in their respective headers. As example the math module in `GLGEMath` is included through the header file `GLGEMath/GLGEMath.h`, which intern includes all classes and sub-modules that belong to the math module like vectors. 

## Personalizing GLGE
Some aspects of GLGE are not fixed in the code. Some things can be customized by modifying the file `GLGE/GLGECore/GLGESettings.h`. The defines in this file can be changed to fit GLGE more to your coding style. This customization should be done carefully. The following customization can be done:

| Define Name | Safty Degree | Usage | Default |
|-------------|--------------|-------|---------|
| GLGE_VERSION | :x: | This defines the version of GLGE. DO NOT CHANGE UNLESS YOU KNOW WHAT YOUR DOING | Current newest: 0.1.0 |
| GLGE_CREATE_NAMESPACE | :check: | This dictates if GLGE should be contained in a namespace (the namespace will only be created if C++ is used) | 0 / False |
| GLGE_NAMESPACE | :white_check_mark: | This defines the name of the namespace that should be used if a namespace is going to be created | GLGE |
| GLGE_PREFIX_C_FUNCTIONS | :white_check_mark: | This says if functions should be prefixed if C is used | 0 / False |
| GLGE_PREFIX_ALWAYS | :white_check_mark: | This can be used to enable prefixing also in C++ | 0 / False |
| GLGE_PREFIX | :white_check_mark: | This is the prefix that will be set in fron all GLGE functions if prefixing is enabled | glge_ | 
| GLGE_COMPILER_ERRORS | :exclamation: | This defines if custom compile time errors will be printed | 1 / True |
| GLGE_COMPILER_WARNINGS | :exclamation: | This defines if custom compile time warnings will be printed | 1 / True |
| GLGE_INCLUDE_MATH | :warning: | This defines if the math module of GLGE should be included | 1 / True |
| GLGE_INCLUDE_GRAPHICS | :warning: | This defines if the graphic module of GLGE should be included | 1 / True |

Please keep in mind: Theses settings are compile-time and only apply if you build the library from source