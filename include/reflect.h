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
// - !!!!! In ONE cpp file, define REGISTER_REFLECTION:
//      #define REGISTER_REFLECTION
//      #include "reflect.h"
//      #include "my_struct_1.h"
//      #include "my_struct_2.h"
//      #include etc...
//
// - Classes / structs should be simple aggregate structs (standard layout)
//      - No private or protected non-static data members
//      - No user-declared / user-provided constructors 
//      - No virtual member functions
//      - No default member initializers (invalid in C++11, okay in C++14 and higher)
//
// - !!!!! BEFORE using reflection, initiate call to 'InitializeReflection()'
//
//####################################################################################
//
// USAGE:
//      EXAMPLE CLASS HEADER: 
//      start header...
//
//          #include "reflect.h"
//          struct Transform {
//              int width;        
//              int height;
//              std::vector <double> position;
//              REFLECT();
//          }
//          #ifdef REGISTER_REFLECTION
//              REFLECT_STRUCT(Transform)
//              REFLECT_MEMBER(width)
//              REFLECT_MEMBER(height)
//              REFLECT_MEMBER(position)
//              REFLECT_END(Transform)
//          #endif
//
//      ...end header
//
//####################################################################################
//
// IN CODE:
//      ...
//          Transform t { };
//              t.width =    100;
//              t.height =   100;
//              t.position = std::vector<double>({1.0, 2.0, 3.0});
//      ...
//
//      META DATA
//      ---------
//      - Component Meta Data
//          GetComponentData<Transform>().property_count;       // By class 
//          GetComponentData(t).property_count                  // By instance
//          GetComponentData("Transform").property_count;       // By name
//
//      - Property Meta Data
//          GetPropertyData<Transform>("width").description;    // By class, property name
//          GetPropertyData<Transform>(0).description;          // By class, property index
//          GetPropertyData(t, "position").description;         // By instance, property name 
//          GetPropertyData(t, 2).description;                  // By instance, property index
//
//
//      GET / SET PROPERTY
//      ------------------
//      - GetProperty by Index
//          int width = GetProperty<int>(t, 0);
//      - GetProperty by Name
//          std::vector<double> position = GetProperty<std::vector<double>>(t, "position");
//
//      - SetProperty by Index
//          int new_width = 50;
//          SetProperty(t, 0, new_width);
//      - SetProperty by Name
//          std::vector<double> new_position = { 56.0, 58.5, 60.2 };
//          SetProperty(t, "position", new_position);
//
//
//
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
//##    Property Types
//############################
enum Property_Types {
    PROPERTY_TYPE_UNKNOWN,
    PROPERTY_TYPE_BOOL,
    PROPERTY_TYPE_CHAR,
    PROPERTY_TYPE_INT,
    PROPERTY_TYPE_DOUBLE,
    PROPERTY_TYPE_VECTOR_BOOL,
    PROPERTY_TYPE_VECTOR_CHAR,
    PROPERTY_TYPE_VECTOR_INT,
    PROPERTY_TYPE_VECTOR_DOUBLE,
    PROPERTY_TYPE_STRING,
};

// Type Definitions
using HashID =          size_t;                                                     // This comes from typeid(OBJECT).hash_code()
using Functions =       std::vector<std::function<void()>>;                         // List of funcitons

//####################################################################################
//##    Component / Property data structs
//############################
struct ComponentData {
    // ----- Data Handled by Library
    std::string         name            { "unknown" };                              // Actual struct / class name 
    HashID              hash_code       { 0 };                                      // typeid().hash_code of actual underlying type of Component
    int                 property_count  { 0 };                                      // Number of registered properties of class
    // ----- Following Meta Data Can Be User Set -----
    std::string         title           { "unknown" };                              // Display name of this Component
    std::string         description     { "No component description." };            // Description of this Component
};

struct PropertyData {
    // ----- Data Handled by Library
    std::string         name            { "unknown" };                              // Actual member variable name 
    HashID              hash_code       { 0 };                                      // typeid().hash_code of actual underlying type of member variable
    int                 offset          { 0 };                                      // char* offset of member variable within parent component struct
    size_t              size            { 0 };                                      // size of actual type of Property
    // ----- Following Meta Data Can Be User Set -----
    std::string         title           { "unknown" };                              // Display name of this Property
    std::string         description     { "No property description." };             // Description of this Property
    int                 type            { PROPERTY_TYPE_UNKNOWN };                  // Type info for what type to use to retrieve value
};

