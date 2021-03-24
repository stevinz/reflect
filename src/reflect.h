//####################################################################################
//
// Description:     Reflect, C++ 11 Reflection Library
// Author:          Stephens Nunnally and Scidian Software
// License:         Distributed under the MIT License
// Source(s):       https://github.com/stevinz/reflect
//
// Copyright (c) 2021 Stephens Nunnally and Scidian Software
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//####################################################################################
//
// INSTALLATION:
// - Copy 'reflect.h' to project
//
// - In ONE cpp file, define REGISTER_REFLECTION:
//      #define REGISTER_REFLECTION
//      #include "reflect.h"
//      #include "my_struct_1.h"
//      #include "my_struct_2.h"
//      #include etc...
//
// - Classes / Structs should be simple aggregate types (standard layout)
//      - No private or protected non-static data members
//      - No user-declared / user-provided constructors 
//      - No virtual member functions
//      - No default member initializers (invalid in C++11, okay in C++14 and higher)
//      - See (https://en.cppreference.com/w/cpp/types/is_standard_layout) for more info 
//
// - BEFORE using reflection, make one call to 'InitializeReflection()'
//
//####################################################################################
//
// USAGE:
//      EXAMPLE CLASS HEADER: 
//      start header...
//
//          #include "reflect.h"
//          struct Transform2D {
//              int width;        
//              int height;
//              std::vector <double> position;
//              REFLECT();
//          }
//          #ifdef REGISTER_REFLECTION
//              REFLECT_CLASS(Transform2D)
//              REFLECT_MEMBER(width)
//              REFLECT_MEMBER(height)
//              REFLECT_MEMBER(position)
//              REFLECT_END(Transform2D)
//          #endif
//
//      ...end header
//
//####################################################################################
//
// IN CODE:
//      ...
//      Transform2D t { };
//          t.width =    100;
//          t.height =   100;
//          t.position = std::vector<double>({1.0, 2.0, 3.0});
//      ...
//
//      TYPEDATA OBJECT
//      ----------------
//      - Class TypeData
//          TypeData& data = ClassData<Transform2D>();          // By class type
//          TypeData& data = ClassData(t);                      // By class instance
//          TypeData& data = ClassData(type_hash);              // By class type hash
//          TypeData& data = ClassData("Transform2D");          // By class name
//
//      - Member TypeData
//          TypeData& data = MemberData<Transform2D>(0);        // By class type, member index
//          TypeData& data = MemberData<Transform2D>("width");  // By class type, member name
//          TypeData& data = MemberData(t, 0);                  // By class instance, member index
//          TypeData& data = MemberData(t, "width");            // By class instance, member name 
//          TypeData& data = MemberData(type_hash, 0);          // By class type hash, member index
//          TypeData& data = MemberData(type_hash, "width");    // By class type hash, member name 
//
//
//      GET / SET MEMBER VARIABLE
//      -------------------------
//      Use the ClassMember<member_type>(class_instance, member_data) function to return
//      a reference to a member variable. This function requires the return type, a class
//      instance (can be void* or class type), and a member variable TypeData object.
//      Before calling ClassMember<>(), member variable type can be checked by comparing to
//      types using helper function TypeHashID<type_to_check>().
//
//      - Member Variable by Index
//          TypeData& member = MemberData(t, 0);
//          if (member.type_hash == TypeHashID<int>()) {
//              // Create reference to member
//              int& width = ClassMember<int>(&t, member);     
//              // Can now set member variable directly
//              width = 120;                                        
//          }
//
//      - Member Variable by Name
//          TypeData& member = MemberData(t, "position");
//          if (member.type_hash == TypeHashID<std::vector<double>>()) {
//              // Create reference to member
//              std::vector<double>& position = ClassMember<std::vector<double>>(&t, member);
//              // Can now set member variable directly
//              position = { 2.0, 4.0, 6.0 };
//          }
//
//      - Iterating Members / Properties
//          int member_count = ClassData("Transform2D").member_count;
//          for (int index = 0; index < member_count; ++index) {
//              TypeData& member = MemberData(t, index);
//              std::cout << " Index: " << member.index << ", ";
//              std::cout << " Name: " << member.name << ",";
//              std::cout << " Value: ";
//              if (member.type_hash == TypeHashID<int>()) {
//                  std::cout << ClassMember<int>(&t, member);
//              } else if (member.type_hash == TypeHashID<std::vector<double>>()) {
//                  std::cout << ClassMember<std::vector<double>>(&t, member)[0];
//              }
//          }
//
//      - Data from Unknown Class Type
//          ...
//          TypeHash saved_hash = ClassData(t).type_hash;
//          void* class_pointer = (void*)(&t);
//          ... 
//          TypeData& member = MemberData(saved_hash, 1);
//          if (member.type_hash == TypeHashID<int>) {
//              std::cout << GetValue<int>(class_pointer, member); 
//          }
//
//
//      USER META DATA
//      --------------
//      Meta data can by stored as std::string within a class or member type. Set user meta data
//      at compile time using CLASS_META_DATA and MEMBER_META_DATA in the class header file during
//      registration with (int, string) or (sting, string) pairs:
//
//          REFLECT_CLASS(Transform2D)
//              CLASS_META_DATA(META_DATA_DESCRIPTION, "Describes object in 2D space.")
//              CLASS_META_DATA("icon", "assets/transform.png")
//          REFLECT_MEMBER(width)
//              MEMBER_META_DATA(META_DATA_DESCRIPTION, "Width of this object.")
//
//      To get / set meta data at runtime, pass a TypeData object (class or member, this can be
//      retrieved many different ways as shown earlier) to the meta data functions. TypeData object
//      MUST BE PASSED BY REFERENCE!
//
//      - Get Reference TypeData
//          TypeData& type_data = ClassData<Transform2D>();             // From class...
//          TypeData& type_data = MemberData<Transform2D>("width");     // or from member variable
//
//      - Get meta data
//          std::string description = GetClassMeta(type_data, META_DATA_DESCRIPTION);
//          std::string icon_file   = GetClassMeta(type_data, "icon");
//
//      - Set meta data
//          SetClassMeta(type_data, META_DATA_DESCRIPTION, description);
//          SetClassMeta(type_data, "icon", icon_file);
//
//
//
//####################################################################################
//
//      Visit (https://github.com/stevinz/reflect) for more detailed instructions
//
//####################################################################################
#ifndef DR_REFLECT_H
#define DR_REFLECT_H

