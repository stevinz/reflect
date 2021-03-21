# Reflect

Small, flexible single header file library for aggregate (struct / class) runtime reflection using C++ 11 features. Minimal STL usage, no other dependencies.

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

- Classes / structs should be simple aggregate types (standard layout)
    - No private or protected non-static data members
    - No user-declared / user-provided constructors 
    - No virtual member functions
    - No default member initializers (invalid in C++11, okay in C++14 and higher)
    - See (https://en.cppreference.com/w/cpp/types/is_standard_layout) for more info 

- BEFORE using reflection functions, initiate call to 
```cpp
    InitializeReflection();
```


-----
## Usage
### Registration in Header File, example: "transform.h"
```cpp
    #ifndef TRANSFORM_H
    #define TRANSFORM_H

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

    #endif // TRANSFORM_H
```

-----
## In Code
### Initialize class instance
```cpp
    Transform t { };
        t.width = 100;
        t.height = 100;
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
    GetPropertyData<Transform>("width").hash_code;   // By class, property name
    GetPropertyData<Transform>(0).description;       // By class, property index
    GetPropertyData(t, "position").description;      // By instance, property name 
    GetPropertyData(t, 2).name;                      // By instance, property index
```

### Get / Set Properties
- Before calling GetProperty<>(), member variable type can be checked by comparing to predefined constants. These can easily be added to in 'reflect.h'...
```cpp
    // GetProperty by Index
    HashID type = GetPropertyData(t, 0).hash_code;
    if (type == PROPERTY_TYPE_INT) {
        int width = GetProperty<int>(t, 0);
    }

    // GetProperty by Name
    HashID type = GetPropertyData(t, "position").hash_code;
    if (type == PROPERTY_VECTOR_DOUBLE) {
        std::vector<double> position = GetProperty<std::vector<double>>(t, "position");
    }

    // SetProperty by Index
    int new_width = 50;
    SetProperty(t, 0, new_width);

    // SetProperty by Name
    std::vector<double> new_position = { 56.0, 58.5, 60.2 };
    SetProperty(t, "position", new_position);
```

-----
## Iterating Properties
```cpp
    using vec = std::vector<double>;
    int property_count = GetComponentData("Transform").property_count;
    for (int p = 0; p < property_count; ++p) {
        std::cout << " Property Number: " << p << ", ";
        std::cout << " Name: " << GetPropertyData(t, p).name << ",";
        std::cout << " Value: ";
        HashID type = GetPropertyData(t, p).hash_code;
        if (type == PROPERTY_TYPE_INT) {
            std::cout << GetProperty<int>(t, p);
        } else if (type == PROPERTY_TYPE_VECTOR_DOUBLE) {
            std::cout << GetProperty<vec>(t, p)[0];
        } else if (type == PROPERTY_TYPE_STRING) {
            std::cout << GetProperty<std::string>(t, p);
        }
        std::cout << std::endl;
    }
```

-----
## Data from Unknown Type
- If using with an entity component system, it's possible you may not have access to class type at runtime. Often a collection of components are stored in a container of void pointers. Somewhere in your code when your class is initialized, store the class typeid().hash_code:
```cpp
    HashID hash = GetComponentData(t).hash_code;
    void* component_ptr = (void*)(&t);
```  
- Later (if your components are stored as void * in an array / vector / etc. with other components) you may still access the member variables of the component without casting it back to the derived type. This is done by using the type hash code from earlier:
```cpp
    using vec = std::vector<double>;
    HashID value_type = GetPropertyData(hash, 3).hash_code;
    if (value_type == PROPERTY_TYPE_VECTOR_DOUBLE) {
        vec rotation = GetProperty<vec>(component_ptr, hash, 3);
        std::cout << "  Rotation X: " << rotation[0];
        std::cout << ", Rotation Y: " << rotation[1];
        std::cout << ", Rotation Z: " << rotation[2];
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