//####################################################################################
//##    DrReflect
//##        Singleton to hold Component / Property reflection and meta data
//############################
class DrReflect 
{
public:
    using PropertyMap = std::map<int, PropertyData>;
    std::unordered_map<HashID, ComponentData>      components      { };             // Holds data about DrComponent / ECS Component structs
    std::unordered_map<HashID, PropertyMap>        properties      { };             // Holds data about Properies (of Components)

public:    
    void AddMetaComponent(ComponentData comp_data) {
        assert(comp_data.hash_code != 0 && "Component hash code is 0, error in registration?");
        components[comp_data.hash_code] = comp_data;
    }
    void AddMetaProperty(ComponentData comp_data, PropertyData prop_data) {
        assert(comp_data.hash_code != 0 && "Component hash code is 0, error in registration?");
        assert(components.find(comp_data.hash_code) != components.end() && "Component never set with AddMetaComponent before calling AddMetaProperty!");
        properties[comp_data.hash_code][prop_data.offset] = prop_data;
        components[comp_data.hash_code].property_count = properties[comp_data.hash_code].size();
    }
};

//####################################################################################
//##    Global Variable Declarations
//####################################################################################
extern DrReflect*       g_reflect;                                                  // Meta data singleton
extern Functions        g_register_list;                                            // Keeps list of registration functions 

//####################################################################################
//##    General Functions
//############################
void            InitializeReflection();                                             // Creates DrReflect class and registers classes and member variables
void            CreateTitle(std::string& name);                                     // Create nice display name from class / member variable names
void            RegisterComponent(ComponentData comp_data);                         // Update Component data
void            RegisterProperty(ComponentData comp_data, PropertyData prop_data);  // Update Property data                
int             PropertyCount();                                                    //	

//####################################################################################
//##    Class / Member Registration
//####################################################################################
// Template wrapper to register type with DrReflect from header files
template <typename T> void RegisterClass() { };                                                

// Call this to register class / struct type with reflection / meta data system, typename CT is Component Type
template <typename CT>
void RegisterComponent(ComponentData comp_data) { 
    assert(std::is_standard_layout<CT>() && "Component is not standard layout!!");
	g_reflect->AddMetaComponent(comp_data); 
}

// Call this to register member variable with reflection / meta data system, typename PT is Property Type
template <typename PT>
void RegisterProperty(ComponentData comp_data, PropertyData prop_data) {        
    //assert(typeid(PT).hash_code() != typeid(std::string).hash_code() && "Type std::string not supported for property type!!");             
	g_reflect->AddMetaProperty(comp_data, prop_data); 
} 

//####################################################################################
//##    Reflection Data Fetching
//############################
// #################### Component Data Fetching ####################
// Meta Data component fetching by component Class Name
template<typename T>
ComponentData GetComponentData() {
    HashID hash = typeid(T).hash_code();
    if (g_reflect->components.find(hash) != g_reflect->components.end()) {
        return g_reflect->components[hash];
    } else { 
        return ComponentData(); 
    }
}
// Meta Data component fetching from passed in component Instance
template<typename T>
ComponentData GetComponentData(T& component) {
    return GetComponentData<T>();
}
// Meta Data component fetching from passed in component typeid().hash_code()
ComponentData GetComponentData(HashID hash_id);
// Meta Data component fetching from passed in component name
ComponentData GetComponentData(std::string component_name);
ComponentData GetComponentData(const char* component_name);

// #################### Property Data Fetching ####################
// -------------------------    By Index  -------------------------
// Meta Data property fetching by member variable Index and component typeid().hash_code()
PropertyData GetPropertyData(HashID component_hash_id, int property_number);
// Meta Data property fetching by member variable Index and component Class Name
template<typename T>
PropertyData GetPropertyData(int property_number) {
    HashID component_hash_id = typeid(T).hash_code();
    return GetPropertyData(component_hash_id, property_number);   
}
// Meta Data property fetching by member variable Index and component Instance
template<typename T>
PropertyData GetPropertyData(T& component, int property_number) {
    return GetPropertyData<T>(property_number);   
}

// -------------------------    By Name  -------------------------
// Meta Data property fetching by member variable Name and component typeid().hash_code()
PropertyData GetPropertyData(HashID component_hash_id, std::string property_name);
// Meta Data property fetching by member variable Name and component Class Name
template<typename T>
PropertyData GetPropertyData(std::string property_name) {
    HashID component_hash_id = typeid(T).hash_code();
    return GetPropertyData(component_hash_id, property_name); 
}
// Meta Data property fetching by member variable Name and component Instance
template<typename T>
PropertyData GetPropertyData(T& component, std::string property_name) {
    return GetPropertyData<T>(property_name); 
}

