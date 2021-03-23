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
//      TYPE DATA
//      ---------
//      - Class Type Data
//          GetClassData<Transform2D>().member_count;   // By class type
//          GetClassData(t).member_count;               // By class instance
//          GetClassData(hash_id).member_count;         // By class hash id
//          GetClassData("Transform2D").member_count;   // By class name
//
//      - Member Type Data
//          GetMemberData<Transform2D>(0).name;         // By class type, member index
//          GetMemberData<Transform2D>("width").name;   // By class type, member name
//          GetMemberData(t, 0).name;                   // By class instance, member index
//          GetMemberData(t, "width").name;             // By class instance, member name 
//          GetMemberData(hash_id, 0).name;             // By class hash id, member index
//          GetMemberData(hash_id, "width").name;       // By class hash id, member name 
//
//
//      GET / SET MEMBER VARIABLE
//      -------------------------
//      Before calling GetValue<>(), member variable type can be checked by comparing to
//      predefined constants. These can easily be added to under "Member Types" below...
//
//      - GetValue by Index
//          HashID type = GetMemberData(t, 0).hash_code;
//          if (type == MEMBER_TYPE_INT) {
//              int width = GetValue<int>(t, 0);
//          }
//
//      - GetValue by Name
//          HashID type = GetMemberData(t, "position").hash_code;
//          if (type == MEMBER_TYPE_VECTOR_DOUBLE) {
//              std::vector<double> position = GetValue<std::vector<double>>(t, "position");
//          }
//
//      - SetValue by Index
//          HashID type = GetMemberData(t, 0).hash_code;
//          if (type == MEMBER_TYPE_INT) {
//              int new_width = 50;
//              SetValue(t, 0, new_width);
//          }
//
//      - SetValue by Name
//          HashID type = GetMemberData(t, "position").hash_code;
//          if (type == MEMBER_TYPE_VECTOR_DOUBLE) {
//              std::vector<double> new_position = { 56.0, 58.5, 60.2 };
//              SetValue(t, "position", new_position);
//          }
//
//      - Iterating Members / Properties
//          using vec = std::vector<double>;
//          int member_count = GetClassData("Transform2D").member_count;
//          for (int index = 0; index < member_count; ++index) {
//              MemberData& member = GetMemberData(t, index);
//              std::cout << " Index: " << member.index << ", ";
//              std::cout << " Name: " << member.name << ",";
//              std::cout << " Value: ";
//              HashID member_type = member.hash_code;
//              if (member_type == MEMBER_TYPE_INT) {
//                  std::cout << GetValue<int>(t, index);
//              } else if (member_type == MEMBER_TYPE_VECTOR_DOUBLE) {
//                  std::cout << GetValue<std::vector<double>>(t, index)[0];
//              }
//          }
//
//      - Data from Unknown Class Type
//          ...
//          HashID saved_hash = GetClassData(t).hash_code;
//          void* class_pointer = (void*)(&t);
//          ... 
//          HashID member_type = GetMemberData(saved_hash, 1).hash_code;
//          if (member_type == MEMBER_TYPE_INT) {
//              std::cout << GetValue<int>(class_pointer, saved_hash, 1); 
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
//      To get / set meta data at runtime, pass a ClassData or MemberData type data object 
//      (this can be retrieved many different ways as shown earlier) to the meta data functions.
//      THIS DATA OBJECT MUST BE PASSED BY REFERENCE!
//
//      - Get class meta data
//          ClassData& class_data = GetClassData<Transform2D>();
//          std::string description = GetClassMeta(class_data, META_DATA_DESCRIPTION);
//          std::string icon_file   = GetClassMeta(class_data, "icon");
//      - Set class meta data
//          SetClassMeta(class_data, META_DATA_DESCRIPTION, description);
//          SetClassMeta(class_data, "icon", icon_file);
//
//      - Get member meta data
//          MemberData& member_data = GetMemberData<Transform2D>("position");
//          std::string description = GetMemberMeta(member_data, META_DATA_DESCRIPTION);
//      - Set member meta data
//          SetMemberMeta(member_data, META_DATA_DESCRIPTION, description);
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
using HashID =          size_t;                                                     // This comes from typeid(OBJECT).hash_code()
using Functions =       std::vector<std::function<void()>>;                         // List of functions
using IntMap =          std::unordered_map<int, std::string>;                       // Meta data int key map
using StringMap =       std::map<std::string, std::string>;                         // Meta data string key map

