# Reflect

Minimal, single header file library for aggregate struct / class reflection in C++ 11. Limited STL usage, no other dependencies.

-----
## Installation
- Copy 'reflect.h' to project
- In ONE cpp file, define REGISTER_REFLECTION:
```cpp
    #define REGISTER_REFLECTION
    #include "reflect.h"
    #include "my_struct_1.h"
    #include "my_struct_2.h"
    #include etc...
```
- Classes / structs should be simple aggregate structs (standard layout)
     - No private or protected non-static data members
     - No user-declared / user-provided constructors 
     - No virtual member functions
     - No default member initializers (invalid in C++11, okay in C++14 and higher)
- BEFORE using reflection functions, initiate call to 
```cpp
    InitializeReflection();
```


-----
## Usage
### Registration in Header file, ex: "transform.h"
```cpp
#include "reflect.h"
struct Transform {
    int width;        
    int height;
    std::vector<double> position;
    std::string text;
    REFLECT();
}
#ifdef REGISTER_REFLECTION
    REFLECT_STRUCT(Transform)
    REFLECT_MEMBER(width)
    REFLECT_MEMBER(height)
    REFLECT_MEMBER(position)
    REFLECT_END(Transform)
#endif
```

-----
## In Code
### Example initialize class instance
```cpp
    Transform t { };
        t.width =    100;
        t.height =   100;
        t.position = std::vector<double>({1.0, 2.0, 3.0});
        t.text = "Hello world!";
```
### Meta Data
```cpp
    // Component Meta Data
    GetComponentData<Transform>().property_count;    // By class 
    GetComponentData(t).property_count               // By instance
    GetComponentData("Transform").property_count;    // By name

    // Property Meta Data
    GetPropertyData<Transform>("width").type;        // By class, property name
    GetPropertyData<Transform>(0).description;       // By class, property index
    GetPropertyData(t, "position").description;      // By instance, property name 
    GetPropertyData(t, 2).name;                      // By instance, property index
```

### Get / Set Properties
```cpp
    // GetProperty by Index
    int width = GetProperty<int>(t, 0);

    // GetProperty by Name
    std::vector<double> position = GetProperty<std::vector<double>>(t, "position");

    // SetProperty by Index
    int new_width = 50;
    SetProperty(t, 0, new_width);

    // SetProperty by Name
    std::vector<double> new_position = { 56.0, 58.5, 60.2 };
    SetProperty(t, "position", new_position);
```

-----
## Iterating Properties
- Member variable type can be specified in your class header file using macro function MEMBER_META_TYPE(Property_Type) ... This enum can easily be exapnded...
```cpp
    using vec = std::vector<double>;
    int property_count = GetComponentData("Transform").property_count;
    for (int p = 0; p < property_count; ++p) {
        std::cout << " Property Number: " << p << ", ";
        std::cout << " Name: " << GetPropertyData(t, p).name << ",";
        std::cout << " Value: ";
        switch (GetPropertyData(t, p).type) {
            case PROPERTY_TYPE_INT:             
                std::cout << GetProperty<int>(t, p);
                break;
            case PROPERTY_TYPE_VECTOR_DOUBLE:   
                std::cout << GetProperty<vec>(t, p)[0];
                break;
            case PROPERTY_TYPE_STRING:
                std::cout << GetProperty<std::string>(t, p);
                break;
            default: ;
        }
        std::cout << std::endl;
    }
```

-----
## Portability

Tested and compiled with:
- GCC 4.2.1
- Emscripten 1.38.43

-----
## License

Reflect is released under the terms of the MIT license, so it is free to use in your free or commercial projects.

Copyright (c) 2021 Stephens Nunnally and Scidian Software

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.