// #################### Property Value Fetching ####################
// Casting from void*, not fully standardized across compilers?
//      DrVec3 rotation = *(DrVec3*)(component_ptr + prop_data.offset);
// Memcpy
//      DrVec3 value;
//      memcpy(&value, component_ptr + prop_data.offset, prop_data.size);
// C++ way
//      static constexpr auto offset_rotation = &Transform2D::rotation;
//      DrVec3 rotation = ((&et)->*off_rot);
// ##### Internal casting function
template<typename ReturnType>
ReturnType InternalGetProperty(char* component_ptr, PropertyData prop_data) {
    assert(prop_data.name != "unknown" && "Could not find property by name!");
    assert(prop_data.hash_code == typeid(ReturnType).hash_code() && "Did not request proper return type!");
    return *(reinterpret_cast<ReturnType*>(component_ptr + prop_data.offset));
}
// Get member variable value from Component by Index
template<typename ReturnType, typename ComponentType>
ReturnType GetProperty(ComponentType& component, int property_number) {
    PropertyData prop_data = GetPropertyData<ComponentType>(property_number);
    return InternalGetProperty<ReturnType>((char*)(&component), GetPropertyData<ComponentType>(property_number));
}
// Get member variable value from Component by Name
template<typename ReturnType, typename ComponentType>
ReturnType GetProperty(ComponentType& component, std::string property_name) {
    return InternalGetProperty<ReturnType>((char*)(&component), GetPropertyData<ComponentType>(property_name));
}
// Get member variable value using Component HashID by Index
template<typename ReturnType>
ReturnType GetProperty(void* component, HashID component_hash_id, int property_number) {
    return InternalGetProperty<ReturnType>((char*)(component), GetPropertyData(component_hash_id, property_number));
}
// Get member variable value using Component HashID by Name
template<typename ReturnType>
ReturnType GetProperty(void* component, HashID component_hash_id, std::string property_name) {
    return InternalGetProperty<ReturnType>((char*)(component), GetPropertyData(component_hash_id, property_name));
}

// #################### Property Value Setting ####################
// Memcpy
//     char *p = block;
//     memcpy(p + offset, &val, sizeof(val));
// ##### Internal casting function
template<typename PropertyType>
void InternalSetProperty(char* component_ptr, PropertyData prop_data, PropertyType value) {
    assert(prop_data.name != "unknown" && "Could not find property by name!");
    assert(prop_data.hash_code == typeid(PropertyType).hash_code() && "Did not request proper value type!");
    PropertyType& existing = *(reinterpret_cast<PropertyType*>(component_ptr + prop_data.offset));
    existing = value;
}
// Set member variable of component by Index
template<typename PropertyType, typename ComponentType>
void SetProperty(ComponentType& component, int property_number, PropertyType value) {
    InternalSetProperty<PropertyType>((char*)(&component), GetPropertyData<ComponentType>(property_number), value);
}
// Set member variable of component by Name
template<typename PropertyType, typename ComponentType>
void SetProperty(ComponentType& component, std::string property_name, PropertyType value) {
    InternalSetProperty<PropertyType>((char*)(&component), GetPropertyData<ComponentType>(property_name), value);
}
// Set member variable value using Component HashID by Index
template<typename PropertyType>
void SetProperty(void* component, HashID component_hash_id, int property_number, PropertyType value) {
    InternalSetProperty<PropertyType>((char*)(component), GetPropertyData(component_hash_id, property_number), value);
}
// Set member variable value using Component HashID by Name
template<typename PropertyType>
void SetProperty(void* component, HashID component_hash_id, std::string property_name, PropertyType value) {
    InternalSetProperty<PropertyType>((char*)(component), GetPropertyData(component_hash_id, property_name), value);
}

//####################################################################################
//##    Macros for Reflection Registration
//####################################################################################
// Static variable added to class allows Registration Function to be added to list of Components to be registered
#define REFLECT() \
    static bool reflection; \
    static bool initReflection();

