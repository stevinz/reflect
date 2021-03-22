# Reflect

Small, flexible, single header library for aggregate (struct / class) runtime reflection and meta data using C++ 11 features. Minimal STL usage, no other dependencies.

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

- Classes / Structs should be simple aggregate types (standard layout)
    - No private or protected non-static data members
    - No user-declared / user-provided constructors 
    - No virtual member functions
    - No default member initializers (invalid in C++11, okay in C++14 and higher)
    - See (https://en.cppreference.com/w/cpp/types/is_standard_layout) for more info 

- BEFORE using reflection functions, make one call to 
```cpp
    InitializeReflection();
```


-----
## Usage
### Registration in Header File, example: "transform.h"
```cpp
    #ifndef TRANSFORM2D_H
    #define TRANSFORM2D_H

    #include "reflect.h"
    struct Transform2D {
        int width;        
        int height;
        std::vector<double> position;
        std::string text;
        REFLECT();
    }
    #ifdef REGISTER_REFLECTION
        REFLECT_CLASS(Transform2D)
        REFLECT_MEMBER(width)
        REFLECT_MEMBER(height)
        REFLECT_MEMBER(position)
        REFLECT_END(Transform2D)
    #endif

    #endif // TRANSFORM2D_H
```

-----
## In Code
### Initialize class instance
```cpp
    Transform2D t { };
        t.width = 100;
        t.height = 100;
        t.position = std::vector<double>({1.0, 2.0, 3.0});
        t.text = "Hello world!";
```

### Type Data Object
```cpp
    // Class Type Data
    GetClassData<Transform2D>().member_count;   // By class type
    GetClassData(t).member_count;               // By class instance
    GetClassData(hash_id).member_count;         // By class hash id
    GetClassData("Transform2D").member_count;   // By class name

    // Member Type Data
    GetMemberData<Transform2D>(0).title;        // By class type, member index
    GetMemberData<Transform2D>("width").title;  // By class type, member name
    GetMemberData(t, 0).title;                  // By class instance, member index    
    GetMemberData(t, "width").title;            // By class instance, member name 
    GetMemberData(hash_id, 0).title;            // By class hash id, member index
    GetMemberData(hash_id, "width").title;      // By class hash id, member name 
```

### Get / Set Member Variables
- Before calling GetValue<>(), member variable type can be checked by comparing to predefined constants. These can easily be added to in 'reflect.h'...
```cpp
    // GetValue by Index
    HashID member_type = GetMemberData(t, 0).hash_code;
    if (member_type == MEMBER_TYPE_INT) {
        int width = GetValue<int>(t, 0);
    }

    // GetValue by Name
    HashID member_type = GetMemberData(t, "position").hash_code;
    if (member_type == MEMBER_TYPE_VECTOR_DOUBLE) {
        std::vector<double> position = GetValue<std::vector<double>>(t, "position");
    }

    // SetValue by Index
    HashID member_type = GetMemberData(t, 0).hash_code;
    if (member_type == MEMBER_TYPE_INT) {
        int new_width = 50;
        SetValue(t, 0, new_width);
    }

    // SetValue by Name
    HashID member_type = GetMemberData(t, "position").hash_code;
    if (member_type == MEMBER_TYPE_VECTOR_DOUBLE) {
        std::vector<double> new_position = { 56.0, 58.5, 60.2 };
        SetValue(t, "position", new_position);
    }
```

-----
## Iterating Members / Properties
```cpp
    using vec = std::vector<double>;
    int member_count = GetClassData("Transform2D").member_count;
    for (int prop = 0; prop < member_count; ++prop) {
        std::cout << " Member Index: " << prop << ", ";
        std::cout << " Name: " << GetMemberData(t, prop).name << ",";
        std::cout << " Value: ";
        HashID member_type = GetMemberData(t, prop).hash_code;
        if (member_type == MEMBER_TYPE_INT) {
            std::cout << GetValue<int>(t, prop);
        } else if (member_type == MEMBER_TYPE_VECTOR_DOUBLE) {
            std::cout << GetValue<vec>(t, prop)[0];
        } else if (member_type == MEMBER_TYPE_STRING) {
            std::cout << GetValue<std::string>(t, prop);
        }
        std::cout << std::endl;
    }
```

-----
## Data from Unknown Class Type
- If using with an entity component system, it's possible you may not have access to class type at runtime. Often a collection of components are stored in a container of void pointers. Somewhere in your code when your class is initialized, store the component class '.hash_code':
```cpp
    HashID saved_hash = GetClassData(t).hash_code;
    void* class_pointer = (void*)(&t);
```  
- Later (if your components are stored as void pointers in an array / vector / etc. with other components) you may still access the member variables of the component without casting the component back to the original type. This is done by using the saved_hash from earlier:
```cpp
    using vec = std::vector<double>;
    HashID member_type = GetMemberData(saved_hash, 3).hash_code;
    if (member_type == MEMBER_TYPE_VECTOR_DOUBLE) {
        vec rotation = GetValue<vec>(class_pointer, saved_hash, 3);
        std::cout << "  Rotation X: " << rotation[0];
        std::cout << ", Rotation Y: " << rotation[1];
        std::cout << ", Rotation Z: " << rotation[2];
    }
```

-----
## User Meta Data
### Registration in Header File, example: "transform.h"
- Meta data can by stored as std::string within a class or member type. Set user meta data at compile time using CLASS_META_DATA and MEMBER_META_DATA in the class header file with (int, string) or (sting, string) pairs:
```cpp
    #ifdef REGISTER_REFLECTION
        REFLECT_CLASS(Transform2D)
            CLASS_META_DATA(META_DATA_DESCRIPTION, "Describes object in 2D space.")
            CLASS_META_DATA("icon", "assets/transform.png")
        REFLECT_MEMBER(width)
            MEMBER_META_DATA(META_DATA_DESCRIPTION, "Width of this object.")
        REFLECT_MEMBER(height)
            MEMBER_META_DATA(META_DATA_DESCRIPTION, "Height of this object.")
        REFLECT_MEMBER(position)
            MEMBER_META_DATA(META_DATA_HIDDEN, "true")
            MEMBER_META_DATA("tooltip", "Pos")
        REFLECT_END(Transform2D)
    #endif
```

### Get / Set Meta Data
- By reference, pass a ClassData or MemberData type data object (this can be retrieved many different ways as shown earlier) to the meta data functions to get / set meta data at runtime:
```cpp
    // Get class meta data
    ClassData& class_data = GetClassData<Transform2D>();
    std::string description = GetClassMeta(class_data, META_DATA_DESCRIPTION);
    std::string icon_file   = GetClassMeta(class_data, "icon");

    // Set class meta data
    SetClassMeta(class_data, META_DATA_DESCRIPTION, description);
    SetClassMeta(class_data, "icon", icon_file);


    // Get member meta data
    MemberData& member_data = GetMemberData<Transform2D>("position");
    std::string description = GetMemberMeta(member_data, META_DATA_DESCRIPTION);
    
    // Set member meta data
    SetMemberMeta(member_data, META_DATA_DESCRIPTION, description);
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