// Includes
#include <functional>
#include <map>
#include <unordered_map>
#include <string>
#include <typeinfo>
#include <vector>

//####################################################################################
//##    Sample Meta Data Enum
//############################
enum Meta_Data {
    META_DATA_DESCRIPTION,
    META_DATA_HIDDEN,
    META_DATA_TYPE,
    META_DATA_COLOR,
    META_DATA_ICON,
    META_DATA_TOOLTIP,
    // etc...
};

//####################################################################################
//##    Type Definitions
//############################
using TypeHash =        size_t;                                                     // This comes from typeid().hash_code()
using Functions =       std::vector<std::function<void()>>;                         // List of functions
using IntMap =          std::unordered_map<int, std::string>;                       // Meta data int key map
using StringMap =       std::map<std::string, std::string>;                         // Meta data string key map

//####################################################################################
//##    Class / Member Type Data
//############################
struct TypeData {
    std::string         name            { "unknown" };                              // Actual struct / class / member variable name 
    std::string         title           { "unknown" };                              // Pretty (capitalized, spaced) name for displaying in gui
    TypeHash            type_hash       { 0 };                                      // Underlying typeid().hash_code of actual type
    IntMap              meta_int_map    { };                                        // Map to hold user meta data by int key
    StringMap           meta_string_map { };                                        // Map to hold user meta data by string key
    // For Class Data
    int                 member_count    { 0 };                                      // Number of registered member variables of class
    // For Member Data
    int                 index           { -1 };                                     // Index of member variable within parent class / struct
    int                 offset          { 0 };                                      // Char* offset of member variable within parent class / struct
    size_t              size            { 0 };                                      // Size of actual type of member variable
};

