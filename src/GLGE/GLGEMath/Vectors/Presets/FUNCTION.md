# What is the purpose of the templates
The templates are files that containt the definition and implementation for a generic vector. The amount of elements in the vector can be read from the file name. The files can be seen like functions. When they are included, they add a new vector type. To create the vector correctly, 3 defines must exist: 
- `GLGE_VEC_TYPE`: This defines the element type of the vector to create. It must be a default C type that supports addition, subtractoin, multiplication and division. The type may be unsigned. 
- `GLGE_VEC_NAME`: This will be the final name of the new vector type. It can be set to anything as long as it would be a valid C type name. 
- `GLGE_VEC_STRUCT_NAME`: The name for the struct of the vector. It must be a valid C struct name. It is a good common to prefix intermediate structs with something. All GLGE intermediate structs are prefixed with `s_`. 
After the file all of the set defines will be undefined. 

# Error Handeling
When one of the required defines is not set, a compile time error will occour describing the problem. The whole creation of the structure will be skipped. The defines will not be undefined. 

# Output
The file will set up the following: A C struct containing the amount of elements as described by the file name. Each element will be by the type defined by `GLGE_VEC_TYPE`. If the compilation is done using C++ the following functions will be defined:

| Operator | Function   | Inputs                 | Return                           | Purpose |
|----------|------------|------------------------|----------------------------------|---------|
| +        | operator+  | two of the new vectors | a new instance of the new vector | this overload adds two vectors together and returns the sum of the vectors |
| -        | operator-  | two of the new vectors | a new instance of the new vector | this overload subtracts the second input vector from the first input vector and returns the difference in a new instance of the new vector type |
| *        | operator*  | two of the new vectors | a new instance of the new vector | this overload multiplies the two input vectors per element together and returns the per-element product in a new vector instance |
| /        | operator/  | two of the new vectors | a new instance of the new vector | this overload divides the two input vectors where the first input vector is used as the nominator and the second as the denominator. The per-element fractions are returned in a new instance of the new vector type |
| -        | operator-  | one of the new vectors | a new instance of the new vector | this overload negates the inputed vector per element and returns the negated vector as a new instance of the new vector type |
| +=       | operator+= | two of the new vectors | no return                        | this overload adds the second inputed vector to the first input and stores the result in the first input |
| -=       | operator-= | two of the new vectors | no return                        | this overload subtracts the second inputed vector from the first input and stores the result in the first input |
| *=       | operator*= | two of the new vectors | no return                        | this overload multiplies the two vectors per element together and stores the resulting per element products in the first inputed vector |
| /=       | operator/= | two of the new vectors | no return                        | this overload divides the two tevectors per element. The first input is used as the nominator and the second input is used as the denominator. The resulting per element fractions are stored in the first inputed vector |
| NONE     | toString() | one of the new vectors | a new instance of std::string    | this function converts the contents of the vector into a string following the GLGE stringify structure |

Regardless of C or C++ some functions for working with the vector type will be defined. For C++ those functions often use the C++ specific functions behind the scenes, so directly using them would be better. But when writing code for C or C and C++, those functions can be used. When compiling they addapt to the used language. All functions are prefixed with the name of the vector because parameter overloading does not work in C. 

| Function Suffix | Inputs | Return | Purpose |
|-----------------|--------|--------|---------|
| _create         | as many elements of the type specified by the file name of the vector element's type | a new instance of the vector type | this function can be used to create a new vector regardless of C or C++ enviroment. When using C++ it won't raise a warning about compound expressions because it will use the struct's constructor |
| _createSingle   | a single element of the type of the vector's elements | a new instance of the vector type | this function creates a new vector where all elements will be set to the inputed value regardless of C or C++ enviroment. When using C++ it won't raise a warning about compound expressions because it will use the struct's constructor |
| _add            | two instances of the vector type | a new instance of the vector type | this function adds the two inputed vectors per element and returns the sums as a new instance of the vector type |
| _subtract       | two instances of the vector type | a new instance of the vector type | this function subtracts the two inputed vectors per element and returns the difference as a new instance of the vector type. The second input will be subtracted from the first input |
| _multiply       | two instances of the vector type | a new instance of the vector type | this function multiplies the two inputed vectors per element and returns the per element product in a new vector instance |
| _divide         | two instances of the vector type | a new instance of the vector type | this function divides the two inputed vectors per element and returns the per element fraction in a new vector instance. The first inputed vector will be used as the nominator and the second inputed vector will be used as the denominator |
| _addTo          | one pointer to a vector and one vector instance | no return | this function adds the second vector to the first inputed vector |
| _subtractFrom   | one pointer to a vector and one vector instance | no return | this function subtracts the second vector from the first inputed vector and stores the resulting difference in the first inputed vector |
| _multiplyTo     | one pointer to a vector and one vector instance | no return | this function multiplies the two vectors per element together and stores the resulting product in the elements of the first vector |
| _divideFrom     | one pointer to a vector and one vector instance | no return | this function divides the two vectors per element and stores the resulting per element fractions in the first inputed vector. The first inputed vector will be used as the nominator and the second inputed vector will be used as the denominator |
| _asString       | one instance of the vector type | a `const char` pointer as a C-Style string allocated on the heap | this function converts the contents of a vector type into a string according to the GLGE stringify structure |
| _negate         | one instance of the vector type | a new instance of the vector type | this function reverses the sign of all the elements of the inputed vector and returns the result as a new vector instance |
| _negateThis     | a pointer to an instance of the vector type | no return | this function negates the signs of all the elements in the vector and stores the result in the same vector | 
| _dot            | two instances of the vector type | one element of the same type as the vector's elements | this function computes the dot product of the two inputed vectors and returns the result |

Only 3D vectors support the following function:
| Function Suffix | Inputs | Return | Purpose |
|-----------------|--------|--------|---------|
| _cross          | two instances of the vector type | a new instance of the vector type | this function computes the 3D cross product of the input vectors and returns the resulting vector as a new instance of the vector type |