// Define Registration Function
#define REFLECT_STRUCT(TYPE) \
    template <> void RegisterClass<TYPE>() { \
        using T = TYPE; \
        ComponentData comp {}; \
			comp.name = #TYPE; \
			comp.hash_code = typeid(TYPE).hash_code(); \
			comp.title = #TYPE; \
            CreateTitle(comp.title); \
		RegisterComponent<T>(comp); \
		int property_number = 0; \
		std::unordered_map<int, PropertyData> props { };

// Meta data functions
#define STRUCT_META_TITLE(STRING) 		comp.title = 		#STRING;	RegisterComponent(comp);
#define STRUCT_META_DESCRIPTION(STRING) comp.description = 	#STRING; 	RegisterComponent(comp);

// Property Registration
#define REFLECT_MEMBER(MEMBER) \
		property_number++; \
		props[property_number] = PropertyData(); \
		props[property_number].name = #MEMBER; \
		props[property_number].hash_code = typeid(T::MEMBER).hash_code(); \
		props[property_number].offset = offsetof(T, MEMBER); \
		props[property_number].size = sizeof(T::MEMBER); \
		props[property_number].title = #MEMBER; \
        CreateTitle(props[property_number].title); \
		RegisterProperty<decltype(T::MEMBER)>(comp, props[property_number]); 

// Meta data functions
#define MEMBER_META_TITLE(STRING) 		props[property_number].title = 			#STRING;	RegisterProperty(comp, props[property_number]); 
#define MEMBER_META_DESCRIPTION(STRING) props[property_number].description = 	#STRING; 	RegisterProperty(comp, props[property_number]); 
#define MEMBER_META_TYPE(INT) 			props[property_number].type = 			INT; 		RegisterProperty(comp, props[property_number]); 

// Static definitions add Registration Function to list of Components to be registered
#define REFLECT_END(TYPE) \
    } \
    bool TYPE::reflection { initReflection() }; \
    bool TYPE::initReflection() { \
        g_register_list.push_back(std::bind(&RegisterClass<TYPE>)); \
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
DrReflect*              g_reflect           { nullptr };                            // Meta data singleton
Functions               g_register_list     { };                                    // Keeps list of registration functions

// ########## General Registration ##########
// Initializes global reflection object, registers classes with Reflection System
void InitializeReflection() {
    // Create Singleton
    g_reflect = new DrReflect();                                                    
    
    // Register Structs / Classes
    for (int func = 0; func < g_register_list.size(); ++func) {
        g_register_list[func]();       
    }   
}

// Used in registration macros to automatically create nice display name from class / member variable names
void CreateTitle(std::string& name) {
    std::replace(name.begin(), name.end(), '_', ' '); \
    name[0] = toupper(name[0]);
    for (int c = 1; c < name.length(); c++) {
        if (name[c - 1] == ' ') name[c] = toupper(name[c]);
    }
}

// ########## Class / Member Registration ##########
// Update Component data
void RegisterComponent(ComponentData comp_data) { 
	g_reflect->AddMetaComponent(comp_data); 
}

// Update Property data
void RegisterProperty(ComponentData comp_data, PropertyData prop_data) {                     
	g_reflect->AddMetaProperty(comp_data, prop_data); 
} 

// ########## Component Data Fetching ##########
// Meta Data component fetching from passed in component typeid().hash_code()
ComponentData GetComponentData(HashID hash_id) {
    for (auto& pair : g_reflect->components) {
        if (pair.first == hash_id) return pair.second;
    }
    return ComponentData();
}
// Meta Data component fetching from passed in component name
ComponentData GetComponentData(std::string component_name) {
    for (auto& pair : g_reflect->components) {
        if (pair.second.name == component_name) return pair.second;
    }
    return ComponentData();
}
// Meta Data component fetching from passed in component name
ComponentData GetComponentData(const char* component_name) {
    return GetComponentData(std::string(component_name));
}

// ########## Property Data Fetching ##########
// Meta Data property fetching by member variable Index and component typeid().hash_code()
PropertyData GetPropertyData(HashID component_hash_id, int property_number) {
    int count = 0;
    for (auto& prop : g_reflect->properties[component_hash_id]) {
        if (count == property_number) return prop.second;
        ++count;
    }
    return PropertyData();    
}
// Meta Data property fetching by member variable Name and component typeid().hash_code()
PropertyData GetPropertyData(HashID component_hash_id, std::string property_name) {
    for (auto& prop : g_reflect->properties[component_hash_id]) {
        if (prop.second.name == property_name) return prop.second;
    }
    return PropertyData();
}

#endif  // REGISTER_REFLECTION
#endif  // DR_REFLECT_H