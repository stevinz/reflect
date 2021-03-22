//
// Description:     Reflect, C++ 11 Reflection Library
// Author:          Stephens Nunnally and Scidian Software
// License:         Distributed under the MIT License
// Source(s):       https://github.com/stevinz/reflect
//
// Copyright (c) 2021 Stephens Nunnally and Scidian Software
//
//
//####################################################################################
//####################################################################################
//####################################################################################
//######################## INCLUDE ALL REFLECTED CLASSES BELOW #######################
#define REGISTER_REFLECTION        
#include "../include/reflect.h"

// ##### User Structs #####
#include "transform.h"





//####################################################################################
//##    Demo
//####################################################################################
// Demo Includes
#include <iostream>

// Main
int main(int argc, char* argv[]) {


    // ########## Turn on reflection
    InitializeReflection();


    // ########## Create class instance
    Transform t { };
        t.width =       10;
        t.height =      20;
        t.position =    std::vector<double>({1.0, 2.0, 3.0});
        t.rotation =    std::vector<double>({4.0, 5.0, 6.0});
        t.scale =       std::vector<double>({7.0, 8.0, 9.0});
        t.text =        "hello world!";
       

    // ########## EXAMPLE: Get meta data by class type / name, member variable name / index
    std::cout << "Class Name: "         << GetClassData<Transform>().name           << std::endl;
    std::cout << "Class Description: "  << GetClassData("Transform").name           << std::endl;
    std::cout << "Class Description: "  << GetClassData<Transform>().name           << std::endl;
    std::cout << "  Member Name:   "    << GetMemberData<Transform>("width").name   << std::endl;
    std::cout << "  Member About:  "    << GetMemberData<Transform>(0).name         << std::endl;

    // ########## EXAMPLE: Get meta data by class instance, member variable name / index
    std::cout << "Class Name: "         << GetClassData(t).name                     << std::endl;
    std::cout << "Class Description: "  << GetClassData(t).name                     << std::endl;
    std::cout << "  Member Name:   "    << GetMemberData(t, "position").name        << std::endl;
    std::cout << "  Member About:  "    << GetMemberData(t, 2).name                 << std::endl;   



    // ########## EXAMPLE: GetValue by Index
    int width = GetValue<int>(t, 0);
    std::cout << "Transform t values:" << std::endl;
    std::cout << "  Width: " << width << std::endl;

    // ########## EXAMPLE: GetValue by Name
    std::vector<double> position =  GetValue<std::vector<double>>(t, "position");
    std::string         txt =       GetValue<std::string>(t, "text");      
    std::cout << "  Position X: " << position[0] << ", Position Y: " << position[1] << ", Position Z: " << position[2] << std::endl;
    std::cout << "  Text: " << txt << std::endl;



    // ########## EXAMPLE: Iterating Members
    std::cout << "Iterating Members: " << std::endl;
    for (int p = 0; p < GetClassData("Transform").member_count; ++p) {
        std::cout << "  Member Index: " << p << ", Name: " << GetMemberData(t, p).name << ", Value: ";
        HashID type = GetMemberData(t, p).hash_code;
        if        (type == MEMBER_TYPE_INT) {
            std::cout << GetValue<int>(t, p);                    
        } else if (type == MEMBER_TYPE_VECTOR_DOUBLE) {    
            std::cout << GetValue<std::vector<double>>(t, p)[0];
        } else if (type == MEMBER_TYPE_STRING) {
            std::cout << GetValue<std::string>(t, p);
        }
        std::cout << std::endl;
    }



    // ########## EXAMPLE: SetValue by Name (can also be called by Index)
    position = { 56.0, 58.5, 60.2 };
    SetValue(t, "position", position);
    std::cout << "After calling SetValue on 'position':" << std::endl;
    std::cout << "  Position X: " << t.position[0] << ", Position Y: " << t.position[1] << ", Position Z: " << t.position[2] << std::endl;



    // ########## EXAMPLE: GetValue from Unknown Types
    //  
    //  If using with entity component system its possible you may not have access to class type at runtime. 
    //  Somewhere in code when your class is initialized store the class HashID:                              
    //
            HashID hash = GetClassData(t).hash_code;
            void*  component_ptr = (void*)(&t);
    //  
    //  Later if your class is stored as void* in an array/vector/list with other classes, you may still get
    //  the member variables using only the Component type hash code:                                                                         
    //
    std::cout << "Getting member variable value from unknown class type:" << std::endl;
    HashID type = GetMemberData(hash, 3).hash_code;
    if (type == MEMBER_TYPE_VECTOR_DOUBLE) {
        std::vector<double> rotation = GetValue<std::vector<double>>(component_ptr, hash, 3);
        std::cout << "  Rotation X: " << rotation[0] << ", Rotation Y: " << rotation[1] << ", Rotation Z: " << rotation[2] << std::endl;
    }


    // ########## END DEMO
}