//####################################################################################
//##    HashID Helper
//############################
template <typename T> HashID                CreateHashID() { return typeid(T).hash_code(); }

//####################################################################################
//##    Member Types
//############################
const HashID MEMBER_TYPE_UNKNOWN =          0;
const HashID MEMBER_TYPE_BOOL =             CreateHashID<bool>();
const HashID MEMBER_TYPE_CHAR =             CreateHashID<char>();
const HashID MEMBER_TYPE_STRING =           CreateHashID<std::string>();
const HashID MEMBER_TYPE_INT =              CreateHashID<int>();
const HashID MEMBER_TYPE_UINT =             CreateHashID<unsigned int>();
const HashID MEMBER_TYPE_LONG =             CreateHashID<long>();
const HashID MEMBER_TYPE_FLOAT =            CreateHashID<float>();
const HashID MEMBER_TYPE_DOUBLE =           CreateHashID<double>();
const HashID MEMBER_TYPE_VECTOR_BOOL =      CreateHashID<std::vector<bool>>();
const HashID MEMBER_TYPE_VECTOR_CHAR =      CreateHashID<std::vector<char>>();
const HashID MEMBER_TYPE_VECTOR_STRING =    CreateHashID<std::vector<std::string>>();
const HashID MEMBER_TYPE_VECTOR_INT =       CreateHashID<std::vector<int>>();
const HashID MEMBER_TYPE_VECTOR_UINT =      CreateHashID<std::vector<unsigned int>>();
const HashID MEMBER_TYPE_VECTOR_LONG =      CreateHashID<std::vector<long>>();
const HashID MEMBER_TYPE_VECTOR_FLOAT =     CreateHashID<std::vector<float>>();
const HashID MEMBER_TYPE_VECTOR_DOUBLE =    CreateHashID<std::vector<double>>();
// ...
// Easy to add more predefined types here...
// These predifined types allow for easy identification before calling: GetValue<type>() / SetValue()
// ...

//####################################################################################
//##    Class / Member Type Data
//############################
struct ClassData {
    std::string         name            { "unknown" };                              // Actual struct / class name 
    std::string         title           { "unknown" };                              // Pretty (capitalized, spaced) name for displaying to user
    HashID              hash_code       { 0 };                                      // typeid().hash_code of actual underlying type of class
    int                 member_count    { 0 };                                      // Number of registered member variables of class
    IntMap              meta_int_map    { };                                        // Map to hold user meta data by int key
    StringMap           meta_string_map { };                                        // Map to hold user meta data by string key
};

struct MemberData {
    std::string         name            { "unknown" };                              // Actual member variable name 
    std::string         title           { "unknown" };                              // Pretty (capitalized, spaced) name for displaying to user
    int                 index           { -1 };                                     // Index of member variable within class
    HashID              hash_code       { 0 };                                      // typeid().hash_code of actual underlying type of member variable
    int                 offset          { 0 };                                      // char* offset of member variable within parent class / struct
    size_t              size            { 0 };                                      // size of actual type of member variable
    IntMap              meta_int_map    { };                                        // Map to hold user meta data by int key
    StringMap           meta_string_map { };                                        // Map to hold user meta data by string key
};
static ClassData  unknown_class   { };                                              // Empty ClassData to return by reference on GetClassData() fail
static MemberData unknown_member  { };                                              // Empty MemberData to return by reference on GetMemberData() fail