// Empty TypeData to return by reference on GetTypeData() fail
static TypeData         unknown_type    { };                                        

//####################################################################################
//##    DrReflect
//##        Singleton to hold Class / Member reflection and meta data
//############################
class DrReflect 
{
public:
    std::unordered_map<TypeHash, TypeData>                  classes     { };        // Holds data about classes / structs
    std::unordered_map<TypeHash, std::map<int, TypeData>>   members     { };        // Holds data about member variables (of classes)

public:    
    void AddClass(TypeData class_data) {
        assert(class_data.type_hash != 0 && "Class type hash is 0, error in registration?");
        classes[class_data.type_hash] = class_data;
    }
    void AddMember(TypeData class_data, TypeData member_data) {
        assert(class_data.type_hash != 0 && "Class type hash is 0, error in registration?");
        assert(classes.find(class_data.type_hash) != classes.end() && "Class never registered with AddClass before calling AddMember!");
        members[class_data.type_hash][member_data.offset] = member_data;
        classes[class_data.type_hash].member_count = members[class_data.type_hash].size();
    }
};

//####################################################################################
//##    Global Variable Declarations
//############################
extern std::shared_ptr<DrReflect>   g_reflect;                                      // Meta data singleton
extern Functions                    g_register_list;                                // Keeps list of registration functions 

//####################################################################################
//##    General Functions
//############################
void            InitializeReflection();                                             // Creates DrReflect instance and registers classes and member variables
void            CreateTitle(std::string& name);                                     // Create nice display name from class / member variable names
void            RegisterClass(TypeData class_data);                                 // Update class TypeData
void            RegisterMember(TypeData class_data, TypeData member_data);          // Update member TypeData                

// TypeHash helper function
template <typename T> 
TypeHash        TypeHashID() { return typeid(T).hash_code(); }

// Meta data
void            SetMetaData(TypeData& type_data, int key, std::string data);
void            SetMetaData(TypeData& type_data, std::string key, std::string data);
std::string     GetMetaData(TypeData& type_data, int key);
std::string     GetMetaData(TypeData& type_data, std::string key);

//####################################################################################
//##    Class / Member Registration
//############################
// Template wrapper to register type information with DrReflect from header files
template <typename T> void InitiateClass() { };                                                

// Call this to register class / struct type with reflection / meta data system
template <typename ClassType>
void RegisterClass(TypeData class_data) { 
    assert(std::is_standard_layout<ClassType>() && "Class is not standard layout!!");
	g_reflect->AddClass(class_data); 
}

// Call this to register member variable with reflection / meta data system
template <typename MemberType>
void RegisterMember(TypeData class_data, TypeData member_data) {
	g_reflect->AddMember(class_data, member_data); 
} 

//####################################################################################
//##    Reflection TypeData Fetching
//############################
// #################### Class Data Fetching ####################
// Class TypeData fetching by actual class type
template<typename T>
TypeData& ClassData() {
    TypeHash class_hash = typeid(T).hash_code();
    if (g_reflect->classes.find(class_hash) != g_reflect->classes.end()) {
        return g_reflect->classes[class_hash];
    } else { 
        return unknown_type; 
    }
}
// Class TypeData fetching from passed in class instance
template<typename T>
TypeData& ClassData(T& class_instance) {
    return ClassData<T>();
}
// Class TypeData fetching from passed in class TypeHash
TypeData& ClassData(TypeHash class_hash);
// Class TypeData fetching from passed in class name
TypeData& ClassData(std::string class_name);
TypeData& ClassData(const char* class_name);

// #################### Member Data Fetching ####################
// -------------------------    By Index  -------------------------
// Member TypeData fetching by member variable index and class TypeHash
TypeData& MemberData(TypeHash class_hash, int member_index);
// Member TypeData fetching by member variable index and class name
template<typename T>
TypeData& MemberData(int member_index) {
    return MemberData(TypeHashID<T>(), member_index);   
}
// Member TypeData fetching by member variable index and class instance
template<typename T>
TypeData& MemberData(T& class_instance, int member_index) {
    return MemberData<T>(member_index);   
}

