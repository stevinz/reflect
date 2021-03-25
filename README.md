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
        REFLECT_MEMBER(text)
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

### TypeData Object
```cpp
    // Class TypeData
    TypeData data = ClassData<Transform2D>();           // By class type
    TypeData data = ClassData(t);                       // By class instance
    TypeData data = ClassData(type_hash);               // By class type hash
    TypeData data = ClassData("Transform2D");           // By class name

    // Member TypeData
    TypeData data = MemberData<Transform2D>(0);         // By class type, member index
    TypeData data = MemberData<Transform2D>("width");   // By class type, member name
    TypeData data = MemberData(t, 0);                   // By class instance, member index
    TypeData data = MemberData(t, "width");             // By class instance, member name 
    TypeData data = MemberData(type_hash, 0);           // By class type hash, member index
    TypeData data = MemberData(type_hash, "width");     // By class type hash, member name 
```

### Get / Set Member Variables
- Use the ClassMember<member_type>(class_instance, member_data) function to return a reference to a member variable. This function requires the return type, a class instance (can be void* or class type), and a member variable TypeData object. Before calling ClassMember<>(), member variable type can be checked by comparing to types using helper function TypeHashID<type_to_check>()
```cpp
    // Member Variable by Index
    TypeData member = MemberData(t, 0);
    if (member.type_hash == TypeHashID<int>()) {
        // Create reference to member
        int& width = ClassMember<int>(&t, member);     
        // Can now set member variable directly
        width = 120;                                        
    }

    // Member Variable by Name
    TypeData member = MemberData(t, "position");
    if (member.type_hash == TypeHashID<std::vector<double>>()) {
        // Create reference to member
        std::vector<double>& position = ClassMember<std::vector<double>>(&t, member);
        // Can now set member variable directly
        position = { 2.0, 4.0, 6.0 };
    }
```

-----
## Iterating Members / Properties
```cpp
    int member_count = ClassData(t).member_count;
    for (int index = 0; index < member_count; ++index) {
        TypeData member = MemberData(t, index);
        std::cout << " Index: " << member.index << ", ";
        std::cout << " Name: "  << member.name  << ", ";
        std::cout << " Title: " << member.title << ", ";
        std::cout << " Value: ";
        if (member.type_hash == TypeHashID<int>()) {
            std::cout << ClassMember<int>(&t, member);
        } else if (member.type_hash == TypeHashID<std::string>()) {
            std::cout << ClassMember<std::string>(&t, member);
        } else if (member.type_hash == TypeHashID<std::vector<double>>()) {
            std::vector<double>& vec = ClassMember<std::vector<double>>(&t, member);
            for (auto& number : vec) {
                std::cout << number << ", ";
            }
        }
    }
```

-----
## Data from Unknown Class Type
- If using with an entity component system, it's possible you may not have access to class type at runtime. Often a collection of components are stored in a container of void pointers. Somewhere in your code when your class is initialized, store the component class TypeHash:
```cpp
    TypeHash saved_hash = ClassData(t).type_hash;
    void* class_pointer = (void*)(&t);
```  
- Later (if your components are stored as void pointers in an array / vector / etc. with other components) you may still access the member variables of the component without casting the component back to the original type. This is done by using the saved_hash from earlier:
```cpp
    using vec = std::vector<double>;
    TypeData member = MemberData(saved_hash, 3);
    if (member.type_hash == TypeHashID<vec>()) {
        vec& rotation = ClassMember<vec>(class_pointer, member);
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
- BY REFERENCE, pass a TypeData object (class or member, this can be retrieved many different ways as shown earlier) to the meta data functions to get / set meta data at runtime:
```cpp
    // TypeData from class
    TypeData& type_data = ClassData<Transform2D>();
    // or from member variable
    TypeData& type_data = MemberData<Transform2D>("width");

    // Get meta data
    std::string description = GetMetaData(type_data, META_DATA_DESCRIPTION);
    std::string icon_file   = GetMetaData(type_data, "icon");

    // Set meta data
    SetMetaData(type_data, META_DATA_DESCRIPTION, description);
    SetMetaData(type_data, "icon", icon_file);
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