//####################################################################################
//##    DrReflect
//##        Singleton to hold Class / Member reflection and meta data
//############################
class DrReflect 
{
public:
    using MemberMap = std::map<int, MemberData>;
    std::unordered_map<HashID, ClassData>           classes         { };            // Holds data about classes / structs
    std::unordered_map<HashID, MemberMap>           members         { };            // Holds data about member variables (of classes)

public:    
    void AddClass(ClassData class_data) {
        assert(class_data.hash_code != 0 && "Class hash code is 0, error in registration?");
        classes[class_data.hash_code] = class_data;
    }
    void AddMember(ClassData class_data, MemberData member_data) {
        assert(class_data.hash_code != 0 && "Class hash code is 0, error in registration?");
        assert(classes.find(class_data.hash_code) != classes.end() && "Class never registered with AddClass before calling AddMember!");
        members[class_data.hash_code][member_data.offset] = member_data;
        classes[class_data.hash_code].member_count = members[class_data.hash_code].size();
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
void            RegisterClass(ClassData class_data);                                // Update class type data
void            RegisterMember(ClassData class_data, MemberData member_data);       // Update member type data                

// Class meta data
void            SetClassMeta(ClassData& class_data, int key, std::string data);
void            SetClassMeta(ClassData& class_data, std::string key, std::string data);
std::string     GetClassMeta(ClassData& class_data, int key);
std::string     GetClassMeta(ClassData& class_data, std::string key);

// Member meta data
void            SetMemberMeta(MemberData& member_data, int key, std::string data);
void            SetMemberMeta(MemberData& member_data, std::string key, std::string data);
std::string     GetMemberMeta(MemberData& member_data, int key);
std::string     GetMemberMeta(MemberData& member_data, std::string key);

//####################################################################################
//##    Class / Member Registration
//############################
// Template wrapper to register type information with DrReflect from header files
template <typename T> void InitiateClass() { };                                                

// Call this to register class / struct type with reflection / meta data system
template <typename ClassType>
void RegisterClass(ClassData class_data) { 
    assert(std::is_standard_layout<ClassType>() && "Class is not standard layout!!");
	g_reflect->AddClass(class_data); 
}

// Call this to register member variable with reflection / meta data system
template <typename MemberType>
void RegisterMember(ClassData class_data, MemberData member_data) {
	g_reflect->AddMember(class_data, member_data); 
} 

//####################################################################################
//##    Reflection Data Fetching
//############################
// #################### Class Data Fetching ####################
// Class type data fetching by class name
template<typename T>
ClassData& GetClassData() {
    HashID class_hash_id = typeid(T).hash_code();
    if (g_reflect->classes.find(class_hash_id) != g_reflect->classes.end()) {
        return g_reflect->classes[class_hash_id];
    } else { 
        return unknown_class; 
    }
}
// Class type data fetching from passed in class instance
template<typename T>
ClassData& GetClassData(T& class_instance) {
    return GetClassData<T>();
}
// Class type data fetching from passed in class typeid().hash_code()
ClassData& GetClassData(HashID hash_id);
// Class type data fetching from passed in class name
ClassData& GetClassData(std::string class_name);
ClassData& GetClassData(const char* class_name);

// #################### Member Data Fetching ####################
// -------------------------    By Index  -------------------------
// Member type data fetching by member variable index and class typeid().hash_code()
MemberData& GetMemberData(HashID class_hash_id, int member_index);
// Member type data fetching by member variable index and class name
template<typename T>
MemberData& GetMemberData(int member_index) {
    return GetMemberData(CreateHashID<T>(), member_index);   
}
// Member type data fetching by member variable index and class instance
template<typename T>
MemberData& GetMemberData(T& class_instance, int member_index) {
    return GetMemberData<T>(member_index);   
}

// -------------------------    By Name  -------------------------
// Member type data fetching by member variable Name and class typeid().hash_code()
MemberData& GetMemberData(HashID class_hash_id, std::string member_name);
// Member type data fetching by member variable Name and class name
template<typename T>
MemberData& GetMemberData(std::string member_name) {
    return GetMemberData(CreateHashID<T>(), member_name); 
}
// Member type data fetching by member variable name and class instance
template<typename T>
MemberData& GetMemberData(T& class_instance, std::string member_name) {
    return GetMemberData<T>(member_name); 
}

// #################### Member Variable Value Fetching ####################
// NOTES:
//  Casting from void*, not fully standardized across compilers?
//      DrVec3 rotation = *(DrVec3*)(class_ptr + member_data.offset);
//  Memcpy
//      DrVec3 value;
//      memcpy(&value, class_ptr + member_data.offset, member_data.size);
//  C++ Member Pointer
//      static constexpr auto offset_rotation = &Transform2D::rotation;
//      DrVec3 rotation = ((&et)->*off_rot);
// ##### Internal casting function
template<typename ReturnType>
ReturnType InternalGetValue(char* class_ptr, MemberData member_data) {
    assert(member_data.name != "unknown" && "Could not find member variable!");
    assert(member_data.hash_code == CreateHashID<ReturnType>() && "Did not request correct return type!");
    return *(reinterpret_cast<ReturnType*>(class_ptr + member_data.offset));
}
// Get member variable value from class by index
template<typename ReturnType, typename ClassType>
ReturnType GetValue(ClassType& class_instance, int member_index) {
    MemberData member_data = GetMemberData<ClassType>(member_index);
    return InternalGetValue<ReturnType>((char*)(&class_instance), GetMemberData<ClassType>(member_index));
}
// Get member variable value from class by name
template<typename ReturnType, typename ClassType>
ReturnType GetValue(ClassType& class_instance, std::string member_name) {
    return InternalGetValue<ReturnType>((char*)(&class_instance), GetMemberData<ClassType>(member_name));
}
// Get member variable value using class HashID by index
template<typename ReturnType>
ReturnType GetValue(void* class_ptr, HashID class_hash_id, int member_index) {
    return InternalGetValue<ReturnType>((char*)(class_ptr), GetMemberData(class_hash_id, member_index));
}
// Get member variable value using class HashID by name
template<typename ReturnType>
ReturnType GetValue(void* class_ptr, HashID class_hash_id, std::string member_name) {
    return InternalGetValue<ReturnType>((char*)(class_ptr), GetMemberData(class_hash_id, member_name));
}

// #################### Member Variable Value Setting ####################
// NOTES:
//  Memcpy
//     char *p = block;
//     memcpy(p + offset, &val, sizeof(val));
// ##### Internal casting function
template<typename MemberType>
void InternalSetValue(char* class_ptr, MemberData member_data, MemberType value) {
    assert(member_data.name != "unknown" && "Could not find member variable!");
    assert(member_data.hash_code == CreateHashID<MemberType>() && "Did not pass correct value type!");
    MemberType& existing = *(reinterpret_cast<MemberType*>(class_ptr + member_data.offset));
    existing = value;
}
// Set member variable of class by index
template<typename MemberType, typename ClassType>
void SetValue(ClassType& class_instance, int member_index, MemberType value) {
    InternalSetValue<MemberType>((char*)(&class_instance), GetMemberData<ClassType>(member_index), value);
}
// Set member variable of class by name
template<typename MemberType, typename ClassType>
void SetValue(ClassType& class_instance, std::string member_name, MemberType value) {
    InternalSetValue<MemberType>((char*)(&class_instance), GetMemberData<ClassType>(member_name), value);
}
// Set member variable value using class HashID by index
template<typename MemberType>
void SetValue(void* class_ptr, HashID class_hash_id, int member_index, MemberType value) {
    InternalSetValue<MemberType>((char*)(class_ptr), GetMemberData(class_hash_id, member_index), value);
}
// Set member variable value using class HashID by name
template<typename MemberType>
void SetValue(void* class_ptr, HashID class_hash_id, std::string member_name, MemberType value) {
    InternalSetValue<MemberType>((char*)(class_ptr), GetMemberData(class_hash_id, member_name), value);
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
        ClassData class_data {}; \
			class_data.name = #TYPE; \
			class_data.hash_code = typeid(TYPE).hash_code(); \
			class_data.title = #TYPE; \
            CreateTitle(class_data.title); \
		RegisterClass<T>(class_data); \
		int member_index = -1; \
		std::unordered_map<int, MemberData> mbrs { };

// Meta data functions
#define CLASS_META_TITLE(STRING) \
        class_data.title = #STRING;	\
        RegisterClass(class_data);
#define CLASS_META_DATA(KEY,VALUE) \
        SetClassMeta(class_data, KEY, VALUE); \
        RegisterClass(class_data);

// Member Registration
#define REFLECT_MEMBER(MEMBER) \
		member_index++; \
		mbrs[member_index] = MemberData(); \
		mbrs[member_index].name = #MEMBER; \
        mbrs[member_index].index = member_index; \
		mbrs[member_index].hash_code = typeid(T::MEMBER).hash_code(); \
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
        SetMemberMeta(mbrs[member_index], KEY, VALUE); \
        RegisterMember(class_data, mbrs[member_index]);

//#define MEMBER_META_DESCRIPTION(STRING) mbrs[member_index].description = 	#STRING; 	RegisterMember(class_data, mbrs[member_index]); 

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
        } else if ((isalpha(name[c - 1]) && isnumber(name[c]))) {
            title += std::string(" ");
        }
        title += name[c];
    }
    name = title;
}