// -------------------------    By Name  -------------------------
// Member TypeData fetching by member variable Name and class TypeHash
TypeData& MemberData(TypeHash class_hash, std::string member_name);
// Member TypeData fetching by member variable Name and class name
template<typename T>
TypeData& MemberData(std::string member_name) {
    return MemberData(TypeHashID<T>(), member_name); 
}
// Member TypeData fetching by member variable name and class instance
template<typename T>
TypeData& MemberData(T& class_instance, std::string member_name) {
    return MemberData<T>(member_name); 
}

// #################### Member Variable Fetching ####################
// NOTES:
//  Internal Casting
//      /* Casting from void*, not fully standardized across compilers? */
//      DrVec3 rotation = *(DrVec3*)(class_ptr + member_data.offset);
//  Memcpy
//      DrVec3 value;
//      memcpy(&value, class_ptr + member_data.offset, member_data.size);
//  C++ Member Pointer
//      static constexpr auto offset_rotation = &Transform2D::rotation;
//      DrVec3 rotation = ((&et)->*off_rot);
template<typename ReturnType>
ReturnType& ClassMember(void* class_ptr, TypeData& member_data) {
    assert(member_data.name != "unknown" && "Could not find member variable!");
    assert(member_data.type_hash == TypeHashID<ReturnType>() && "Did not request correct return type!");
    return *(reinterpret_cast<ReturnType*>(((char*)(class_ptr)) + member_data.offset));
}

//####################################################################################
//##    Macros for Reflection Registration
//####################################################################################
// Static variable added to class allows registration function to be added to list of classes to be registered
#define REFLECT() \
    static bool reflection; \
    static bool initReflection();

// Define Registration Function
#define REFLECT_CLASS(TYPE) \
    template <> void InitiateClass<TYPE>() { \
        using T = TYPE; \
        TypeData class_data {}; \
			class_data.name = #TYPE; \
			class_data.type_hash = typeid(TYPE).hash_code(); \
			class_data.title = #TYPE; \
            CreateTitle(class_data.title); \
		RegisterClass<T>(class_data); \
		int member_index = -1; \
		std::unordered_map<int, TypeData> mbrs { };

// Meta data functions
#define CLASS_META_TITLE(STRING) \
        class_data.title = #STRING;	\
        RegisterClass(class_data);
#define CLASS_META_DATA(KEY,VALUE) \
        SetMetaData(class_data, KEY, VALUE); \
        RegisterClass(class_data);

// Member Registration
#define REFLECT_MEMBER(MEMBER) \
		member_index++; \
		mbrs[member_index] = TypeData(); \
		mbrs[member_index].name = #MEMBER; \
        mbrs[member_index].index = member_index; \
		mbrs[member_index].type_hash = typeid(T::MEMBER).hash_code(); \
		mbrs[member_index].offset = offsetof(T, MEMBER); \
		mbrs[member_index].size = sizeof(T::MEMBER); \
		mbrs[member_index].title = #MEMBER; \
        CreateTitle(mbrs[member_index].title); \
		RegisterMember<decltype(T::MEMBER)>(class_data, mbrs[member_index]); 

// Meta data functions
#define MEMBER_META_TITLE(STRING) \
        mbrs[member_index].title = #STRING;	\
        RegisterMember(class_data, mbrs[member_index]);
#define MEMBER_META_DATA(KEY,VALUE) \
        SetMetaData(mbrs[member_index], KEY, VALUE); \
        RegisterMember(class_data, mbrs[member_index]);

// Static definitions add registration function to list of classes to be registered
#define REFLECT_END(TYPE) \
    } \
    bool TYPE::reflection { initReflection() }; \
    bool TYPE::initReflection() { \
        g_register_list.push_back(std::bind(&InitiateClass<TYPE>)); \
        return true; \
    }

//####################################################################################
//####################################################################################
//####################################################################################
//##    BEGIN REFLECT IMPLEMENTATION
//####################################################################################
//####################################################################################
//####################################################################################
#ifdef REGISTER_REFLECTION