// ########## Class / Member Registration ##########
// Update class type data
void RegisterClass(ClassData class_data) { 
	g_reflect->AddClass(class_data); 
}

// Update member type data
void RegisterMember(ClassData class_data, MemberData member_data) {                     
	g_reflect->AddMember(class_data, member_data); 
} 

//####################################################################################
//##    Type Data Fetching
//####################################################################################
// ########## Class Data Fetching ##########
// Class type data fetching from passed in class typeid().hash_code()
ClassData& GetClassData(HashID hash_id) {
    for (auto& pair : g_reflect->classes) {
        if (pair.first == hash_id) return pair.second;
    }
    return unknown_class;
}
// Class type data fetching from passed in class name
ClassData& GetClassData(std::string class_name) {
    for (auto& pair : g_reflect->classes) {
        if (pair.second.name == class_name) return pair.second;
    }
    return unknown_class;
}
// Class type data fetching from passed in class name
ClassData& GetClassData(const char* class_name) {
    return GetClassData(std::string(class_name));
}

// ########## Member Data Fetching ##########
// Member type data fetching by member variable index and class typeid().hash_code()
MemberData& GetMemberData(HashID class_hash_id, int member_index) {
    int count = 0;
    for (auto& member : g_reflect->members[class_hash_id]) {
        if (count == member_index) return member.second;
        ++count;
    }
    return unknown_member;
}
// Member type data fetching by member variable name and class typeid().hash_code()
MemberData& GetMemberData(HashID class_hash_id, std::string member_name) {
    for (auto& member : g_reflect->members[class_hash_id]) {
        if (member.second.name == member_name) return member.second;
    }
    return unknown_member;
}