// Gloabls
std::shared_ptr<DrReflect>      g_reflect           { nullptr };                    // Meta data singleton
Functions                       g_register_list     { };                            // Keeps list of registration functions

// ########## General Registration ##########
// Initializes global reflection object, registers classes with reflection system
void InitializeReflection() {
    // Create Singleton
    g_reflect = std::make_shared<DrReflect>();
    
    // Register Structs / Classes
    for (int func = 0; func < g_register_list.size(); ++func) {
        g_register_list[func]();       
    }
    g_register_list.clear();        // Clean up
}

// Used in registration macros to automatically create nice display name from class / member variable names
void CreateTitle(std::string& name) {
    // Replace underscores, capitalize first letters
    std::replace(name.begin(), name.end(), '_', ' ');
    name[0] = toupper(name[0]);
    for (int c = 1; c < name.length(); c++) {
        if (name[c - 1] == ' ') name[c] = toupper(name[c]);
    }

    // Add spaces to seperate words
    std::string title = "";
    title += name[0];
    for (int c = 1; c < name.length(); c++) {
        if (islower(name[c - 1]) && isupper(name[c])) {
            title += std::string(" ");
        } else if ((isalpha(name[c - 1]) && isdigit(name[c]))) {
            title += std::string(" ");
        }
        title += name[c];
    }
    name = title;
}

// ########## Class / Member Registration ##########
// Update class TypeData
void RegisterClass(TypeData class_data) { 
	g_reflect->AddClass(class_data); 
}

// Update member TypeData
void RegisterMember(TypeData class_data, TypeData member_data) {                     
	g_reflect->AddMember(class_data, member_data); 
} 

//####################################################################################
//##    TypeData Fetching
//####################################################################################
// ########## Class Data Fetching ##########
// Class TypeData fetching from passed in class TypeHash
TypeData& ClassData(TypeHash class_hash) {
    for (auto& pair : g_reflect->classes) {
        if (pair.first == class_hash) return pair.second;
    }
    return unknown_type;
}
// Class TypeData fetching from passed in class name
TypeData& ClassData(std::string class_name) {
    for (auto& pair : g_reflect->classes) {
        if (pair.second.name == class_name) return pair.second;
    }
    return unknown_type;
}
// Class TypeData fetching from passed in class name
TypeData& ClassData(const char* class_name) {
    return ClassData(std::string(class_name));
}

// ########## Member Data Fetching ##########
// Member TypeData fetching by member variable index and class TypeHash
TypeData& MemberData(TypeHash class_hash, int member_index) {
    int count = 0;
    for (auto& member : g_reflect->members[class_hash]) {
        if (count == member_index) return member.second;
        ++count;
    }
    return unknown_type;
}
// Member TypeData fetching by member variable name and class TypeHash
TypeData& MemberData(TypeHash class_hash, std::string member_name) {
    for (auto& member : g_reflect->members[class_hash]) {
        if (member.second.name == member_name) return member.second;
    }
    return unknown_type;
}

//####################################################################################
//##    Meta Data (User Info)
//####################################################################################
void SetMetaData(TypeData& type_data, int key, std::string data) {
    if (type_data.type_hash != 0) type_data.meta_int_map[key] = data;
}
void SetMetaData(TypeData& type_data, std::string key, std::string data) {
    if (type_data.type_hash != 0) type_data.meta_string_map[key] = data;
}
std::string GetMetaData(TypeData& type_data, int key) {
    if (type_data.type_hash != 0) {
        if (type_data.meta_int_map.find(key) != type_data.meta_int_map.end())
            return type_data.meta_int_map[key];
    }
    return "";
}
std::string GetMetaData(TypeData& type_data, std::string key) {
    if (type_data.type_hash != 0) {
        if (type_data.meta_string_map.find(key) != type_data.meta_string_map.end())
            return type_data.meta_string_map[key];
    }
    return "";
}

#endif  // REGISTER_REFLECTION
#endif  // DR_REFLECT_H