//####################################################################################
//##    Meta Data User Info
//####################################################################################
// Set class meta data
void SetClassMeta(ClassData& class_data, int key, std::string data) {
    if (class_data.hash_code != 0) class_data.meta_int_map[key] = data;
}
void SetClassMeta(ClassData& class_data, std::string key, std::string data) {
    if (class_data.hash_code != 0) class_data.meta_string_map[key] = data;
}
// Get class meta data
std::string GetClassMeta(ClassData& class_data, int key) {
    if (class_data.hash_code != 0) {
        if (class_data.meta_int_map.find(key) != class_data.meta_int_map.end())
            return class_data.meta_int_map[key];
    }
    return "";
}
std::string GetClassMeta(ClassData& class_data, std::string key) {
    if (class_data.hash_code != 0) {
        if (class_data.meta_string_map.find(key) != class_data.meta_string_map.end())
            return class_data.meta_string_map[key];
    }
    return "";
}
// Set member meta data
void SetMemberMeta(MemberData& member_data, int key, std::string data) {
    if (member_data.hash_code != 0) member_data.meta_int_map[key] = data;
}
void SetMemberMeta(MemberData& member_data, std::string key, std::string data) {
    if (member_data.hash_code != 0) member_data.meta_string_map[key] = data;
}
// Get member meta data
std::string GetMemberMeta(MemberData& member_data, int key) {
    if (member_data.hash_code != 0) {
        if (member_data.meta_int_map.find(key) != member_data.meta_int_map.end())
            return member_data.meta_int_map[key];
    }
    return "";
}
std::string GetMemberMeta(MemberData& member_data, std::string key) {
    if (member_data.hash_code != 0) {
        if (member_data.meta_string_map.find(key) != member_data.meta_string_map.end())
            return member_data.meta_string_map[key];
    }
    return "";
}

#endif  // REGISTER_REFLECTION
#endif  // DR_REFLECT_H